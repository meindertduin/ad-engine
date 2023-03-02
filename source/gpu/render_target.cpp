#include <GL/glew.h>
#include "render_target.h"

namespace gpu {
    RenderTarget::RenderTarget() {

    }

    void RenderTarget::renderTest() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}