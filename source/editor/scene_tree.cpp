#include "scene_tree.h"
#include "imgui/imgui.h"

namespace editor {
    SceneTree::SceneTree(game::Scene *scene)
            : mScene(scene)
            , mHasRoot(scene->root() != nullptr)
    {
        if (mHasRoot)
            mRoot = std::make_unique<SceneTreeNode>(this, mScene, scene->root());
    }

    void SceneTree::update() {
        if (!ImGui::Begin("Scene")) {
            ImGui::End();
            return;
        }

        ImGui::SeparatorText("Node Tree");

        if (!mHasRoot) {
            if (ImGui::Button("New +"))
                mPopup = std::make_unique<NodeNewPopup>("new_root_popup", this);

            if (mPopup && !mPopup->update()) {
                mPopup = nullptr;
            }
        }

        if (mRoot) {
            mRoot->update();
        }

        ImGui::End();
    }

    void SceneTree::removeChild(const SceneTreeObject *node) {
        mRoot = nullptr;
        mHasRoot = false;
    }

    void SceneTree::createChild(game::NodeType type, const std::string &name) {
        auto node = mScene->createNode(type, name, nullptr);
        mRoot = std::make_unique<SceneTreeNode>(this, mScene, node);
    }

    std::shared_ptr<SceneTreeNode> SceneTree::findChild(const std::string &name) {
        if (mRoot && mRoot->node()->name() == name)
            return mRoot;

        return nullptr;
    }
}


