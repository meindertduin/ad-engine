#pragma once

#include <string>
#include <memory>

#include <bgfx/bgfx.h>

namespace gfx {
    class Texture2D {
    public:
        virtual ~Texture2D() = default;
        static std::unique_ptr<Texture2D> loadFromFile(const std::string& path);
        virtual void render(bgfx::UniformHandle uniformHandle) = 0;
    };
}