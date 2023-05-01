#include <fstream>
#include <queue>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "render_pipeline.h"
#include "texture.h"
#include "shader_manager.h"
#include "texture_manager.h"
#include "material_manager.h"

#include "light.h"
#include "camera.h"
#include "engine/profiler.h"

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
        explicit RenderPipelineImpl(math::Size2D frameDimensions)
            : mCamera(frameDimensions)
        {
        }

        void initialize() override {
            PosTextVertex::init();

            DirLight dirLight {};
            std::vector<DirLight> dirLights;

            dirLight.direction = glm::vec3(0.0f, -1.0f, -1.0f);
            dirLight.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
            dirLight.diffuse = glm::vec3(0.3f, 0.3f, 0.3f);
            dirLight.specular = glm::vec3(1.0f, 1.0f, 1.0f);
            dirLights.push_back(dirLight);

            PointLight pointLight {};
            std::vector<PointLight> pointLights;

            pointLight.position = glm::vec3(0.0f, 1.0f, 3.0f);
            pointLight.ambient = glm::vec3(0.1f, 0.1f, 0.1f);
            pointLight.diffuse = glm::vec3(0.3f, 0.3f, 0.3f);
            pointLight.specular = glm::vec3(0.8f, 0.8f, 0.8f);
            pointLight.constant = 1.0f;
            pointLight.linear = 0.3f;
            pointLight.quadratic = 0.032f;

            pointLights.push_back(pointLight);

            mLights = Lights(dirLights, pointLights);
            mLights.setBufferData();
        }

        void renderCommand(const RenderCommand &command) override {
            mRenderCommands.push(command);
        }

        void renderFrame() override {
            gpu::clear();
            Profiler::instance().writeEntry("Render start");

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
        uint32_t mWidth = 0;
        uint32_t mHeight = 0;

        Camera mCamera;

        std::queue<RenderCommand> mRenderCommands;
        Lights mLights;
    };

    std::unique_ptr<RenderPipeline> RenderPipeline::createInstance(math::Size2D frameDimensions) {
        return std::make_unique<RenderPipelineImpl>(frameDimensions);
    }
}
