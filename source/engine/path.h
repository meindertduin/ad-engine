#pragma once

#include <string>
#include "hash.h"

class Path {
public:
    Path() = default;

    explicit Path(const std::string &path)
        : mPath{path}
        , mHash{path}
    {}

    explicit Path(const std::string &&path)
        : mPath{path}
    {
        mHash = Hash64{mPath};
    }

    [[nodiscard]] constexpr ALWAYS_INLINE const std::string& path() const { return mPath; }
    [[nodiscard]] constexpr ALWAYS_INLINE Hash64 hash() const { return mHash; }

    inline auto operator==(const Path &rhs) const { return mHash == rhs.mHash; }
    inline auto operator!=(const Path &rhs) const { return mHash != rhs.mHash; }
private:
    std::string mPath {};
    Hash64 mHash {};
};