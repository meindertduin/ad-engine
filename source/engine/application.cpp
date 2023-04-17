#include "application.h"

#include "game/universe.h"
#include "engine.h"
#include "logging.h"
#include "game/transform.h"
#include "game/ecs.h"

Application::Application(const WindowOptions &options)
    : mWindow(options)
{
}

bool Application::initialize() {
    if (sInitialized) {
        Logger::error("Application already initialized");
        return false;
    }

    if (!mWindow.initialize()) {
        return false;
    }

    sInitialized = true;

    mScene = game::Universe::createInstance(Engine::instance().allocator());
    mScene->initialize();

    Engine::initialize();

    return true;
}

void Application::run() {
    while (!mWindow.closed()) {
        mWindow.pollEvents();

        mScene->render();

        mWindow.swapBuffers();
    }
}

Application *Application::createInstance(const WindowOptions &options) {
    if (sInstance) {
        Logger::error("Application already created");
        return nullptr;
    }

    sInstance = new Application(options);

    return sInstance;
}

Application *Application::instance() {
    assert(sInstance && "Application not created");
    assert(sInitialized && "Application not initialized");

    return sInstance;
}

