#include <cstdio>
#include <fstream>

#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <gfx/render_pipeline.h>
#include "engine/window.h"

constexpr uint32_t WIDTH = 640;
constexpr uint32_t HEIGHT = 480;

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

bgfx::VertexBufferHandle m_vbh;
bgfx::IndexBufferHandle m_ibh;
bgfx::ProgramHandle m_program;

int main ( int argc, char* args[] ) {
    AdWindow window { math::Size { WIDTH, HEIGHT }, "Ad Render Demo" };
    window.initialize();

    PosColorVertex::init();
    m_vbh = bgfx::createVertexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)),
            PosColorVertex::ms_decl
    );

    m_ibh = bgfx::createIndexBuffer(
            // Static data can be passed with bgfx::makeRef
            bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
    );

    bgfx::ShaderHandle vsh = loadShader("shaders/build/v_simple.bin");
    bgfx::ShaderHandle fsh = loadShader("shaders/build/f_simple.bin");

    m_program = bgfx::createProgram(vsh,fsh,  true);



    // Reset window
    bgfx::reset(WIDTH, HEIGHT, BGFX_RESET_VSYNC);

    // Enable debug text.
    bgfx::setDebug(BGFX_DEBUG_TEXT /*| BGFX_DEBUG_STATS*/);

    // Set view rectangle for 0th view
    bgfx::setViewRect(0, 0, 0, uint16_t(WIDTH), uint16_t(HEIGHT));

    // Clear the view rect
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);


    // Set empty primitive on screen
    bgfx::touch(0);

    // Poll for events and wait till user closes window
    bool quit = false;
    SDL_Event currentEvent;
    while(!quit) {
        while(SDL_PollEvent(&currentEvent) != 0) {
            if(currentEvent.type == SDL_QUIT) {
                quit = true;
            }

            const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
            const bx::Vec3 eye = { 0.0f, 0.0f, 10.0f };

            // Set view and projection matrix for view 0.
            float view[16];
            bx::mtxLookAt(view, eye, at);

            float proj[16];
            bx::mtxProj(proj,
                        60.0f,
                        float(WIDTH)/float(HEIGHT),
                        0.1f, 100.0f,
                        bgfx::getCaps()->homogeneousDepth);

            bgfx::setViewTransform(0, view, proj);

            // Set view 0 default viewport.
            bgfx::setViewRect(0, 0, 0,
                              WIDTH,
                              HEIGHT);

            bgfx::touch(0);


            float mtx[16];
            bx::mtxRotateY(mtx, 0.0f);

            // position x,y,z
            mtx[12] = 0.0f;
            mtx[13] = 0.0f;
            mtx[14] = 0.0f;

            // Set model matrix for rendering.
            bgfx::setTransform(mtx);

            // Set vertex and index buffer.
            bgfx::setVertexBuffer(0, m_vbh);
            bgfx::setIndexBuffer(m_ibh);

            // Set render states.
            bgfx::setState(BGFX_STATE_DEFAULT);

            // Submit primitive for rendering to view 0.
            bgfx::submit(0, m_program);

            bgfx::frame();
        }
    }

    return 0;
}