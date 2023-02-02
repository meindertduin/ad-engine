#pragma once

#include <cstdint>
#include <bitset>

#include "platform/gcc.h"

namespace game {

    constexpr uint32_t MaxComponents = 32;
    using Signature = std::bitset<MaxComponents>;

    class Object {
    public:
        explicit Object(uint32_t id)
            : mId(id)
        {}

        [[nodiscard]] constexpr ALWAYS_INLINE uint32_t id() const {
            return mId;
        }

        bool operator==(const Object &other) const {
            return mId == other.mId;
        }

        bool operator!=(const Object &other) const {
            return mId != other.mId;
        }

    private:
        uint32_t  mId;
    };
}
