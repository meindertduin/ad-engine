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
    explicit Application(const WindowOptions &options);

    bool initialize();
    void run();

private:
    AdWindow mWindow;
    bool mInitialized { false };
    std::unique_ptr<game::Scene> mScene { nullptr };
};