#pragma once

#include <memory>
#include "texture.h"
#include "render_component.h"

namespace gfx {
    class RenderPipeline {
    public:
        static std::unique_ptr<RenderPipeline> createInstance(int width, int height);

        virtual void initialize() = 0;

        virtual void beforeRender() = 0;
        virtual void renderObject(RenderComponent &component) = 0;
        virtual void afterRender() = 0;

        virtual ~RenderPipeline() = default;
    };
}
