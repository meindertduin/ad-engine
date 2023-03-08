#pragma once

#include <cstdint>

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
        constexpr static uint32_t kTerrainWidth = 8;
        TerrainTile mTerrainFile[kTerrainWidth][kTerrainWidth];
    };
}