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
    const auto function = [](const std::string &value) {
        Logger::info("Observer: {}", value);
    };

    auto observer = observable.subscribe(function);

    observable.notify("Hello");
    observable.notify("World");

    observer->unsubscribe();

    if (!app->initialize()) {
        return 1;
    }

    app->run();

    Engine::shutdown();

    return 0;
}