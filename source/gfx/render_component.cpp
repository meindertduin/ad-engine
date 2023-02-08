#include "render_component.h"
#include "game/component.h"
#include "material_manager.h"

namespace gfx {
    REGISTER_COMPONENT(RenderComponent);

    RenderComponent::RenderComponent(const Path &materialPath, const Path &texturePath) noexcept
        : mMaterial(MaterialManager::instance().createMaterial(materialPath))
        , mTexture(Texture2D::loadFromFile("assets/bricks.png"))
    {
    }
}


