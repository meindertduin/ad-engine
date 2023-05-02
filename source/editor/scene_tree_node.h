#pragma once

#include "game/scene.h"
#include "node_select_popup.h"

namespace editor {
    class SceneTreeNode {
    public:
        explicit SceneTreeNode(game::Scene *scene, game::Node *node);

        void update();

        [[nodiscard]] ALWAYS_INLINE constexpr game::Node* node() const { return mNode; }
        [[nodiscard]] ALWAYS_INLINE constexpr game::Scene* scene() const { return mScene; }
        void addChild(game::Node *node);
    private:
        game::Scene *mScene { nullptr };
        game::Node *mNode { nullptr };

        std::unique_ptr<NodeSelectPopup> mPopup { nullptr };

        std::vector<SceneTreeNode> mChildren;
    };
}
