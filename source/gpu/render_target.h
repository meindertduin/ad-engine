#pragma once

#include <cstdint>

namespace gpu {
    class RenderTarget {
    public:
        RenderTarget();
        ~RenderTarget();

        RenderTarget(const RenderTarget&) = delete;
        RenderTarget& operator=(const RenderTarget&) = delete;

        void renderTest();
    private:
        uint32_t mVertexShader;
        uint32_t mFragmentShader;
        uint32_t mShaderProgram;

        uint32_t mVAO;
        uint32_t mVBO;
        uint32_t mEBO;
    };
}