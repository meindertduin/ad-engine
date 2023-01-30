#pragma once

#include "shader.h"
#include "engine/path.h"
#include <memory>
#include <unordered_map>

namespace gfx {
    class ShaderManager {
    public:
        static ShaderManager& instance() {
            static ShaderManager sInstance;
            return sInstance;
        }

        Shader* createShader(const Path &path);
        Shader* getShader(const Path &path);

        void cleanup() {
            mShaders.clear();
        }
    private:
        ShaderManager() = default;

        std::unordered_map<Path, std::unique_ptr<Shader>> mShaders;
    };
}
