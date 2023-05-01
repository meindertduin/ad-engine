#pragma once

#include <string>

#include <math/size.h>
#include "observable.h"

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
    static std::unique_ptr<AdWindow> createInstance(const WindowOptions &options);
    virtual ~AdWindow() = default;

    virtual bool initialize() = 0;
    virtual void setupFrame() = 0;
    virtual void renderFrame() = 0;
    virtual void pollEvents() = 0;
    virtual uint32_t getTicks() const = 0;

    virtual math::Size2D size() const = 0;
    virtual bool closed() const = 0;
    virtual Observable<WindowEvent> &windowEvent() = 0;
};