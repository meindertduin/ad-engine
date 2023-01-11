#pragma once

#include <string>
#include <memory>

#include <bgfx/bgfx.h>

namespace gfx {
    struct UniformHandle {
        uint16_t idx;
    };

    class Texture {
    public:
        virtual ~Texture() = default;
        static std::unique_ptr<Texture> loadFromFile(const std::string& path);
        virtual void render(bgfx::UniformHandle uniformHandle) = 0;
    };
}