#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include "object.h"
#include "transform.h"

namespace game {
    class Universe;

    enum class NodeType {
        Node,
        SpriteNode,
    };

    static const std::unordered_map<NodeType, std::string> nodeTypeToStringMap = {
        { NodeType::Node, "Node" },
        { NodeType::SpriteNode, "Sprite" },
    };

    static const std::unordered_map<std::string, NodeType> stringToNodeTypeMap = {
        { "Node", NodeType::Node },
        { "Sprite", NodeType::SpriteNode },
    };

    const std::vector<std::string>& getNodeTypes();

    class Node {
    public:
        Node(Universe *universe, const Transform &transform);
        virtual ~Node();

        [[nodiscard]] constexpr ALWAYS_INLINE const std::string& name() const { return mName; }
        constexpr ALWAYS_INLINE void setName(const std::string &name) { mName = name; }

        [[nodiscard]] constexpr ALWAYS_INLINE const Transform& transform() const { return mRelativeTransform; }
        constexpr ALWAYS_INLINE void setTransform(const Transform &transform) { mRelativeTransform = transform; }

        [[nodiscard]] constexpr ALWAYS_INLINE const Object& object() const { return mObject; }
        [[nodiscard]] constexpr ALWAYS_INLINE uint32_t id() const { return mObject.id(); }

        [[nodiscard]] constexpr ALWAYS_INLINE Node *parent() const { return mParent; }

        [[nodiscard]] constexpr ALWAYS_INLINE const std::vector<Node*>& children() const { return mChildren; }


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
        Universe *mUniverse {nullptr };
        std::string mName;

        Transform mRelativeTransform;

        // May optimize with a hash map where nodes have an id
        std::vector<Node*> mChildren;

        void setWorldTransform();
    };

    class SpriteNode : public Node {
    public:
        SpriteNode(Universe *scene, const Transform &transform);
    };

    class NodeIterator {
    public:
        explicit NodeIterator(Node *node)
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