#include "render_component.h"
#include "material_manager.h"
#include "mesh_manager.h"

namespace gfx {
    REGISTER_COMPONENT(RenderComponent);

    RenderComponent::RenderComponent(const Path &materialPath) noexcept
        : mMaterial(MaterialManager::instance().createMaterial(materialPath))
    {
        // Testing with a plane
        mMesh = MeshManager::instance().plane();
    }
}