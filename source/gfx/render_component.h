#pragma once

#include "game/component.h"
#include "engine/path.h"
#include "material.h"
#include "texture.h"
#include "mesh.h"

namespace gfx {
    class RenderComponent : public game::Component<RenderComponent> {
    public:
        explicit RenderComponent(const Path &materialPath) noexcept;

        MaterialHandle material() { return mMaterial; }
        MeshHandle mesh() { return mMesh; }
    private:
        MaterialHandle mMaterial;
        MeshHandle mMesh;
    };
};