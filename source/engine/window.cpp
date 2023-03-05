#include "window.h"
#include "logging.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include "gpu/gpu.h"

AdWindow::AdWindow(const WindowOptions &options)
    : mSize(options.size)
    , mTitle(options.title)
{
}

AdWindow::~AdWindow() {
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
}

bool AdWindow::initialize() {
    // Initialize SDL systems
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        Logger::error("Failed to initialize SDL: {}", SDL_GetError());
        return false;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    //Create a window
    pWindow = SDL_CreateWindow(mTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mSize.width(), mSize.height(), SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(pWindow == nullptr) {
        Logger::error("Failed to create SDL window: {}", SDL_GetError());
        return false;
    }

    auto context = SDL_GL_CreateContext(pWindow);
    if(context == nullptr) {
        Logger::error("Failed to create OpenGL context: {}", SDL_GetError());
        return false;
    }

    gpu::initialize();

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version)
    if (!SDL_GetWindowWMInfo(pWindow, &wmi)) {
        Logger::error("Failed to get SDL window info: {}", SDL_GetError());
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

void AdWindow::swapBuffers() {
    SDL_GL_SwapWindow(pWindow);
}

