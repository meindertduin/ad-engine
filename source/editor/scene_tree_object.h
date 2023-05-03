#pragma once

namespace editor {
    class SceneTreeObject {
    public:
        virtual ~SceneTreeObject() = default;
        virtual void removeChild(const SceneTreeObject *node) = 0;
        virtual void createChild(game::NodeType type, const std::string &name) = 0;
        virtual std::shared_ptr<SceneTreeNode> findChild(const std::string &name) = 0;

        virtual game::Node* node() = 0;
    };
}