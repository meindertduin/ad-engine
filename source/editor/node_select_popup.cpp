#include "node_select_popup.h"
#include "imgui/imgui.h"

namespace editor {
    NodeSelectPopup::NodeSelectPopup(const char *id, const std::function<void(game::NodeType, const std::string &name)> &onNodeSelect)
        : mId(id)
        , mOnNodeSelect(onNodeSelect)
    {
        ImGui::OpenPopup(mId);
    }

    bool NodeSelectPopup::update() {
        auto open = ImGui::BeginPopup(mId);
        if (open) {
            ImGui::SeparatorText("Select node");
            auto &names = game::getNodeTypes();

            for (int i = names.size(); i-- > 0; )
                if (ImGui::Selectable(names[i].c_str())) {
                    auto type = game::stringToNodeTypeMap.at(names[i]);
                    std::string name = "node";
                    mOnNodeSelect(type, name);
                    ImGui::CloseCurrentPopup();
                }

            ImGui::EndPopup();

        }

        return open;
    }
}