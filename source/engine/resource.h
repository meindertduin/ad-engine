#pragma once

#include <cstdint>
#include "platform/gcc.h"

template<typename T>
class Handle {
public:
    Handle() = default;
    explicit Handle(uint32_t id)
        : mId(id)
    {
    }

    [[nodiscard]] constexpr ALWAYS_INLINE uint32_t id() const { return mId; }

    T* operator->() const {
        return T::Manager::instance().get(mId);
    }
private:
    uint32_t mId;
};