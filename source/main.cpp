#include <fstream>

#include "engine/window.h"
#include "gfx/render_pipeline.h"
#include "engine/engine.h"

#include "game/scene.h"
#include "game/ecs.h"
#include "gfx/render_component.h"
#include "game/transform.h"

#include "engine/logging.h"
#include "engine/map.h"

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

auto main() -> int {
    LOG_DEBUG("Application starting");

    AdWindow window { math::Size { WIDTH, HEIGHT }, "Ad Render Demo" };

    if (!window.initialize()) {
        return 1;
    }

    auto scene = game::Scene::createInstance(Engine::instance().allocator());

    auto object = scene->createObject();
    Logger::info("Object: {}", object);
    object.addComponent(gfx::RenderComponent { Path { "assets/material_scripts/material.lua" }, Path { "assets/bricks.png" } });
    object.addComponent(game::Transform { 0.0f, 0.0f });

    auto secondObject = scene->createObject();
    Logger::info("SecondObject: {} \n", secondObject);
    secondObject.addComponent(gfx::RenderComponent { Path { "assets/material_scripts/material.lua" }, Path { "assets/bricks.png" } });
    secondObject.addComponent(game::Transform { 200.0f, 100.0f });

    auto renderPipeline = gfx::RenderPipeline::createInstance(WIDTH, HEIGHT);
    renderPipeline->initialize();

    HashMap<int, int> map { Engine::instance().allocator() };

    map.insert(1, 2);
    map.insert(2, 3);
    map.insert(4, 3);
    Logger::info("{}", map[4]);

    while(!window.closed()) {
        window.pollEvents();

        scene->render();
    }

    Engine::instance().shutdown();

    return 0;
}