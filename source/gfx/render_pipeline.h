#pragma once

#include <memory>
#include "texture.h"
#include "render_component.h"
#include "math/size.h"

namespace game { class Transform; }

namespace gfx {
    struct RenderCommand {
        Material *material;
        Texture2D *texture;
        game::Transform *transform;
    };

    class RenderPipeline {
    public:
        static std::unique_ptr<RenderPipeline> createInstance(Allocator &allocator, math::Size2D frameDimensions);

        virtual void initialize() = 0;

        virtual void renderCommand(const RenderCommand &command) = 0;
        virtual void renderFrame() = 0;

        virtual void resize(math::Size2D frameDimensions) = 0;

        virtual ~RenderPipeline() = default;
    };
}
