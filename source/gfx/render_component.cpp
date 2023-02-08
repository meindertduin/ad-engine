#include "render_component.h"
#include "game/component.h"
#include "material_manager.h"

namespace gfx {
    REGISTER_COMPONENT(RenderComponent);

    RenderComponent::RenderComponent(const Path &materialPath) noexcept
        : mMaterial(MaterialManager::instance().getMaterial(materialPath))
    {
    }
}


