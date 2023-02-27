#pragma once

#include "shader.h"
#include "engine/path.h"
#include "engine/resource.h"
#include <memory>
#include <unordered_map>

namespace gfx {
    class ShaderManager {
    public:
        static ShaderManager& instance() {
            static ShaderManager sInstance;
            return sInstance;
        }

        ShaderHandle createShader(const Path &path);

        void cleanup() {
            mShaderPathsIdsMap.clear();
            mShaders.clear();
        }
    private:
        friend class Handle<Shader>;

        ShaderManager() = default;

        Shader* get(uint32_t id);

        std::unordered_map<Path, uint32_t> mShaderPathsIdsMap;
        std::unordered_map<uint32_t, std::unique_ptr<Shader>> mShaders;

        uint32_t mNextId { 0 };
    };
}
