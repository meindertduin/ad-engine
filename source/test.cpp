#include <cstdio>

#include "math/size.h"
#include "engine/window.h"
#include "gpu/render_target.h"

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

int main(int argc, char **argv) {
    WindowOptions options { .title = "Application", .size = { WIDTH, HEIGHT } };
    AdWindow window { options };

    if (!window.initialize()) {
        return 1;
    }

    gpu::RenderTarget renderTarget;

    while (!window.closed()) {
        window.pollEvents();

        renderTarget.renderTest();

        window.swapBuffers();
    }

    math::Size2D size { 1, 2 };
    printf("%d %d\n", size.width(), size.height());

    return 0;
}