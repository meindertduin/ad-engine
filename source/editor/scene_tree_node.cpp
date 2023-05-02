#include "scene_tree_node.h"
#include "imgui/imgui.h"

namespace editor {
    SceneTreeNode::SceneTreeNode(SceneTreeNode *parent, game::Scene *scene, game::Node *node)
            : mParent(parent)
            , mScene(scene)
            , mNode(node)
    {
        for (auto &child : node->children()) {
            mChildren.push_back(std::make_unique<SceneTreeNode>(this, scene, child));
        }
    }

    SceneTreeNode::~SceneTreeNode() {
        mScene->removeNode(mNode);
    }

    void SceneTreeNode::update() {
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        ImGuiTreeNodeFlags node_flags = base_flags;

        if (mNode->children().empty()) {
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        auto nodeId = mNode->id();
        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t) nodeId, node_flags, mNode->name().c_str());

        bool hasChildren = !mNode->children().empty();

        if (ImGui::IsItemClicked(ImGuiMouseButton_Right) && !mPopup) {
            mPopup = std::make_unique<NodeSelectPopup>(mNode->name().c_str(), this);
        }

        if(mPopup && !mPopup->update()) {
            mPopup = nullptr;
        }

        if (node_open) {
            for (auto &child : mChildren) {
                if (child) {
                    child->update();
                }
            }
        }
        if (node_open && hasChildren)
            ImGui::TreePop();
    }

    void SceneTreeNode::createChild(game::Node *node) {
        mChildren.push_back(std::make_unique<SceneTreeNode>(this, mScene, node));
    }

    void SceneTreeNode::deleteNode() {
        if (mParent) {
            mParent->removeChild(this);
        }
    }

    void SceneTreeNode::removeChild(const SceneTreeNode *node) {
        for (auto it = mChildren.begin(); it != mChildren.end(); it++) {
            if (it->get() == node) {
                mChildren.erase(it);
                break;
            }
        }
    }
}