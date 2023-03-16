#pragma once

#include <string>
#include "gfx/mesh.h"

namespace game {

    class TerrainGenerator {
    public:
        std::unique_ptr<gfx::Mesh> generateTerrainMesh(const std::string &folder);
    };
}