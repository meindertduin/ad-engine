#pragma once

#include "node.h"
#include "engine/path.h"

namespace game {
    class Scene {
    public:
        explicit Scene(Universe *universe, const Path &path);
        ~Scene();

        [[nodiscard]] constexpr ALWAYS_INLINE Node *root() const { return mRoot; }
        Node* getNodeById(uint32_t id);

        Node* createNode(NodeType type, const std::string &name, Node *parent);

        void removeNode(Node *node);
        void renameNode(Node *node, const std::string &name);
    private:
        Node *mRoot;
        Universe *mUniverse;

        std::unordered_map<uint32_t, Node*> mNodesMap;

        void addNode(Node *node);
    };
}
