#pragma once

#include <memory>
#include "texture.h"
#include "render_component.h"

namespace game { class Transform; }

namespace gfx {
    class RenderPipeline {
    public:
        static std::unique_ptr<RenderPipeline> createInstance(int width, int height);

        virtual void initialize() = 0;

        virtual void beforeRender() = 0;
        virtual void renderObject(const game::Transform &transform, RenderComponent &component) = 0;
        virtual void afterRender() = 0;

        virtual ~RenderPipeline() = default;
    };
}
