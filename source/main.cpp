#include <fstream>

#include "engine/window.h"
#include "gfx/render_pipeline.h"

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

auto main() -> int {
    AdWindow window { math::Size { WIDTH, HEIGHT }, "Ad Render Demo" };
    window.initialize();

    auto renderPipeline = gfx::RenderPipeline::createInstance(WIDTH, HEIGHT);
    renderPipeline->initialize();

    while(!window.closed()) {
        window.pollEvents();

        renderPipeline->render();
    }

    return 0;
}