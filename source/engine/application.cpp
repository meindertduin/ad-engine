#include "application.h"

#include "game/universe.h"
#include "engine.h"
#include "logging.h"
#include "game/transform.h"
#include "game/ecs.h"
#include "profiler.h"

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

    mScene = game::Universe::createInstance();
    mScene->initialize();

    Engine::initialize();

    return true;
}

void Application::run() {
    while (!mWindow->closed()) {
        Profiler::instance().begin("Frame start");
        mWindow->pollEvents();
        Profiler::instance().writeEntry("Poll events");
        mWindow->setupFrame();
        Profiler::instance().writeEntry("Setup frame");
        mScene->render();
        Profiler::instance().writeEntry("Render");
        mWindow->renderFrame();
        Profiler::instance().writeEntry("Render frame");
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

