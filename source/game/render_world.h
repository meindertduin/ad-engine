#pragma once

#include "object.h"
#include "gfx/sprite.h"
#include "scene.h"

#include "gfx/render_pipeline.h"

namespace game {
    class RenderWorld {
    public:
        explicit RenderWorld(Scene &scene);

        void render();
    private:
        Scene &mScene;
        std::unique_ptr<gfx::RenderPipeline> mRenderPipeline;
    };
}
