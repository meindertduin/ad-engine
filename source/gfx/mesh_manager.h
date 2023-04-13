#pragma once

#include "mesh.h"
#include "engine/path.h"

namespace gfx {
    class MeshManager {
    public:
        static MeshManager& instance() {
            static MeshManager instance;
            return instance;
        }

        void initialize();

        MeshHandle plane();

        void cleanup() {
            mMeshPathIds.clear();
            mMeshes.clear();
        }

    private:
        friend class Handle<Mesh>;

        MeshManager() = default;

        Mesh* get(uint32_t id);

        std::unordered_map<Path, uint32_t> mMeshPathIds;
        std::unordered_map<uint32_t, std::unique_ptr<Mesh>> mMeshes;

        MeshHandle mPlane;
        uint32_t mNextId { 0 };
    };
}