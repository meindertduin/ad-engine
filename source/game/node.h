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

        [[nodiscard]] constexpr ALWAYS_INLINE const std::unordered_map<std::string, Node*>& children() const { return mChildrenMap; }


        void addChild(Node *child);
        void addParent(Node *parent);

        void removeChild(Node *child) {
            if (mChildrenMap.contains(child->name())) {
                mChildrenMap.erase(child->name());
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

        std::unordered_map<std::string, Node*> mChildrenMap;

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
            for (const auto &[name, child] : mNode->children()) {
                NodeIterator(child).each(callback);
            }
        }
    private:
        Node *mNode { nullptr };
    };
}