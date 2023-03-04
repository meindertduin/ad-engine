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

namespace gfx {
    constexpr int MaxShaderParams = 16;

    struct PosTextVertex {
        float x;
        float y;
        float z;
        float tx;
        float ty;
    };

    // constexpr PosTextVertex sCubeVertices[] =
    // {
    //     // x       y     z     tx    ty
    //     {  50.0f,  50.0f, 0.0f, 1.0f, 1.0f },
    //     {  50.0f, -50.0f, 0.0f, 1.0f, 0.0f },
    //     { -50.0f, -50.0f, 0.0f, 0.0f, 0.0f },
    //     { -50.0f,  50.0f, 0.0f, 0.0f, 1.0f }
    // };

    // constexpr PosTextVertex sCubeVertices[] =
    // {
    //         // x       y     z     tx    ty
    //         {  50.0f,  50.0f, 0.0f, 1.0f, 1.0f },
    //         {  50.0f, -50.0f, 0.0f, 1.0f, 0.0f },
    //         { -50.0f, -50.0f, 0.0f, 0.0f, 0.0f },
    //         { -50.0f,  50.0f, 0.0f, 0.0f, 1.0f }
    // };
    //
    // constexpr uint16_t sCubeTriList[] =
    // {
    //     0,1,3,
    //     1,2,3
    // };

    float vertices[] = {
            0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
            0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
            -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left
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
            glDeleteVertexArrays(1, &mVAO);
            glDeleteBuffers(1, &mVBO);
            glDeleteBuffers(1, &mEBO);
        }

        void initialize() override {
            glGenVertexArrays(1, &mVAO);
            glGenBuffers(1, &mVBO);
            glGenBuffers(1, &mEBO);

            glBindVertexArray(mVAO);

            glBindBuffer(GL_ARRAY_BUFFER, mVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
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

                auto  mtx = glm::identity<glm::mat4x4>();
                mtx = glm::translate(mtx, glm::vec3(0, 0, 0));

                // position x,y,z
                glm::translate(mtx, glm::vec3(command.transform->x(), command.transform->y(), 0.0f));

                // glUniformMatrix4fv(glGetUniformLocation(command.material->shader()->programHandle(), "model"), 1, GL_FALSE, glm::value_ptr(mtx));
                // glUniformMatrix4fv(glGetUniformLocation(command.material->shader()->programHandle(), "view"), 1, GL_FALSE, glm::value_ptr(mView));

                glBindVertexArray(mVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

        uint32_t mVAO;
        uint32_t mVBO;
        uint32_t mEBO;

        glm::mat4x4 mView;

        std::queue<RenderCommand> mRenderCommands;
    };

    std::unique_ptr<RenderPipeline> RenderPipeline::createInstance(Allocator &allocator, math::Size2D frameDimensions) {
        return std::make_unique<RenderPipelineImpl>(allocator, frameDimensions);
    }
}
