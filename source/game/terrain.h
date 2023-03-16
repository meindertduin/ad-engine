#pragma once

#include <cstdint>

#include "gfx/mesh.h"
#include "gfx/material.h"

namespace game {

    class Terrain {
    public:
        Terrain() = default;

        Terrain(const Terrain &other) = delete;
        Terrain& operator=(const Terrain &other) = delete;

        void initialize();

        [[nodiscard]] gfx::MaterialHandle material() const { return mMaterial; }
        std::unique_ptr<gfx::Mesh>& mesh() { return mTerrainMesh; }
    private:
        std::unique_ptr<gfx::Mesh> mTerrainMesh;
        gfx::MaterialHandle mMaterial;
    };
}