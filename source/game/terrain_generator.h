#pragma once

#include <string>
#include "gfx/mesh.h"
#include "tile_set.h"

namespace game {
    struct TerrainData {
        math::Size2D size;
        int tileSize;
        std::unordered_map<int, std::shared_ptr<TerrainTile>> tilesSet;
        std::vector<uint32_t> tiles;
    };

    class TerrainGenerator {
    public:
        std::unique_ptr<TerrainData> generateTerrain(const std::string &folder);
    };
}