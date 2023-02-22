#pragma once

#include "window.h"
#include "game/object.h"

#include <memory>

namespace game {
    class Scene;
}

namespace gfx {
    class RenderPipeline;
}

class Application {
public:
    static Application* createInstance(const WindowOptions &options);
    static Application* instance();

    bool initialize();
    void run();
protected:
    explicit Application(const WindowOptions &options);
private:
    static inline bool sInitialized { false };

    AdWindow mWindow;
    std::unique_ptr<game::Scene> mScene { nullptr };

    static inline Application* sInstance { nullptr };
};