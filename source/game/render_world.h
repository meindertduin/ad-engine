#pragma once

#include <functional>
#include "object.h"
#include "gfx/sprite.h"
#include "universe.h"

#include "math/size.h"
#include "gfx/render_pipeline.h"
#include "terrain.h"

namespace game {
    class RenderWorld {
    public:
        explicit RenderWorld(Universe &scene, Allocator &allocator, const math::Size2D &frameDimensions);
        void resize(const math::Size2D &frameDimensions);
        void render();
        void renderTerrain();
    private:
        Universe &mScene;
        Terrain mTerrain;
        std::unique_ptr<gfx::RenderPipeline> mRenderPipeline;
    };
}
