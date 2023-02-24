#pragma once

#include <cstdint>
#include <compare>
#include <string>
#include <stdexcept>
#include "platform/gcc.h"

class Hash64 {
public:
    Hash64() = default;
    explicit Hash64(const std::string &value);

    bool operator==(const Hash64 &rhs) const = default;
    auto operator<=>(const Hash64 rhs) const { return mHash <=> rhs.mHash; }

    [[nodiscard]] constexpr ALWAYS_INLINE uint64_t value() const { return mHash; }
private:
    uint64_t mHash { 0 };
};


class Hash32 {
public:
    Hash32() = default;
    explicit Hash32(const uint32_t &value);

    bool operator==(const Hash32 &rhs) const = default;
    auto operator<=>(const Hash32 rhs) const { return mHash <=> rhs.mHash; }

    [[nodiscard]] constexpr ALWAYS_INLINE uint64_t value() const { return mHash; }
private:
    uint32_t mHash { 0 };
};

template<>
struct std::hash<Hash64> {
    auto operator()(const Hash64 hash64) const -> size_t {
        return hash<uint64_t>()(hash64.value());
    }
};