#pragma once

#include <memory>
#include "texture.h"
#include "render_component.h"
#include "math/size.h"
#include "mesh.h"
#include "game/transform.h"

namespace gfx {
    struct RenderCommand {
        Material *material;
        game::Transform transform;
        Mesh *mesh;
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
