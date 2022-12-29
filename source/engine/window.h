#pragma once

#include <string>
#include <bx/math.h>

#include <math/size.h>

struct SDL_Window;

class AdWindow {
public:
    explicit AdWindow(const math::Size2 &size, std::string title);
    ~AdWindow();

    bool initialize();

    [[nodiscard]] constexpr ALWAYS_INLINE math::Size2 size() const {
        return mSize;
    }
private:
    SDL_Window* pWindow { nullptr };
    math::Size2 mSize;
    std::string mTitle;
};