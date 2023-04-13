#include "terrain.h"

#include "gfx/material_manager.h"

namespace game {
    void Terrain::initialize() {
        TerrainGenerator generator;
        mTerrainData = generator.generateTerrain("assets/terrain/forest");
    }
}