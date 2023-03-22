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

#include "camera.h"

namespace gfx {
    struct PosTextVertex {
        glm::vec3 position;
        glm::vec2 texCoords;

        static void init() {
            layout
                .addAttribute(gpu::Attribute::Position, gpu::AttributeType::Vec3, false)
                .addAttribute(gpu::Attribute::TexCoord, gpu::AttributeType::Vec2, false);
        }

        static inline gpu::VertexLayout layout;
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
            using enum gpu::AttributeType;

            PosTextVertex::init();

            gpu::SharedBufferLayout layout;
            layout
                .addAttribute("direction", Vec3, sizeof(glm::vec3))
                .addAttribute("ambient", Vec3, sizeof(glm::vec3))
                .addAttribute("diffuse", Vec3, sizeof(glm::vec3))
                .addAttribute("specular", Vec3, sizeof(glm::vec3));

            mSharedUniformBuffer = gpu::SharedUniformBuffer::create(LightsBlockBinding, layout);

            auto direction = glm::vec3(0.0f, -1.0f, 0.0f);
            auto ambient = glm::vec3(0.2f, 0.2f, 0.2f);
            auto diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
            auto specular = glm::vec3(1.0f, 1.0f, 1.0f);

            mSharedUniformBuffer->setData("direction", gpu::BufferDataPointer(&direction, sizeof(glm::vec3)));
            mSharedUniformBuffer->setData("ambient", gpu::BufferDataPointer(&ambient, sizeof(glm::vec3)));
            mSharedUniformBuffer->setData("diffuse", gpu::BufferDataPointer(&diffuse, sizeof(glm::vec3)));
            mSharedUniformBuffer->setData("specular", gpu::BufferDataPointer(&specular, sizeof(glm::vec3)));
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
                model = glm::translate(model, command.transform->position());

                gpu::setUniform(command.material->shader()->programHandle(), "model", model);
                gpu::setUniform(command.material->shader()->programHandle(), "view", mCamera.view());
                gpu::setUniform(command.material->shader()->programHandle(), "projection", mCamera.projection());
                gpu::setUniform(command.material->shader()->programHandle(), "invtransmodel", glm::inverse(glm::transpose(model)));

                command.mesh->draw();

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

        Camera mCamera;

        std::queue<RenderCommand> mRenderCommands;
        std::unique_ptr<gpu::SharedUniformBuffer> mSharedUniformBuffer;
    };

    std::unique_ptr<RenderPipeline> RenderPipeline::createInstance(Allocator &allocator, math::Size2D frameDimensions) {
        return std::make_unique<RenderPipelineImpl>(allocator, frameDimensions);
    }
}
