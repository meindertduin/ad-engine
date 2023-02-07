#include <fstream>

#include "engine/window.h"
#include "gfx/render_pipeline.h"
#include "engine/engine.h"

#include "game/scene.h"
#include "game/transform.h"
#include "game/ecs.h"

#include <iostream>

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

auto main() -> int {
    AdWindow window { math::Size { WIDTH, HEIGHT }, "Ad Render Demo" };
    window.initialize();

    auto scene = game::Scene::createInstance(Engine::instance().allocator());

    auto object = scene->createObject();
    printf("Object id: %d \n", object.id());

    object.addComponent(game::Transform { 1.0f, 1.0f });
    auto &transform = object.getComponent<game::Transform>();

    printf("Transform: %f %f \n", transform.x(), transform.y());

    auto renderPipeline = gfx::RenderPipeline::createInstance(WIDTH, HEIGHT);
    renderPipeline->initialize();

    while(!window.closed()) {
        window.pollEvents();

        renderPipeline->render();
    }

    Engine::instance().shutdown();

    return 0;
}