#pragma once

#include "game/scene.h"
#include "node_select_popup.h"

namespace editor {
    class SceneTreeNode {
    public:
        SceneTreeNode(SceneTreeNode *parent, game::Scene *scene, game::Node *node);
        ~SceneTreeNode();

        void update();

        [[nodiscard]] ALWAYS_INLINE constexpr game::Node* node() const { return mNode; }
        [[nodiscard]] ALWAYS_INLINE constexpr game::Scene* scene() const { return mScene; }

        void createChild(game::Node *node);
        void removeChild(const SceneTreeNode *node);

        void deleteNode();
    private:
        SceneTreeNode *mParent { nullptr };
        game::Scene *mScene;
        game::Node *mNode;

        std::unique_ptr<NodeSelectPopup> mPopup { nullptr };

        std::vector<std::unique_ptr<SceneTreeNode>> mChildren;
    };
}
