#pragma once

#include <cstdint>

#include "gfx/mesh.h"
#include "gfx/material.h"
#include "terrain_generator.h"

namespace game {

    class Terrain {
    public:
        Terrain() = default;

        Terrain(const Terrain &other) = delete;
        Terrain& operator=(const Terrain &other) = delete;

        void initialize();

        std::unique_ptr<TerrainData> &terrainData() { return mTerrainData; }
    private:
        std::unique_ptr<TerrainData> mTerrainData;
    };
}