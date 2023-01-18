#pragma once

#include <string>
#include "hash.h"

class Path {
public:
    Path() = default;

    explicit Path(const std::string &path)
        : mValue{path}
        , mHash{path}
    {}

    explicit Path(const std::string &&path)
        : mValue{path}
    {
        mHash = Hash64{mValue};
    }

    [[nodiscard]] constexpr ALWAYS_INLINE const std::string& value() const { return mValue; }
    [[nodiscard]] constexpr ALWAYS_INLINE Hash64 hash() const { return mHash; }

    inline auto operator==(const Path &rhs) const { return mHash == rhs.mHash; }
    inline auto operator!=(const Path &rhs) const { return mHash != rhs.mHash; }

private:
    std::string mValue {};
    Hash64 mHash {};
};

template<>
struct std::hash<Path> {
    auto operator()(const Path &path) const -> size_t {
        return hash<uint64_t>()(path.hash().value());
    }
};
