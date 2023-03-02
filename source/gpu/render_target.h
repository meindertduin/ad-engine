#pragma once

#include <cstdint>

namespace gpu {
    class RenderTarget {
    public:
        RenderTarget();

        RenderTarget(const RenderTarget&) = delete;
        RenderTarget& operator=(const RenderTarget&) = delete;

        void renderTest();
    };
}