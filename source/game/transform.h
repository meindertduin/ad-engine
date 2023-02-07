#include "component.h"

namespace game {
    class Transform : public Component<Transform> {
    public:
        Transform() noexcept = default;

        Transform(float x, float y) noexcept
            : mX(x)
            , mY(y)
        {}

        [[nodiscard]] ALWAYS_INLINE constexpr float x() const {
            return mX;
        }

        [[nodiscard]] ALWAYS_INLINE constexpr float y() const {
            return mY;
        }

        void move(float x, float y) {
            mX += x;
            mY += y;
        }

        void setPosition(float x, float y) {
            mX = x;
            mY = y;
        }

    private:
        float mX = 0.0f;
        float mY = 0.0f;
    };

}