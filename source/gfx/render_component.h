#pragma once

#include "game/component.h"
#include "engine/path.h"
#include "material.h"

namespace gfx {
    class RenderComponent : public game::Component<RenderComponent> {
    public:
        explicit RenderComponent(const Path &materialPath) noexcept;
    private:
        Material* mMaterial { nullptr };
    };
};