#include "node_select_popup.h"
#include "imgui/imgui.h"
#include "scene_tree_node.h"

namespace editor {
    NodeSelectPopup::NodeSelectPopup(const std::string &id, const std::function<void(NodeAction)> &onSelect)
            : mId(id)
            , mOnSelect(onSelect)
    {
        ImGui::OpenPopup(mId.c_str());
    }

    bool NodeSelectPopup::update() {
        auto open = ImGui::BeginPopup(mId.c_str());
        if (open) {
            using enum editor::NodeAction;

            if (ImGui::MenuItem("New +"))
                return select(New);

            if (ImGui::MenuItem("Delete"))
                return select(Delete);

            if (ImGui::MenuItem("Rename"))
                return select(Rename);

            ImGui::EndPopup();
        }

        return open;
    }

    bool NodeSelectPopup::select(NodeAction action) {
        ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
        mOnSelect(action);
        return true;
    }

    NodeRenamePopup::NodeRenamePopup(const std::string &id, SceneTreeNode *node, const std::string &name)
        : mId(id)
        , mNode(node)
    {
        ImGui::OpenPopup(mId.c_str());
        strcpy(mNewNodeName, name.c_str());
    }

    bool NodeRenamePopup::update() {
        auto open = ImGui::BeginPopup(mId.c_str());
        if (open) {
            ImGui::InputText("Node name", mNewNodeName, IM_ARRAYSIZE(mNewNodeName));
            if (ImGui::Button("Rename")) {
                mNode->renameNode(mNewNodeName);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        return open;
    }

    NodeNewPopup::NodeNewPopup(const std::string &id, SceneTreeObject *node)
        : mId(id)
        , mTreeNode(node)
    {
        ImGui::OpenPopup(mId.c_str());
    }

    bool NodeNewPopup::update() {
        auto open = ImGui::BeginPopup(mId.c_str());
        if (open) {
            ImGui::SeparatorText("Select node");
            auto &names = game::getNodeTypes();

            for (int i = 0; i < names.size(); i++) {
                if (ImGui::Selectable(names[i].c_str(), mNewNodeTypeIndex == i, ImGuiSelectableFlags_DontClosePopups)) {
                    mNewNodeTypeIndex = i;
                }
            }

            ImGui::InputText("Node name", mNewNodeName, IM_ARRAYSIZE(mNewNodeName));

            if (ImGui::Button("New")) {
                std::string name = mNewNodeName;
                auto &names = game::getNodeTypes();
                auto type = game::stringToNodeTypeMap.at(names[mNewNodeTypeIndex]);

                mTreeNode->createChild(type, name);

                ImGui::CloseCurrentPopup();
                ImGui::EndPopup();

                return false;
            }

            ImGui::EndPopup();
        }

        return open;
    }
}