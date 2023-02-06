#pragma once

#include "object.h"
#include "gfx/sprite.h"

#include <unordered_map>

namespace game {
    class RenderWorld {
    public:
        void addSprite(Object object, const gfx::Sprite &sprite);
        void removeSprite(Object object);
    private:
        std::unordered_map<Object, gfx::Sprite> mSprites;
    };
}
