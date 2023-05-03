#pragma once

#include "game/scene.h"
#include "node_select_popup.h"
#include "scene_tree_object.h"

namespace editor {
    class SceneTreeNode : public SceneTreeObject {
    public:
        SceneTreeNode(SceneTreeObject *parent, game::Scene *scene, game::Node *node);
        ~SceneTreeNode() override;

        SceneTreeNode(SceneTreeNode &other) = delete;
        SceneTreeNode& operator=(const SceneTreeNode &other) = delete;

        void update();

        [[nodiscard]] ALWAYS_INLINE constexpr game::Node* node() const { return mNode; }
        [[nodiscard]] ALWAYS_INLINE constexpr game::Scene* scene() const { return mScene; }

        void createChild(game::NodeType type, const std::string &name) override;
        void removeChild(const SceneTreeObject *node) override;

        std::shared_ptr<SceneTreeNode> findChild(const std::string &name) override;

        void deleteNode();
        void renameNode(const std::string &name);

        void addChild(const std::shared_ptr<SceneTreeNode>& child);
        void changeParent(SceneTreeNode *parent);

        game::Node* node() override { return mNode; }
    private:
        SceneTreeObject *mParent { nullptr };
        game::Scene *mScene;
        game::Node *mNode;

        bool mIsDeleted { false };

        std::unique_ptr<NodePopup> mPopup { nullptr };

        std::vector<std::shared_ptr<SceneTreeNode>> mChildren;
    };
}
