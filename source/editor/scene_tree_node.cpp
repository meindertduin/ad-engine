#include "scene_tree_node.h"
#include "imgui/imgui.h"

namespace editor {
    struct SceneTreeDragPayload {
        SceneTreeNode *treeNode;
    };

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
        static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        ImGuiTreeNodeFlags nodeFlags = baseFlags;

        if (mNode->children().empty()) {
            nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        auto nodeId = mNode->id();
        bool nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t) nodeId, nodeFlags, mNode->name().c_str());
        bool hasChildren = !mNode->children().empty();

        if (ImGui::BeginDragDropSource()) {
            SceneTreeDragPayload payload { this };
            ImGui::SetDragDropPayload("node", &payload, sizeof(SceneTreeDragPayload));
            ImGui::Text("%s", mNode->name().c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("node")) {
                IM_ASSERT(payload->DataSize == sizeof(SceneTreeDragPayload));
                SceneTreeDragPayload dragPayload = *(SceneTreeDragPayload*) payload->Data;
                dragPayload.treeNode->changeParent(this);
            }
            ImGui::EndDragDropTarget();
        }

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

        if (nodeOpen && !mIsDeleted) {
            for (const auto &child : mChildren) {
                child->update();
            }
        }
        if (nodeOpen && hasChildren)
            ImGui::TreePop();
    }

    void SceneTreeNode::createChild(game::NodeType type, const std::string &name) {
        auto node = mScene->createNode(type, name, mNode);

        mChildren.push_back(std::make_shared<SceneTreeNode>(this, mScene, node));
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

    void SceneTreeNode::addChild(const std::shared_ptr<SceneTreeNode>& child) {
        mChildren.push_back(child);
    }

    void SceneTreeNode::changeParent(SceneTreeNode *newParent) {
        if (newParent == mParent) {
            return;
        }
        auto sharedNodePointer = mParent->findChild(mNode->name());
        mParent->removeChild(this);

        auto parentNode = mNode->parent();
        parentNode->removeChild(mNode);

        mParent = newParent;
        mParent->node()->addChild(mNode);

        newParent->addChild(sharedNodePointer);
    }

    std::shared_ptr<SceneTreeNode> SceneTreeNode::findChild(const std::string &name) {
        for (const auto &child : mChildren) {
            if (child->mNode->name() == name) {
                return child;
            }
        }
        return nullptr;
    }
}