#pragma once

#include <unordered_map>
#include <memory>
#include "engine/path.h"
#include "engine/resource.h"
#include "material.h"

namespace gfx {
    class Material;

    class MaterialManager {
    public:
        static MaterialManager& instance() {
            static MaterialManager instance;
            return instance;
        }

        MaterialHandle createMaterial(const Path &path);
        Material* get(int id);

    private:
        MaterialManager() = default;

        std::unordered_map<Path, uint32_t> mMaterialPathsIdsMap;
        std::unordered_map<uint32_t, std::unique_ptr<Material>> mMaterials;

        uint32_t mNextId { 0 };
    };
}