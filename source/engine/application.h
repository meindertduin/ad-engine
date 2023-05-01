#pragma once

#include "window.h"
#include "game/object.h"

#include <memory>

namespace game {
    class Universe;
}

namespace gfx {
    class RenderPipeline;
}

namespace editor {
    class Editor;
}

class Application {
public:
    static Application* createInstance(const WindowOptions &options);
    static Application* instance();

    bool initialize();
    void run();

    AdWindow& window() { return *mWindow; }
    game::Universe& universe() { return *mUniverse; }
protected:
    explicit Application(const WindowOptions &options);
private:
    static inline bool sInitialized { false };

    std::unique_ptr<AdWindow> mWindow;
    std::unique_ptr<game::Universe> mUniverse {nullptr };

    std::unique_ptr<editor::Editor> mEditor { nullptr };

    static inline Application* sInstance { nullptr };
};