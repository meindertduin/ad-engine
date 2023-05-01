#pragma once

#include "node.h"
#include "engine/path.h"

namespace game {
    class Scene {
    public:
        explicit Scene(Universe *universe, const Path &path);

        [[nodiscard]] constexpr ALWAYS_INLINE Node *root() const { return mRoot; }
        Node* getNodeById(uint32_t id);
        Node* getNodeByName(const std::string &name);

        Node* createNode(NodeType type, const std::string &name, Node *parent);
    private:
        Node *mRoot;
        Universe *mUniverse;

        std::unordered_map<uint32_t, Node*> mNodesMap;
        std::unordered_map<std::string, uint32_t> mNodeIdsMap;

        void addNode(Node *node);
        void removeNode(Node *node);
    };
}
