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
    if (mInitialized) {
        Logger::error("Application already initialized");
        return false;
    }

    if (!mWindow.initialize()) {
        return false;
    }

    mScene = game::Scene::createInstance(Engine::instance().allocator());

    mInitialized = true;

    return true;
}

void Application::run() {
    auto object = mScene->createObject();
    Logger::info("Object: {}", object);
    object.addComponent(gfx::RenderComponent{Path{"assets/material_scripts/material.lua"}, Path{"assets/bricks.png"}});
    object.addComponent(game::Transform{20.0f, 20.0f});

    auto secondObject = mScene->createObject();
    Logger::info("SecondObject: {} \n", secondObject);
    secondObject.addComponent(
            gfx::RenderComponent{Path{"assets/material_scripts/material.lua"}, Path{"assets/bricks.png"}});
    secondObject.addComponent(game::Transform{200.0f, 100.0f});

    while (!mWindow.closed()) {
        mWindow.pollEvents();

        mScene->render();
    }
}

