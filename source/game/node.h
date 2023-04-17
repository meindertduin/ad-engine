#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include "object.h"
#include "transform.h"

namespace game {
    class Scene;

    class Node {
    public:
        explicit Node(Scene *scene);
        virtual ~Node();

        void addChild(Node *child);
        void addParent(Node *parent);

        void removeChild(Node *child) {
            auto it = std::find(mChildren.begin(), mChildren.end(), child);
            if (it != mChildren.end()) {
                mChildren.erase(it);
            }
        }

        void move(const glm::vec3 &position);
    protected:
        friend class NodeIterator;

        Object mObject;

        Node *mParent { nullptr };
        Scene *mScene { nullptr };

        Transform mRelativeTransform;

        // May optimize with a hash map where nodes have an id
        std::vector<Node*> mChildren;

        void setWorldTransform();
    };

    class SpriteNode : public Node {
    public:
        explicit SpriteNode(Scene *scene);
    };

    class NodeIterator {
    public:
        NodeIterator(Node *node)
            : mNode(node)
        {
        }

        void each(const std::function<void(Node*)>& callback) {
            callback(mNode);
            for (auto child : mNode->mChildren) {
                NodeIterator(child).each(callback);
            }
        }
    private:
        Node *mNode { nullptr };
    };
}