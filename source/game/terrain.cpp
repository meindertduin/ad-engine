#include "terrain.h"
#include "terrain_generator.h"

namespace game {
    void Terrain::initialize() {
        TerrainGenerator generator;
        mTerrainMesh = generator.generateTerrainMesh("assets/terrain/forest");
    }

    void Terrain::render() {
        mTerrainMesh->draw();
    }
}