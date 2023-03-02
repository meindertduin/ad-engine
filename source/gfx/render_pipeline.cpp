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

    constexpr PosTextVertex sCubeVertices[] =
    {
        // x       y     z     tx    ty
        {  50.0f,  50.0f, 0.0f, 1.0f, 1.0f },
        {  50.0f, -50.0f, 0.0f, 1.0f, 0.0f },
        { -50.0f, -50.0f, 0.0f, 0.0f, 0.0f },
        { -50.0f,  50.0f, 0.0f, 0.0f, 1.0f }
    };

    constexpr uint16_t sCubeTriList[] =
    {
        0,1,3,
        1,2,3
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
            // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
            glBindVertexArray(mVAO);

            glBindBuffer(GL_ARRAY_BUFFER, mVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(sCubeVertices), sCubeVertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sCubeTriList), sCubeTriList, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

            // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
            // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
            glBindVertexArray(0);
        }

        void renderCommand(const RenderCommand &command) override {
            mRenderCommands.push(command);
        }

        void beforeRender() override {
            constexpr glm::vec3 at = { 0.0f, 0.0f, 0.0f };
            constexpr glm::vec3 eye = { 0.0f, 0.0f, 0.0f };

            // Set view and projection matrix for view 0.
            mView = glm::lookAt(eye, at, glm::vec3(0.0f, 1.0f, 0.0f));

            auto halfWidth = float(mWidth) / 2.0f;
            auto halfHeight = float(mHeight) / 2.0f;

            mProjection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.0f, 1000.0f);
        }

        void renderFrame() override {
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            while (!mRenderCommands.empty()) {
                auto const &command = mRenderCommands.front();

                command.material->shader()->bind();

                // std::array<float, 16> mtx {};
                // bx::mtxRotateY(mtx.data(), 0.0f);

                // // position x,y,z
                // mtx[12] = command.transform->x();
                // mtx[13] = command.transform->y();
                // mtx[14] = 0.0f;

                // // Set model matrix for rendering.
                // bgfx::setTransform(mtx.data());

                // bgfx::setVertexBuffer(0, mVbh);
                // bgfx::setIndexBuffer(mIbh);

                // auto &textures = command.material->textures();
                // textures[0]->render(mTextureUniform);
                // bgfx::setState(BGFX_STATE_DEFAULT);

                // std::array<float, 8> params = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
                // bgfx::setUniform(mShaderParamsUniformHandle, params.data(), MaxShaderParams);

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
        glm::mat4x4 mProjection;

        std::queue<RenderCommand> mRenderCommands;
    };

    std::unique_ptr<RenderPipeline> RenderPipeline::createInstance(Allocator &allocator, math::Size2D frameDimensions) {
        return std::make_unique<RenderPipelineImpl>(allocator, frameDimensions);
    }
}
