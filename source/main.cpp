#include <fstream>

#include "engine/window.h"
#include "gfx/render_pipeline.h"
#include "engine/allocator.h"
#include "engine/vector.h"

#include <memory>
#include <iostream>

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

auto main() -> int {
    auto allocator = std::make_unique<ListAllocator>(1024 * 1024 * 10, ListAllocator::PlacementPolicy::First);

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