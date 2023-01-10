#pragma once

#include <string>

#include <math/size.h>

struct SDL_Window;

class AdWindow {
public:
    explicit AdWindow(const math::Size2D &size, std::string title);
    ~AdWindow();

    bool initialize();
    void pollEvents();


    [[nodiscard]] constexpr ALWAYS_INLINE math::Size2D size() const {
        return mSize;
    }

    [[nodiscard]] constexpr ALWAYS_INLINE bool closed() const {
        return mClosed;
    }
private:
    SDL_Window* pWindow { nullptr };
    math::Size2D mSize;
    std::string mTitle;
    bool mClosed { false };
};