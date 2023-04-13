#pragma once

#include <glm/glm.hpp>
#include "math/size.h"

namespace gfx {
    class Camera {
    public:
        explicit Camera(const math::Size2D &size);

        void resize(const math::Size2D &size);

        [[nodiscard]] const glm::mat4& view() const { return mView; }
        [[nodiscard]] const glm::mat4& projection() const { return mProjection; }
    private:
        glm::mat4 mProjection { 1.0f };
        glm::mat4 mView { 1.0f };
    };
}