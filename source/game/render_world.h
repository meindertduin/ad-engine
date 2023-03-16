#pragma once

#include <functional>
#include "object.h"
#include "gfx/sprite.h"
#include "scene.h"

#include "math/size.h"
#include "gfx/render_pipeline.h"
#include "terrain.h"

namespace game {
    class RenderWorld {
    public:
        explicit RenderWorld(Scene &scene, Allocator &allocator, const math::Size2D &frameDimensions);
        void resize(const math::Size2D &frameDimensions);
        void render();
    private:
        Scene &mScene;
        Terrain mTerrain;
        std::unique_ptr<gfx::Mesh> mMesh;
        std::unique_ptr<gfx::RenderPipeline> mRenderPipeline;
    };
}
