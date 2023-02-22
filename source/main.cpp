#include <fstream>

#include "engine/logging.h"
#include "engine/window.h"
#include "game/scene.h"
#include "engine/application.h"
#include "engine/engine.h"

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

auto main() -> int {
    LOG_DEBUG("Application starting");

    WindowOptions options { .title = "Application", .size = { WIDTH, HEIGHT } };
    Application app(options);

    if (!app.initialize()) {
        return 1;
    }

    app.run();

    Engine::instance().shutdown();

    return 0;
}