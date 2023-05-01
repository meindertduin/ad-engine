#include "scene_tree.h"
#include "imgui/imgui.h"

namespace editor {
    void SceneTree::update() {
        if (!ImGui::Begin("Scene")) {
            ImGui::End();
            return;
        }

        ImGui::SeparatorText("Node Tree");

        if (!mHasRoot) {
            if (ImGui::Button("New +"))
                ImGui::OpenPopup("my_select_popup");

            ImGui::SameLine();
            auto &names = game::getNodeTypes();
            if (ImGui::BeginPopup("my_select_popup"))
            {
                ImGui::SeparatorText("Select node");
                for (int i = names.size(); i-- > 0; )
                    if (ImGui::Selectable(names[i].c_str())) {
                        auto type = game::stringToNodeTypeMap.at(names[i]);
                        mRoot = SceneTreeNode(mScene, mScene->createNode(type, "root", nullptr));
                    }

                ImGui::EndPopup();
            }

            ImGui::End();
        }

        if (mRoot) {
            mRoot->update();
        }

        ImGui::End();
    }

    SceneTree::SceneTree(game::Scene *scene)
        : mScene(scene)
        , mHasRoot(scene->root() != nullptr)
    {
        if (mHasRoot)
            mRoot = SceneTreeNode(mScene, scene->root());
    }
}


