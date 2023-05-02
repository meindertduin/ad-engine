#pragma once

namespace editor {
    class SceneTreeObject {
    public:
        virtual ~SceneTreeObject() = default;
        virtual void removeChild(const SceneTreeObject *node) = 0;
    };
}