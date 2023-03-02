#pragma once

#include <cstdint>
#include "gfx/shader.h"

namespace gpu {
    class RenderTarget {
    public:
        RenderTarget();
        ~RenderTarget();

        RenderTarget(const RenderTarget&) = delete;
        RenderTarget& operator=(const RenderTarget&) = delete;

        void renderTest();
    private:
        gfx::ShaderHandle mShader;

        uint32_t mVAO;
        uint32_t mVBO;
        uint32_t mEBO;
    };
}