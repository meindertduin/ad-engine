#include <fstream>

#include "engine/window.h"
#include "gfx/render_pipeline.h"
#include "engine/engine.h"

#include "game/scene.h"
#include "game/ecs.h"
#include "gfx/render_component.h"
#include "game/transform.h"

#include <iostream>

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

auto main() -> int {
    AdWindow window { math::Size { WIDTH, HEIGHT }, "Ad Render Demo" };
    window.initialize();

    auto scene = game::Scene::createInstance(Engine::instance().allocator());

    auto object = scene->createObject();
    printf("Object id: %d \n", object.id());
    object.addComponent(gfx::RenderComponent { Path { "assets/material_scripts/material.lua" }, Path { "assets/bricks.png" } });
    object.addComponent(game::Transform { 0.0f, 0.0f });

    auto secondObject = scene->createObject();
    printf("Object id: %d \n", secondObject.id());
    secondObject.addComponent(gfx::RenderComponent { Path { "assets/material_scripts/material.lua" }, Path { "assets/bricks.png" } });
    secondObject.addComponent(game::Transform { 200.0f, 100.0f });

    auto renderPipeline = gfx::RenderPipeline::createInstance(WIDTH, HEIGHT);
    renderPipeline->initialize();

    while(!window.closed()) {
        window.pollEvents();

        scene->render();
    }

    Engine::instance().shutdown();

    return 0;
}