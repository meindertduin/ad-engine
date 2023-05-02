#pragma once

#include "game/universe.h"
#include "game/scene.h"
#include "scene_tree_node.h"
#include "node_select_popup.h"
#include "scene_tree_object.h"

namespace editor {
    class SceneTree : public SceneTreeObject {
    public:
        explicit SceneTree(game::Scene *scene);

        void update();

        void removeChild(const SceneTreeObject *node) override;
    private:
        game::Scene *mScene { nullptr };
        bool mHasRoot { false };
        std::unique_ptr<SceneTreeNode> mRoot { nullptr };
    };
}
