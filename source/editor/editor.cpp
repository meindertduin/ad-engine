#include "editor.h"
#include "engine/application.h"

namespace editor {
    Editor::Editor(Application *application)
        : mNodeTree(application->universe().scene())
    {
    }

    void Editor::update() {
        mNodeTree.update();
    }
}