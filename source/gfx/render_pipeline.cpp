#include <bx/math.h>
#include <bgfx/bgfx.h>
#include "render_pipeline.h"

#include "texture.h"

#include <fstream>

namespace gfx {
    bgfx::ShaderHandle loadShader(const char* _name) {
        char* data = new char[2048];
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
        const bgfx::Memory* mem = bgfx::copy(data,fileSize+1);
        mem->data[mem->size-1] = '\0';
        bgfx::ShaderHandle handle = bgfx::createShader(mem);
        bgfx::setName(handle, _name);
        return handle;
    }

    struct PosColorVertex {
        float m_x;
        float m_y;
        float m_z;
        uint32_t m_abgr;

        static void init() {
            ms_decl
                    .begin()
                    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
                    .end();
        };

        static bgfx::VertexLayout ms_decl;
    };

    bgfx::VertexLayout PosColorVertex::ms_decl;

    static PosColorVertex s_cubeVertices[] =
    {
            {  0.5f,  0.5f, 0.0f, 0xff0000ff },
            {  0.5f, -0.5f, 0.0f, 0xff0000ff },
            { -0.5f, -0.5f, 0.0f, 0xff00ff00 },
            { -0.5f,  0.5f, 0.0f, 0xff00ff00 }
    };

    static const uint16_t s_cubeTriList[] =
            {
                    0,1,3,
                    1,2,3
            };

    class RenderPipelineImpl : public RenderPipeline {
    public:
        RenderPipelineImpl(int width, int height)
            : mWidth(width)
            , mHeight(height)
        {
        }

        void initialize() override {
            PosColorVertex::init();

            mTexture = Texture::loadFromFile("assets/bricks.png");

            mVbh = bgfx::createVertexBuffer(
                    // Static data can be passed with bgfx::makeRef
                    bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)),
                    PosColorVertex::ms_decl
            );

            mIbh = bgfx::createIndexBuffer(
                    // Static data can be passed with bgfx::makeRef
                    bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
            );

            mFbh.idx = bgfx::kInvalidHandle;

            bgfx::ShaderHandle vsh = loadShader("shaders/build/v_simple.bin");
            bgfx::ShaderHandle fsh = loadShader("shaders/build/f_simple.bin");

            mProgram = bgfx::createProgram(vsh, fsh,  true);

            // Reset window
            bgfx::reset(mWidth, mHeight, BGFX_RESET_VSYNC);

            // Enable debug text.
            bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);

            // Set view rectangle for 0th view
            bgfx::setViewRect(0, 0, 0, uint16_t(mWidth), uint16_t(mHeight));

            // Clear the view rect
            bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);

            // Set empty primitive on screen
            bgfx::touch(0);
        }

        void render() override {
            constexpr bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
            constexpr bx::Vec3 eye = { 0.0f, 0.0f, 10.0f };

            // Set view and projection matrix for view 0.
            float view[16];
            bx::mtxLookAt(view, eye, at);

            float proj[16];
            bx::mtxProj(proj, 60.0f, float(mWidth)/float(mHeight), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

            bgfx::setViewTransform(0, view, proj);

            // Set view 0 default viewport.
            bgfx::setViewRect(0, 0, 0, mWidth, mHeight);
            bgfx::setViewFrameBuffer(0, mFbh);

            bgfx::touch(0);

            float mtx[16];
            bx::mtxRotateY(mtx, 0.0f);

            // position x,y,z
            mtx[12] = 0.0f;
            mtx[13] = 0.0f;
            mtx[14] = 0.0f;

            bx::mtxTranslate(mtx, -4.0f, 4.0f, 0.0f);

            // Set model matrix for rendering.
            bgfx::setTransform(mtx);

            // Set vertex and index buffer.
            bgfx::setVertexBuffer(0, mVbh);
            bgfx::setIndexBuffer(mIbh);

            // Set render states.
            bgfx::setState(BGFX_STATE_DEFAULT);

            // Submit primitive for rendering to view 0.
            bgfx::submit(0, mProgram);

            bgfx::frame();
        }

    private:
        int mWidth;
        int mHeight;

        bgfx::VertexBufferHandle mVbh;
        bgfx::IndexBufferHandle mIbh;
        bgfx::FrameBufferHandle mFbh;

        bgfx::ProgramHandle mProgram;
        std::unique_ptr<Texture> mTexture { nullptr };
    };

    std::unique_ptr<RenderPipeline> RenderPipeline::createInstance(int width, int height) {
        return std::make_unique<RenderPipelineImpl>(width, height);
    }
}
