#pragma once

#include "game/universe.h"
#include "game/scene.h"
#include "scene_tree_node.h"

namespace editor {
    class SceneTree {
    public:
        explicit SceneTree(game::Scene *scene);

        void update();
    private:
        game::Scene *mScene { nullptr };
        bool mHasRoot { false };

        std::optional<SceneTreeNode> mRoot { std::nullopt };
    };
}
