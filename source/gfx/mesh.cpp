//
// Created by dikkie on 3/15/23.
//

#include "mesh.h"

namespace gfx {
    Mesh::Mesh(const std::vector<Vertex> &vertices) {
        mVertexBuffer = gpu::VertexBuffer::create(vertices.data(), vertices.size() * sizeof(Vertex), Vertex::layout);
    }

    Mesh::Mesh(Vertex *vertices, size_t size) {
        mVertexBuffer = gpu::VertexBuffer::create(vertices, size, Vertex::layout);
    }

    void Mesh::draw() const {
        mVertexBuffer->bind();
        mVertexBuffer->draw();
    }

}