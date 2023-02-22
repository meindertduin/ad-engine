#pragma once

#include <functional>
#include "object.h"
#include "gfx/sprite.h"
#include "scene.h"

#include "gfx/render_pipeline.h"
#include "math/size.h"

namespace game {

    class RenderWorld {
    public:
        explicit RenderWorld(Scene &scene, const math::Size2D &frameDimensions);

        void render();
    private:
        Scene &mScene;
        std::unique_ptr<gfx::RenderPipeline> mRenderPipeline;
    };
}
