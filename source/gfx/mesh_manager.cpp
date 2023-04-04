#include "mesh_manager.h"

namespace gfx {
    static gfx::Vertex planeVertices[] = {
            { glm::vec3(1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) }, // 0
            { glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) }, // 1
            { glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) }, // 3
            { glm::vec3(1.0f, -1.0f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) }, // 1
            { glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) }, // 2
            { glm::vec3(-1.0f,  1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) }, // 3
    };

    Mesh *MeshManager::get(uint32_t id) {
        return mMeshes[id].get();
    }

    void MeshManager::initialize() {
        auto mesh = std::make_unique<Mesh>(planeVertices, sizeof(planeVertices));

        mMeshes.insert({ mNextId, std::move(mesh) });
        mPlane = MeshHandle(mNextId++);
    }

    MeshHandle MeshManager::plane() {
        return mPlane;
    }
}
