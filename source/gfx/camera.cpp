#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"

namespace gfx {
    Camera::Camera(const math::Size2D &size) {
        mProjection = glm::perspective(glm::radians(45.0f), float(size.width()) / float(size.height()), 0.1f, 1000.0f);

        auto positionMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 10.0f, 10.0f) * -1.0f);
        auto rotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        mView = rotation * positionMatrix;
    }

    void Camera::resize(const math::Size2D &size) {
        mProjection = glm::perspective(glm::radians(45.0f), float(size.width()) / float(size.height()), 0.1f, 1000.0f);
    }
}