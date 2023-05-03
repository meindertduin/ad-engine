#pragma once

#include <functional>
#include "game/node.h"
#include "game/scene.h"

namespace editor {
    class SceneTreeNode;
    class SceneTreeObject;

    enum class NodeAction {
        New,
        Rename,
        Delete,
    };

    class NodePopup {
    public:
        NodePopup() = default;
        virtual ~NodePopup() = default;
        virtual bool update() = 0;
    };

    class NodeSelectPopup : public NodePopup {
    public:
        NodeSelectPopup(const std::string &id, const std::function<void(NodeAction)> &onSelect);
        bool update() override;
    private:
        std::string mId;
        std::function<void(NodeAction)> mOnSelect;

        bool select(NodeAction action);
    };

    class NodeRenamePopup : public NodePopup {
    public:
        NodeRenamePopup(const std::string &id, SceneTreeNode *node, const std::string &name);
        bool update() override;
    private:
        std::string mId;
        SceneTreeNode *mNode;
        char mNewNodeName[128] = "";
    };

    class NodeNewPopup : public NodePopup {
    public:
        NodeNewPopup(const std::string &id, SceneTreeObject *node);
        bool update() override;
    private:
        std::string mId;
        SceneTreeObject *mTreeNode;
        char mNewNodeName[32] = "node";
        int mNewNodeTypeIndex = 0;
    };
}