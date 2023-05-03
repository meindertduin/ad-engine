#pragma once

#include "game/scene.h"
#include "node_select_popup.h"
#include "scene_tree_object.h"

namespace editor {
    class SceneTreeNode : public SceneTreeObject {
    public:
        SceneTreeNode(SceneTreeObject *parent, game::Scene *scene, game::Node *node);
        ~SceneTreeNode() override;

        void update();

        [[nodiscard]] ALWAYS_INLINE constexpr game::Node* node() const { return mNode; }
        [[nodiscard]] ALWAYS_INLINE constexpr game::Scene* scene() const { return mScene; }

        void createChild(game::Node *node);
        void removeChild(const SceneTreeObject *node) override;

        void deleteNode();
        void renameNode(const std::string &name);
    private:
        SceneTreeObject *mParent { nullptr };
        game::Scene *mScene;
        game::Node *mNode;

        bool mIsDeleted { false };

        std::unique_ptr<NodePopup> mPopup { nullptr };

        std::vector<std::unique_ptr<SceneTreeNode>> mChildren;
    };
}
