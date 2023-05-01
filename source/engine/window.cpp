#include "window.h"
#include "logging.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include "gpu/gpu.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"

class AdWindowImpl : public AdWindow {
public:
    explicit AdWindowImpl(const WindowOptions &options)
            : mSize(options.size)
            , mTitle(options.title)
    {
    }

    ~AdWindowImpl() override {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(mContext);

        SDL_DestroyWindow(pWindow);
        SDL_Quit();
    }

    AdWindowImpl(AdWindowImpl &) = delete;
    AdWindowImpl&operator=(AdWindowImpl &) = delete;

    bool initialize() override {
        // Initialize SDL systems
        if(SDL_Init(SDL_INIT_VIDEO) < 0) {
            Logger::error("Failed to initialize SDL: {}", SDL_GetError());
            return false;
        }

        // TODO set attributes based on platform etc
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        //Create a window
        pWindow = SDL_CreateWindow(mTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mSize.width(), mSize.height(), SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if(pWindow == nullptr) {
            Logger::error("Failed to create SDL window: {}", SDL_GetError());
            return false;
        }

        mContext = SDL_GL_CreateContext(pWindow);
        if(mContext == nullptr) {
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

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.Fonts->AddFontFromFileTTF("assets/fonts/intelone-mono-font-family-regular.ttf", 20.0f);

        ImGui::StyleColorsDark();

        ImGui_ImplSDL2_InitForOpenGL(pWindow, mContext);

        // TODO parse the actual openGL version
        const char* glsl_version = "#version 330";
        ImGui_ImplOpenGL3_Init(glsl_version);

        return true;
    }

    void setupFrame() override {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();
    }

    void renderFrame() override {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(pWindow);
    }

    void pollEvents() override {
        SDL_Event currentEvent;

        while(SDL_PollEvent(&currentEvent)) {
            ImGui_ImplSDL2_ProcessEvent(&currentEvent);
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

    uint32_t getTicks() const override {
        return SDL_GetTicks();
    }

    math::Size2D size() const override {
        return mSize;
    }

    bool closed() const override {
        return mClosed;
    }

    Observable<WindowEvent> &windowEvent() override {
        return mWindowEventObservable;
    }

private:
    SDL_Window* pWindow { nullptr };
    SDL_GLContext mContext;
    math::Size2D mSize;
    std::string mTitle;
    bool mClosed { false };

    Observable<WindowEvent> mWindowEventObservable;
};

std::unique_ptr<AdWindow> AdWindow::createInstance(const WindowOptions &options) {
    return std::make_unique<AdWindowImpl>(options);
}
