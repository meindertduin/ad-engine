#include "node.h"
#include "universe.h"
#include "ecs.h"
#include "gfx/render_component.h"

namespace game {
    Node::Node(Universe *scene, const Transform &transform)
        : mObject(scene->createObject())
        , mScene(scene)
        , mRelativeTransform(transform)
    {
        mObject.addComponent(game::Transform(0, 0, 0));

        Logger::info("Object: {}", mObject);
    }

    Node::~Node() {
        mScene->destroyObject(mObject);
    }

    void Node::addChild(Node *child) {
        mChildren.push_back(child);

        child->addParent(this);
    }

    void Node::addParent(Node *parent) {
        mParent = parent;

        setWorldTransform();
    }

    void Node::move(const glm::vec3 &position) {
        mRelativeTransform.setPosition(position);

        setWorldTransform();
    }

    void Node::setWorldTransform() {
        auto &worldTransform = mObject.getComponent<Transform>();

        if (mParent) {
            const auto &parentWorldTransform = mParent->mObject.getComponent<Transform>();
            worldTransform.setPosition(parentWorldTransform.position() + mRelativeTransform.position());
        } else {
            worldTransform.setPosition(mRelativeTransform.position());
        }
    }

    SpriteNode::SpriteNode(Universe *scene, const Transform &transform)
        : Node(scene, transform)
    {
        mObject.addComponent(gfx::RenderComponent{ Path{ "assets/material_scripts/material.lua" } });
    }
}