#include "shader_manager.h"

namespace gfx {
    Shader* ShaderManager::createShader(const Path &path) {
        auto shader = std::make_unique<Shader>();

        mShaders.insert({ path, std::move(shader) });
        mShaders[path] = std::move(shader);

        return mShaders[path].get();
    }

    Shader *ShaderManager::getShader(const Path &path) {
        return mShaders[path].get();
    }
}