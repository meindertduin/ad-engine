#pragma once

#include <memory>
#include "texture.h"
#include "render_component.h"
#include "math/size.h"

namespace game { class Transform; }

namespace gfx {
    class RenderPipeline {
    public:
        static std::unique_ptr<RenderPipeline> createInstance(math::Size2D frameDimensions);

        virtual void initialize() = 0;

        virtual void beforeRender() = 0;
        virtual void renderObject(const game::Transform &transform, RenderComponent &component) = 0;
        virtual void afterRender() = 0;

        virtual ~RenderPipeline() = default;
    };
}
