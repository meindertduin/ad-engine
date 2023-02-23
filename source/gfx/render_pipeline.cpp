#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <fstream>

#include "render_pipeline.h"
#include "material_manager.h"
#include "game/transform.h"
#include "texture.h"
#include "shader_manager.h"

namespace gfx {
    constexpr int MaxShaderParams = 16;

    bgfx::ShaderHandle loadShader(const char* _name) {
        char data[2048];
        std::ifstream file;
        size_t fileSize;

        file.open(_name);

        if(file.is_open()) {
            file.seekg(0, std::ios::end);
            fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            file.read(data, fileSize);
            file.close();
        }

        const bgfx::Memory* mem = bgfx::copy(data, fileSize+1);
        mem->data[mem->size-1] = '\0';
        bgfx::ShaderHandle handle = bgfx::createShader(mem);
        bgfx::setName(handle, _name);

        return handle;
    }

    struct PosTextVertex {
        float x;
        float y;
        float z;
        float tx;
        float ty;

        static void init() {
            ms_decl
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                .end();
        };

        static inline bgfx::VertexLayout ms_decl;
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
        explicit RenderPipelineImpl(math::Size2D frameDimensions)
            : mWidth(frameDimensions.width())
            , mHeight(frameDimensions.height())
        {
        }

        ~RenderPipelineImpl() override {
            bgfx::destroy(mTextureUniform);

            bgfx::destroy(mIbh);
            bgfx::destroy(mVbh);
        }

        void initialize() override {
            PosTextVertex::init();

            mShaderParamsUniformHandle = bgfx::createUniform("u_params", bgfx::UniformType::Vec4, MaxShaderParams);
            mTextureUniform = bgfx::createUniform("v_texCoord0", bgfx::UniformType::Sampler);

            mVbh = bgfx::createVertexBuffer(
                bgfx::makeRef(sCubeVertices, sizeof(sCubeVertices)),
                PosTextVertex::ms_decl
            );

            mIbh = bgfx::createIndexBuffer(bgfx::makeRef(sCubeTriList, sizeof(sCubeTriList)));

            mFbh.idx = bgfx::kInvalidHandle;

            // Reset window
            bgfx::reset(mWidth, mHeight, BGFX_RESET_VSYNC);

            // Enable debug text.
            bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);

            bgfx::setViewRect(0, 0, 0, uint16_t(mWidth), uint16_t(mHeight));
            bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);

            bgfx::touch(0);
        }

        void beforeRender() override {
            constexpr bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
            constexpr bx::Vec3 eye = { 0.0f, 0.0f, 10.0f };

            // Set view and projection matrix for view 0.
            float view[16];
            bx::mtxLookAt(view, eye, at);

            float proj[16];

            auto halfWidth = float(mWidth) / 2.0f;
            auto halfHeight = float(mHeight) / 2.0f;

            bx::mtxOrtho(proj, -halfWidth, halfWidth, -halfHeight, halfHeight, 0.1f, 1000.0f, 0.0f, bgfx::getCaps()->homogeneousDepth);

            bgfx::setViewTransform(0, view, proj);

            // Set view 0 default viewport.
            bgfx::setViewRect(0, 0, 0, mWidth, mHeight);
            bgfx::setViewFrameBuffer(0, mFbh);

            bgfx::touch(0);
        }

        void renderObject(const game::Transform &transform, RenderComponent &component) override {
            float mtx[16];
            bx::mtxRotateY(mtx, 0.0f);

            // position x,y,z
            mtx[12] = transform.x();
            mtx[13] = transform.y();
            mtx[14] = 0.0f;

            // Set model matrix for rendering.
            bgfx::setTransform(mtx);

            // Set vertex and index buffer.
            bgfx::setVertexBuffer(0, mVbh);
            bgfx::setIndexBuffer(mIbh);

            component.texture()->render(mTextureUniform);
            bgfx::setState(BGFX_STATE_DEFAULT);

            float params[8] = { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
            bgfx::setUniform(mShaderParamsUniformHandle, params, MaxShaderParams);

            component.material()->shader()->bind(0);
        }

        void afterRender() override {
            bgfx::frame();
        }

        void resize(math::Size2D frameDimensions) override {
           mWidth = frameDimensions.width();
           mHeight = frameDimensions.height();

            bgfx::reset(mWidth, mHeight, BGFX_RESET_VSYNC);

            bgfx::setViewRect(0, 0, 0, mWidth, mHeight);
            bgfx::setViewFrameBuffer(0, mFbh);

            bgfx::touch(0);
        }

    private:
        int mWidth;
        int mHeight;

        bgfx::VertexBufferHandle mVbh;
        bgfx::IndexBufferHandle mIbh;
        bgfx::FrameBufferHandle mFbh;

        bgfx::UniformHandle mTextureUniform = BGFX_INVALID_HANDLE;
        bgfx::UniformHandle mShaderParamsUniformHandle = BGFX_INVALID_HANDLE;
    };

    std::unique_ptr<RenderPipeline> RenderPipeline::createInstance(math::Size2D frameDimensions) {
        return std::make_unique<RenderPipelineImpl>(frameDimensions);
    }
}
