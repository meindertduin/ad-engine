#include "application.h"

#include "game/scene.h"
#include "engine.h"
#include "logging.h"
#include "game/transform.h"
#include "gfx/render_component.h"
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

    mScene = game::Scene::createInstance(Engine::instance().allocator());
    mScene->initialize();

    Engine::initialize();

    return true;
}

void Application::run() {
    auto object = mScene->createObject();
    Logger::info("Object: {}", object);
    object.addComponent(gfx::RenderComponent{Path{"assets/material_scripts/material.lua"}});
    object.addComponent(game::Transform(0.5f, 0.0f, 0));

    auto secondObject = mScene->createObject();
    Logger::info("SecondObject: {} \n", secondObject);

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

