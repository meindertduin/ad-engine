#pragma once

#include <string>

#include <math/size.h>
#include "observable.h"

struct SDL_Window;

struct WindowOptions {
    std::string title;
    math::Size2D size;
};

struct WindowEvent {
    enum class Type {
        None,
        Quit,
        Resize
    };

    Type type;
    math::Size2D size;
};

class AdWindow {
public:
    explicit AdWindow(const WindowOptions &options);
    ~AdWindow();

    bool initialize();
    void pollEvents();


    [[nodiscard]] constexpr ALWAYS_INLINE math::Size2D size() const {
        return mSize;
    }

    [[nodiscard]] constexpr ALWAYS_INLINE bool closed() const {
        return mClosed;
    }

    [[nodiscard]] constexpr ALWAYS_INLINE Observable<WindowEvent> &windowEvent() {
        return mWindowEventObservable;
    }

private:
    SDL_Window* pWindow { nullptr };
    math::Size2D mSize;
    std::string mTitle;
    bool mClosed { false };

    Observable<WindowEvent> mWindowEventObservable;
};