#pragma once

#include <string>

#include <math/size.h>

struct SDL_Window;

class AdWindow {
public:
    explicit AdWindow(const math::Size2D &size, std::string title);
    ~AdWindow();

    bool initialize();

    [[nodiscard]] constexpr ALWAYS_INLINE math::Size2D size() const {
        return mSize;
    }
private:
    SDL_Window* pWindow { nullptr };
    math::Size2D mSize;
    std::string mTitle;
};