#include "render_world.h"

namespace game {
    void RenderWorld::addSprite(Object object, const gfx::Sprite &sprite) {
        mSprites.insert(std::make_pair(object, sprite));
    }

    void RenderWorld::removeSprite(Object object) {
        mSprites.erase(object);
    }
}
