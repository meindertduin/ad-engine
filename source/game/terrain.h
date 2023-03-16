#pragma once

#include <cstdint>

#include "gfx/mesh.h"

namespace game {

    class Terrain {
    public:
        Terrain() = default;

        Terrain(const Terrain &other) = delete;
        Terrain& operator=(const Terrain &other) = delete;

        void initialize();

        void render();
    private:
        std::unique_ptr<gfx::Mesh> mTerrainMesh;
    };
}