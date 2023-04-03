#pragma once

#include "engine/path.h"
#include "tile.h"
#include <unordered_map>

namespace game {
    class TileSet {
    public:
        explicit TileSet() = default;

        TileSet(const TileSet &other) = delete;
        TileSet& operator=(const TileSet &other) = delete;

        TerrainTile& getTile(int id) { return mTiles[id]; }
        bool hasTile(int id) const;
        void addTile(int id, TerrainTile &&tile);
    private:
        std::unordered_map<int, TerrainTile> mTiles;
    };
}