#include <fstream>

#include "engine/logging.h"
#include "engine/window.h"
#include "game/universe.h"
#include "engine/application.h"
#include "engine/engine.h"

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1080;

auto main() -> int {
    LOG_DEBUG("Application starting");

    WindowOptions options { .title = "Application", .size = { WIDTH, HEIGHT } };
    auto app = Application::createInstance(options);

    if (!app->initialize()) {
        return 1;
    }

    app->run();

    Engine::shutdown();

    return 0;
}