#pragma once

#include <cstdint>

#include "gfx/mesh.h"

namespace game {
    struct TerrainTile {
        uint32_t tileId;
    };

    class Terrain {
    public:
        Terrain() = default;

        Terrain(const Terrain &other) = delete;
        Terrain& operator=(const Terrain &other) = delete;

        void initialize();

    private:
        std::unique_ptr<gfx::Mesh> mTerrainMesh;
    };
}