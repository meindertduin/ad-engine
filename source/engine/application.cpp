#include "application.h"

#include "game/universe.h"
#include "engine.h"
#include "logging.h"
#include "game/transform.h"
#include "game/ecs.h"
#include "profiler.h"
#include "editor/editor.h"

Application::Application(const WindowOptions &options)
    : mWindow(AdWindow::createInstance(options))
{
}

bool Application::initialize() {
    if (sInitialized) {
        Logger::error("Application already initialized");
        return false;
    }

    if (!mWindow->initialize()) {
        return false;
    }

    Profiler::createInstance(mWindow.get());

    sInitialized = true;

    mUniverse = game::Universe::createInstance();
    mUniverse->initialize();
    mEditor = std::make_unique<editor::Editor>(this);

    Engine::initialize();

    return true;
}

void Application::run() {
    while (!mWindow->closed()) {
        mWindow->pollEvents();
        mWindow->setupFrame();
        mEditor->update();
        mUniverse->render();
        mWindow->renderFrame();
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

