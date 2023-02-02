#pragma once

#include "object.h"
#include "gfx/sprite.h"

#include <map>

namespace game {
    class RenderWorld {
    public:
        void addSprite(Object object, const gfx::Sprite &sprite);
        void removeSprite(Object object);
    private:
        std::map<uint32_t , gfx::Sprite> mSprites;
    };
}
