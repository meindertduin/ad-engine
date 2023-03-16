#include "terrain.h"
#include "terrain_generator.h"

#include "gfx/material_manager.h"

namespace game {
    void Terrain::initialize() {
        TerrainGenerator generator;
        mTerrainMesh = generator.generateTerrainMesh("assets/terrain/forest");
        mMaterial = gfx::MaterialManager::instance().createMaterial(Path {"assets/material_scripts/material.lua"});
    }
}