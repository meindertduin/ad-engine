#pragma once

#include "game/component.h"
#include "engine/path.h"
#include "material.h"
#include "texture.h"

namespace gfx {
    class RenderComponent : public game::Component<RenderComponent> {
    public:
        explicit RenderComponent(const Path &materialPath, const Path &texturePath) noexcept;

        MaterialHandle material() { return mMaterial; }
        std::shared_ptr<Texture2D> texture() { return mTexture; }
    private:
        MaterialHandle mMaterial;
        std::shared_ptr<Texture2D> mTexture;
    };
};