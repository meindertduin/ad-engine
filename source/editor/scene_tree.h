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

        void createChild(game::NodeType type, const std::string &name) override;
        void removeChild(const SceneTreeObject *node) override;
        std::shared_ptr<SceneTreeNode> findChild(const std::string &name) override;

        game::Node* node() override { return nullptr; }
    private:
        game::Scene *mScene { nullptr };
        bool mHasRoot { false };
        std::shared_ptr<SceneTreeNode> mRoot { nullptr };
        std::unique_ptr<NodePopup> mPopup { nullptr };
    };
}
