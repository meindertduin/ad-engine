#include "node.h"
#include "universe.h"
#include "ecs.h"
#include "gfx/render_component.h"

namespace game {
    const std::vector<std::string>& getNodeTypes() {
        static bool initialized = false;
        static std::vector<std::string> types;
        if (initialized) {
            return types;
        }
        for (const auto &[key, value] : nodeTypeToStringMap) {
            types.push_back(value);
        }
        initialized = true;

        return types;
    }

    Node::Node(Universe *universe, const Transform &transform)
        : mObject(universe->createObject())
        , mUniverse(universe)
        , mRelativeTransform(transform)
    {
        mObject.addComponent(game::Transform(0, 0, 0));

        Logger::info("Object: {}", mObject);
    }

    Node::~Node() {
        mUniverse->destroyObject(mObject);
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