#include "scene_tree_node.h"
#include "imgui/imgui.h"

namespace editor {
    SceneTreeNode::SceneTreeNode(SceneTreeObject *parent, game::Scene *scene, game::Node *node)
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
            mPopup = std::make_unique<NodeSelectPopup>(mNode->name().c_str(), [&](NodeAction action){
                std::string name = mNode->name();
                auto id = "node_popup_select" + std::to_string(mNode->id());
                switch (action) {
                    using enum editor::NodeAction;
                    case New:
                        mPopup = std::make_unique<NodeNewPopup>(id, this);
                        break;
                    case Rename:
                        mPopup = std::make_unique<NodeRenamePopup>(id, this, name);
                        break;
                    case Delete:
                        deleteNode();
                        mIsDeleted = true;
                        break;
                }
            });
        }

        if(mPopup && !mPopup->update()) {
            mPopup = nullptr;
        }

        if (node_open && !mIsDeleted) {
            for (const auto &child : mChildren) {
                child->update();
            }
        }
        if (node_open && hasChildren)
            ImGui::TreePop();
    }

    void SceneTreeNode::createChild(game::Node *node) {
        mChildren.push_back(std::make_unique<SceneTreeNode>(this, mScene, node));
    }

    void SceneTreeNode::removeChild(const SceneTreeObject *node) {
        for (auto it = mChildren.begin(); it != mChildren.end(); it++) {
            if (it->get() == node) {
                mChildren.erase(it);
                break;
            }
        }
    }

    void SceneTreeNode::deleteNode() {
        if (mParent) {
            mParent->removeChild(this);
        }
    }

    void SceneTreeNode::renameNode(const std::string &name) {
        mScene->renameNode(mNode, name);
    }
}