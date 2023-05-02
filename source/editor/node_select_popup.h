#pragma once

#include <functional>
#include "game/node.h"
#include "game/scene.h"

namespace editor {
    class SceneTreeNode;

    class NodeSelectPopup {
    public:
        NodeSelectPopup(const char *id, SceneTreeNode *node);
        bool update();
    private:
        const char *mId;
        SceneTreeNode *mNode;

        char mNewNodeName[32] = "node";
        int mNewNodeTypeIndex = 0;
        void renderButtonAndPopup();
    };
}