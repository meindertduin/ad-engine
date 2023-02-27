#pragma once

#include <unordered_map>
#include <memory>
#include "engine/path.h"
#include "engine/resource.h"
#include "material.h"

namespace gfx {
    class MaterialManager {
    public:
        static MaterialManager& instance() {
            static MaterialManager instance;
            return instance;
        }

        MaterialHandle createMaterial(const Path &path);

        void cleanup() {
            mMaterialPathsIdsMap.clear();
            mMaterials.clear();
        }

    private:
        friend class Handle<Material>;

        MaterialManager() = default;

        Material* get(uint32_t id);

        std::unordered_map<Path, uint32_t> mMaterialPathsIdsMap;
        std::unordered_map<uint32_t, std::unique_ptr<Material>> mMaterials;

        uint32_t mNextId { 0 };
    };
}