#include <fstream>
#include <queue>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "render_pipeline.h"
#include "game/transform.h"
#include "texture.h"
#include "shader_manager.h"
#include "texture_manager.h"
#include "material_manager.h"

#include "gpu/gpu.h"
#include "camera.h"
#include "game/terrain.h"

namespace gfx {
    struct PosTextVertex {
        float x, y, z;
        float u, v;

        static void init() {
            layout
                .addAttribute(gpu::Attribute::Position, gpu::AttributeType::Vec3, false)
                .addAttribute(gpu::Attribute::TexCoord, gpu::AttributeType::Vec2, false);
        }

        static inline gpu::VertexLayout layout;
    };

    PosTextVertex vertices[] = {
        { 1.0f,  1.0f, 0.0f, 1.0f, 1.0f, }, // top right
        { 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, }, // bottom right
        { -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, }, // bottom left
        { -1.0f,  1.0f, 0.0f, 0.0f, 1.0f }, // top left
    };

    unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
    };

    class RenderPipelineImpl : public RenderPipeline {
    public:
        explicit RenderPipelineImpl(Allocator &allocator, math::Size2D frameDimensions)
            : mWidth(frameDimensions.width())
            , mHeight(frameDimensions.height())
            , mCamera(frameDimensions)
        {
        }

        void initialize() override {
            PosTextVertex::init();
            mTerrain.initialize();

            mVertexBuffer = gpu::VertexBuffer::create(vertices, sizeof(vertices), PosTextVertex::layout);
            mIndexBuffer = gpu::IndexBuffer::create(indices, sizeof(indices));
        }

        void renderCommand(const RenderCommand &command) override {
            mRenderCommands.push(command);
        }

        void renderFrame() override {
            gpu::clear();

            while (!mRenderCommands.empty()) {
                auto const &command = mRenderCommands.front();

                int textureHandle = 0;
                for (auto texture : command.material->textures()) {
                    texture->render(textureHandle++);
                }

                command.material->shader()->bind();

                auto model = glm::mat4(1.0f);
                model = glm::scale(model, glm::vec3(5.0f));
                model = glm::translate(model, glm::vec3(command.transform->x(), command.transform->y(), 0.0f));
                model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

                gpu::setUniform(command.material->shader()->programHandle(), "model", model);
                gpu::setUniform(command.material->shader()->programHandle(), "view", mCamera.view());
                gpu::setUniform(command.material->shader()->programHandle(), "projection", mCamera.projection());

                mVertexBuffer->draw();

                mRenderCommands.pop();
            }
        }

        void resize(math::Size2D frameDimensions) override {
            mCamera.resize(frameDimensions);

            mWidth = frameDimensions.width();
            mHeight = frameDimensions.height();

            gpu::setViewport(0, 0, mWidth, mHeight);
        }

    private:
        uint32_t mWidth;
        uint32_t mHeight;

        std::unique_ptr<gpu::VertexBuffer> mVertexBuffer;
        std::unique_ptr<gpu::IndexBuffer> mIndexBuffer;

        Camera mCamera;

        std::queue<RenderCommand> mRenderCommands;
        game::Terrain mTerrain;
    };

    std::unique_ptr<RenderPipeline> RenderPipeline::createInstance(Allocator &allocator, math::Size2D frameDimensions) {
        return std::make_unique<RenderPipelineImpl>(allocator, frameDimensions);
    }
}
