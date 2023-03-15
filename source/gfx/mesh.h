#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace gfx {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    class Mesh {
    public:
        explicit Mesh(const std::vector<Vertex> &vertices);
        void render();
    private:
        uint32_t mVao;
        uint32_t mVbo;
    };
}
