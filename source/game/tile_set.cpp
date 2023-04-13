#include "tile_set.h"

namespace game {
    bool TileSet::hasTile(int id) const {
        return mTiles.contains(id);
    }

    void TileSet::addTile(int id, TerrainTile &&tile) {
        mTiles.insert({ id, std::move(tile) });
    }
}