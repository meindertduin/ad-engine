#include <fstream>
#include <queue>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "render_pipeline.h"
#include "game/transform.h"
#include "texture.h"
#include "shader_manager.h"
#include "texture_manager.h"
#include "material_manager.h"

#include "gpu/gpu.h"

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
        { 50.0f,  50.0f, 0.0f, 1.0f, 1.0f, }, // top right
        { 50.0f, -50.0f, 0.0f, 1.0f, 0.0f, }, // bottom right
        { -50.0f, -50.0f, 0.0f, 0.0f, 0.0f, }, // bottom left
        { -50.0f,  50.0f, 0.0f, 0.0f, 1.0f }, // top left
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
        {
        }

        ~RenderPipelineImpl() override {
            glDeleteBuffers(1, &mEBO);
        }

        void initialize() override {
            PosTextVertex::init();

            glGenBuffers(1, &mEBO);
            mVertexBuffer = std::make_unique<gpu::VertexBuffer>(vertices, sizeof(vertices), PosTextVertex::layout);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        }

        void renderCommand(const RenderCommand &command) override {
            mRenderCommands.push(command);
        }

        void beforeRender() override {
            constexpr glm::vec3 at = { 0.0f, 0.0f, 0.0f };
            constexpr glm::vec3 eye = { 0.0f, 0.0f, 10.0f };

            // Set view and projection matrix for view 0.
            mView = glm::lookAt(eye, at, glm::vec3(0.0f, 1.0f, 0.0f));

            auto halfWidth = float(mWidth) / 2.0f;
            auto halfHeight = float(mHeight) / 2.0f;

            mView *= glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.0f, 1000.0f);
        }

        void renderFrame() override {
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            while (!mRenderCommands.empty()) {
                auto const &command = mRenderCommands.front();

                int textureHandle = 0;
                for (auto texture : command.material->textures()) {
                    texture->render(textureHandle++);
                }

                command.material->shader()->bind();

                auto model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(command.transform->x(), command.transform->y(), 0.0f));

                auto view = glm::mat4(1.0f);
                auto projection = glm::mat4(1.0f);

                auto halfWidth = float(mWidth) / 2.0f;
                auto halfHeight = float(mHeight) / 2.0f;

                projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.0f, 1000.0f);
                view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));

                gpu::setUniform(command.material->shader()->programHandle(), "model", model);
                gpu::setUniform(command.material->shader()->programHandle(), "view", view);
                gpu::setUniform(command.material->shader()->programHandle(), "projection", projection);

                mVertexBuffer->draw();

                mRenderCommands.pop();
            }
        }

        void resize(math::Size2D frameDimensions) override {
            mWidth = frameDimensions.width();
            mHeight = frameDimensions.height();

            glViewport(0, 0, mWidth, mHeight);
        }

    private:
        uint32_t mWidth;
        uint32_t mHeight;

        uint32_t mEBO;

        std::unique_ptr<gpu::VertexBuffer> mVertexBuffer;

        glm::mat4x4 mView;

        std::queue<RenderCommand> mRenderCommands;
    };

    std::unique_ptr<RenderPipeline> RenderPipeline::createInstance(Allocator &allocator, math::Size2D frameDimensions) {
        return std::make_unique<RenderPipelineImpl>(allocator, frameDimensions);
    }
}
