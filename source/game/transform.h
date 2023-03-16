#include <glm/vec3.hpp>
#include "component.h"

namespace game {
    class Transform : public Component<Transform> {
    public:
        Transform() noexcept = default;

        Transform(float x, float y) noexcept : mPosition(x, y, 0.0f) {}
        Transform(float x, float y, float z) noexcept : mPosition(x, y, z) {}

        explicit Transform(const glm::vec3 &position) noexcept : mPosition(position) {}

        [[nodiscard]] const glm::vec3& position() const { return mPosition; }
    private:
        glm::vec3 mPosition { 0.0f };
    };
}