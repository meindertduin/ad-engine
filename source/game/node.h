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

        void setParent(Node *parent) {
            mParent = parent;
        }

        void addChild(Node *child) {
            mChildren.push_back(child);
        }

        void removeChild(Node *child) {
            auto it = std::find(mChildren.begin(), mChildren.end(), child);
            if (it != mChildren.end()) {
                mChildren.erase(it);
            }
        }
    private:
        Object mObject;

        friend class NodeIterator;
        Node *mParent { nullptr };

        // May optimize with a hash map where nodes have an id
        std::vector<Node*> mChildren;
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