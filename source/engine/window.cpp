#include "window.h"
#include "logging.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

AdWindow::AdWindow(const WindowOptions &options)
    : mSize(options.size)
    , mTitle(options.title)
{
}

AdWindow::~AdWindow() {
    bgfx::shutdown();

    SDL_DestroyWindow(pWindow);
    SDL_Quit();
}

bool AdWindow::initialize() {
    // Initialize SDL systems
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        Logger::error("Failed to initialize SDL: {}", SDL_GetError());
        return false;
    }
    else {
        //Create a window
        pWindow = SDL_CreateWindow(mTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mSize.width(), mSize.height(), SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if(pWindow == nullptr) {
            Logger::error("Failed to create SDL window: {}", SDL_GetError());
            return false;
        }
    }

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version)
    if (!SDL_GetWindowWMInfo(pWindow, &wmi)) {
        Logger::error("Failed to get SDL window info: {}", SDL_GetError());
        return false;
    }

    bgfx::PlatformData pd;
    // and give the pointer to the window to pd
    pd.ndt = wmi.info.x11.display;
    pd.nwh = (void*) wmi.info.x11.window;

    bgfx::Init init;
    init.platformData = pd;

    bgfx::renderFrame();

    if (!bgfx::init(init)) {
        Logger::error("Failed to initialize bgfx");
        return false;
    }

    return true;
}

void AdWindow::pollEvents() {
    SDL_Event currentEvent;

    while(SDL_PollEvent(&currentEvent) != 0) {
        if(currentEvent.type == SDL_QUIT) {
            mClosed = true;
        }

        if (currentEvent.type == SDL_WINDOWEVENT) {
            if (currentEvent.window.event == SDL_WINDOWEVENT_RESIZED) {
                mSize = math::Size2D {currentEvent.window.data1, currentEvent.window.data2};

                mWindowEventObservable.notify({ WindowEvent::Type::Resize, mSize });
            }
        }
    }
}

