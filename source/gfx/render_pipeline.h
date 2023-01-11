#pragma once

#include <memory>
#include "texture.h"

namespace gfx {
    class RenderPipeline {
    public:
        static std::unique_ptr<RenderPipeline> createInstance(int width, int height);

        virtual void initialize() = 0;
        virtual void render() = 0;

        virtual ~RenderPipeline() = default;
    };
}
