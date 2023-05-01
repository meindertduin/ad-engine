#include "scene_tree_node.h"
#include "imgui/imgui.h"

namespace editor {
    void SceneTreeNode::update() {
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        ImGuiTreeNodeFlags node_flags = base_flags;

        if (mNode->children().empty()) {
            node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        }

        auto nodeId = mNode->id();
        bool node_open = ImGui::TreeNodeEx((void*)(intptr_t) nodeId, node_flags, mNode->name().c_str());
        if (node_open) {
            for (auto &child : mChildren) {
                child.update();
            }
        }
        if (node_open && !mChildren.empty())
            ImGui::TreePop();
    }

    SceneTreeNode::SceneTreeNode(game::Scene *scene, game::Node *node)
            : mScene(scene)
            , mNode(node)
    {
        for (auto &child : node->children()) {
            mChildren.emplace_back(scene, child);
        }
    }
}