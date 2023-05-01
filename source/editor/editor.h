#pragma once

#include "scene_tree.h"

class Application;

namespace editor {
    class Editor {
    public:
        explicit Editor(Application *application);
        void update();
    private:
        SceneTree mNodeTree;
    };
}