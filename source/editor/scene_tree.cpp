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
                ImGui::OpenPopup("my_select_popup");

            ImGui::SameLine();
            auto &names = game::getNodeTypes();
            if (ImGui::BeginPopup("my_select_popup"))
            {
                ImGui::SeparatorText("Select node");
                for (int i = names.size(); i-- > 0; )
                    if (ImGui::Selectable(names[i].c_str())) {
                        auto type = game::stringToNodeTypeMap.at(names[i]);
                        mRoot = std::make_unique<SceneTreeNode>(this, mScene, mScene->createNode(type, "root", nullptr));
                        mHasRoot = true;
                    }

                ImGui::EndPopup();
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
}


