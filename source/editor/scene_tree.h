#pragma once

#include "game/universe.h"
#include "game/scene.h"
#include "scene_tree_node.h"
#include "node_select_popup.h"

namespace editor {
    class SceneTree {
    public:
        explicit SceneTree(game::Scene *scene);

        void update();
    private:
        game::Scene *mScene { nullptr };
        bool mHasRoot { false };
        std::unique_ptr<SceneTreeNode> mRoot { nullptr };
    };
}
