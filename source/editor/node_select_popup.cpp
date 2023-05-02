#include "node_select_popup.h"
#include "imgui/imgui.h"
#include "scene_tree_node.h"

namespace editor {
    NodeSelectPopup::NodeSelectPopup(const char *id, SceneTreeNode *node)
            : mId(id)
            , mNode(node)
    {
        ImGui::OpenPopup(mId);
    }

    bool NodeSelectPopup::update() {
        auto open = ImGui::BeginPopup(mId);
        if (open) {
            renderButtonAndPopup();
            ImGui::EndPopup();
        }

        return open;
    }

    void NodeSelectPopup::renderButtonAndPopup() {
        if (ImGui::Button("New +"))
            ImGui::OpenPopup("new_node_popup");

        if (ImGui::BeginPopup("new_node_popup")) {
            ImGui::SeparatorText("Select node");
            auto &names = game::getNodeTypes();

            for (int i = 0; i < names.size(); i++) {
                if (ImGui::Selectable(names[i].c_str(), mNewNodeTypeIndex == i, ImGuiSelectableFlags_DontClosePopups)) {
                    mNewNodeTypeIndex = i;
                }
            }

            ImGui::InputText("Node name", mNewNodeName, IM_ARRAYSIZE(mNewNodeName));

            if (ImGui::Button("Create")) {
                std::string name = mNewNodeName;
                auto &names = game::getNodeTypes();
                auto type = game::stringToNodeTypeMap.at(names[mNewNodeTypeIndex]);
                auto node = mNode->scene()->createNode(type, name, mNode->node());
                mNode->addChild(node);
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        if (ImGui::Button("Delete")) {
            ImGui::CloseCurrentPopup();
        }
    }
}