#pragma once

#include <unordered_map>
#include <memory>
#include "engine/path.h"

namespace gfx {
    class Material;

    class MaterialManager {
    public:
        static MaterialManager& instance() {
            static MaterialManager instance;
            return instance;
        }

        Material* createMaterial(const Path &path);
        Material* getMaterial(const Path &path);
    private:
        MaterialManager() = default;
        std::unordered_map<Path, std::unique_ptr<Material>> mMaterials;
    };
}