#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "gpu/gpu.h"
#include "engine/resource.h"

namespace gfx {
    class Mesh;
    class MeshManager;
    using MeshHandle = Handle<Mesh>;

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;

        static void init() {
            if (initialized) {
                return;
            }

            layout
                .addAttribute(gpu::Attribute::Position, gpu::AttributeType::Vec3, false)
                .addAttribute(gpu::Attribute::Normal, gpu::AttributeType::Vec3, false)
                .addAttribute(gpu::Attribute::TexCoord, gpu::AttributeType::Vec2, false);

            initialized = true;
        }

        static inline gpu::VertexLayout layout;
        static inline bool initialized = false;
    };

    class Mesh : public Resource<MeshManager> {
    public:
        explicit Mesh(const std::vector<Vertex> &vertices);
        Mesh(Vertex *vertices, size_t size);
        void draw() const;

    private:
        std::unique_ptr<gpu::VertexBuffer> mVertexBuffer;
    };

    namespace detail{
        struct VertexInitializer {
            VertexInitializer() {
                Vertex::init();
            }
        };

        [[maybe_unused]] static const inline VertexInitializer vertexInitializer;
    }
}
