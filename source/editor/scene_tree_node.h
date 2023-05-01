#pragma once

#include "game/scene.h"

namespace editor {
    class SceneTreeNode {
    public:
        explicit SceneTreeNode(game::Scene *scene, game::Node* node);

        void update();
    private:
        game::Scene *mScene { nullptr };
        game::Node *mNode { nullptr };

        std::vector<SceneTreeNode> mChildren;
    };
}
