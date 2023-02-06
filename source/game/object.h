#pragma once

#include <cstdint>
#include <bitset>

#include "platform/gcc.h"
#include "constants.h"

namespace game {
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

        auto operator <=>(const Object &other) const {
            return mId <=> other.mId;
        }
    private:
        uint32_t  mId;
    };
}

template<>
struct std::hash<game::Object> {
    auto operator()(const game::Object object) const -> size_t {
        return hash<uint32_t>()(object.id());
    }
};
