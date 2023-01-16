#pragma once

#include <cstdint>
#include <compare>
#include <string>
#include "platform/gcc.h"

class Hash64 {
public:
    Hash64() = default;
    explicit Hash64(const std::string &value);

    auto operator==(const Hash64 &rhs) const { return mHash == rhs.mHash; }
    auto operator!=(const Hash64 rhs) const { return mHash != rhs.mHash; }
    auto operator<=>(const Hash64 rhs) const { return mHash <=> rhs.mHash; }

    [[nodiscard]] constexpr ALWAYS_INLINE uint64_t value() const { return mHash; }
private:
    uint64_t mHash {0 };
};

template<>
struct std::hash<Hash64> {
    auto operator()(const Hash64 hash64) const -> size_t {
        return hash<uint64_t>()(hash64.value());
    }
};