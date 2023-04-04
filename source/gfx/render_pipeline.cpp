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

    class Lights {
    public:
        Lights() = default;
        Lights(const std::vector<gpu::DirLight> &dirLights)
            : mDirLightsCount(static_cast<int>(dirLights.size()))
            , mDirLights(dirLights)
        {
            mLayout = std::make_unique<gpu::BufferLayout>();
            mLayout->addAttribute("dirLightsCount", sizeof(int));

            for (int i = 0; i < mDirLightsCount; ++i) {
                mLayout->addAttribute("dirLights[" + std::to_string(i) + "].direction", sizeof(glm::vec3));
                mLayout->addAttribute("dirLights[" + std::to_string(i) + "].ambient", sizeof(glm::vec3));
                mLayout->addAttribute("dirLights[" + std::to_string(i) + "].diffuse", sizeof(glm::vec3));
                mLayout->addAttribute("dirLights[" + std::to_string(i) + "].specular", sizeof(glm::vec3));
            }

            mBuffer = gpu::SharedUniformBuffer::create(LightsBlockBinding, mLayout->totalSize());
        }

        void setBufferData() {
            auto dirLightsCountOffset = mLayout->attribute("dirLightsCount").offset;

            mBuffer->setData(dirLightsCountOffset, &mDirLightsCount, sizeof(int));

            for (int i = 0; i < mDirLightsCount; i++) {
                auto directionOffset = mLayout->attribute("dirLights[" + std::to_string(i) + "].direction").offset;
                auto ambientOffset = mLayout->attribute("dirLights[" + std::to_string(i) + "].ambient").offset;
                auto diffuseOffset = mLayout->attribute("dirLights[" + std::to_string(i) + "].diffuse").offset;
                auto specularOffset = mLayout->attribute("dirLights[" + std::to_string(i) + "].specular").offset;

                mBuffer->setData(directionOffset, &mDirLights[i].direction, sizeof(glm::vec3));
                mBuffer->setData(ambientOffset, &mDirLights[i].ambient, sizeof(glm::vec3));
                mBuffer->setData(diffuseOffset, &mDirLights[i].diffuse, sizeof(glm::vec3));
                mBuffer->setData(specularOffset, &mDirLights[i].specular, sizeof(glm::vec3));
            }
        }
    private:
        std::unique_ptr<gpu::SharedUniformBuffer> mBuffer;
        std::unique_ptr<gpu::BufferLayout> mLayout;

        int mDirLightsCount;
        std::vector<gpu::DirLight> mDirLights;
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

            gpu::DirLight dirLight {};
            std::vector<gpu::DirLight> dirLights;

            dirLight.direction = glm::vec3(0.0f, -1.0f, -1.0f);
            dirLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
            dirLight.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
            dirLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
            dirLights.push_back(dirLight);

            mLights = Lights(dirLights);
            mLights.setBufferData();
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
                model = glm::translate(model, command.transform.position());

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
        Lights mLights;
    };

    std::unique_ptr<RenderPipeline> RenderPipeline::createInstance(Allocator &allocator, math::Size2D frameDimensions) {
        return std::make_unique<RenderPipelineImpl>(allocator, frameDimensions);
    }
}
