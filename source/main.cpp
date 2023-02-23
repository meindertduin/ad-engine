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
    auto app = Application::createInstance(options);

    Observable<std::string> observable;
    auto observer = observable.subscribe([](const std::string &value) {
        Logger::info("Observer: {}", value);
    });

    observable.notify("Hello");
    observable.notify("World");
    observer->~Observer();

    if (!app->initialize()) {
        return 1;
    }

    app->run();

    Engine::instance().shutdown();

    return 0;
}