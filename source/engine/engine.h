#pragma once

#include "allocator.h"
#include "platform/gcc.h"


class Engine {
public:
    static Engine& instance() {
        static Engine engine;
        return engine;
    }

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    constexpr ALWAYS_INLINE ListAllocator& allocator() {
        return mAllocator;
    }

private:
    Engine() = default;

    ListAllocator mAllocator { 1024 * 1024 };
};