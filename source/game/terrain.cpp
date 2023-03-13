#include "terrain.h"
#include <fastwfc/direction.hpp>

#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi_image_write.h>

namespace game {
    void Terrain::initialize() {
        [[maybe_unused]] auto x = get_opposite_direction(0);
    }
}