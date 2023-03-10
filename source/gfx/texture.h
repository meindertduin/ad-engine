#pragma once

#include <string>
#include <memory>

#include "engine/resource.h"

namespace gfx {
    class Texture2D;
    class TextureManager;

    using TextureHandle = Handle<Texture2D>;

    class Texture2D : public Resource<TextureManager> {
    public:
        virtual ~Texture2D() = default;

        static std::unique_ptr<Texture2D> loadFromFile(const std::string& path);
        virtual void render(uint32_t uniformHandle) = 0;
    };
}