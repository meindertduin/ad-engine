#include "window.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <utility>

const int WIDTH = 640;
const int HEIGHT = 480;

AdWindow::~AdWindow() {
    bgfx::shutdown();

    SDL_DestroyWindow(pWindow);
    SDL_Quit();
}

bool AdWindow::initialize() {
    // Initialize SDL systems
    if(SDL_Init( SDL_INIT_VIDEO ) < 0)
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    else
    {
        //Create a window
        pWindow = SDL_CreateWindow( "BGFX Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN );
        if(pWindow == nullptr)
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        }
    }

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(pWindow, &wmi))
    {
        return false;
    }

    bgfx::PlatformData pd;
    // and give the pointer to the window to pd
    pd.ndt = wmi.info.x11.display;
    pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;

    bgfx::Init init;
    init.platformData = pd;

    bgfx::renderFrame();

    bgfx::init(init);

    return false;
}

AdWindow::AdWindow(const math::SizeU2 &size, std::string title) :
    mSize(size),
    mTitle(std::move(title))
{
}
