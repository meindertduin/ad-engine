#pragma once

#include "gfx/material.h"
#include "gfx/mesh.h"
#include "transform.h"

namespace game {
    class TerrainTile {
    public:
        [[nodiscard]] gfx::MaterialHandle material() const { return mMaterial; }
        std::unique_ptr<gfx::Mesh>& mesh() { return mTileMesh; }

        void setMesh(std::unique_ptr<gfx::Mesh> &&mesh) { mTileMesh = std::move(mesh); }
        void setMaterial(const Path &path);

    private:
        gfx::MaterialHandle mMaterial;
        std::unique_ptr<gfx::Mesh> mTileMesh;
    };
}