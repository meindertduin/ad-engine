#include "node.h"
#include "scene.h"
#include "ecs.h"
#include "gfx/render_component.h"

namespace game {
    Node::Node(Scene *scene)
        : mObject(scene->createObject())
    {
        mObject.addComponent(game::Transform(0.5f, 0.0f, 0));
        mObject.addComponent(gfx::RenderComponent{ Path{ "assets/material_scripts/material.lua" } });

        Logger::info("Object: {}", mObject);
    }
}