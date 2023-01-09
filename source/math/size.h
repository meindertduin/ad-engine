#pragma once

#include <utility>
#include "platform/gcc.h"
#include <array>
#include <cstdint>

namespace math {
    template<typename T, std::size_t Dimensions>
    class Size {
    public:
        constexpr ALWAYS_INLINE T width() const {
            static_assert(Dimensions >= 2, "Size must have at least 2 dimensions");
            return v[0];
        }

        constexpr ALWAYS_INLINE T height() const {
            static_assert(Dimensions >= 2, "Size must have at least 2 dimensions");
            return v[1];
        }

        void setWidth(T width) {
            static_assert(Dimensions >= 2, "Size must have at least 2 dimensions");
            v[0] = width;
        }

        void setHeight(T height) {
            static_assert(Dimensions >= 2, "Size must have at least 2 dimensions");
            v[1] = height;
        }

        constexpr auto operator==(const Size& other) const noexcept {
            return std::equal(v.begin(), v.end(), other.v.begin());
        }

        constexpr auto operator!=(const Size& other) const noexcept {
            return !std::equal(v.begin(), v.end(), other.v.begin());
        }

        std::array<T, Dimensions> v;
    };

    template<typename T, typename ... U>
    Size(T, U...) -> Size<T, 1 + sizeof...(U)>;

    using Size2D = Size<int, 2>;
}