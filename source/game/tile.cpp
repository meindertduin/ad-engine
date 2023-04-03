#include "tile.h"

#include "gfx/material_manager.h"

namespace game {
    void TerrainTile::setMaterial(const Path &path) {
        mMaterial = gfx::MaterialManager::instance().createMaterial(path);
    }
}