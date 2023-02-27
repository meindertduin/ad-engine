#pragma once

#include <unordered_map>
#include "texture.h"
#include "engine/path.h"

namespace gfx {
    class TextureManager {
    public:
        static TextureManager& instance() {
            static TextureManager instance;
            return instance;
        }

        TextureHandle createTexture(const Path &path);

        void cleanup() {
            mTexturePathsIdsMap.clear();
            mTextures.clear();
        }
    private:
        friend class Handle<Texture2D>;

        TextureManager() = default;

        Texture2D* get(uint32_t id);

        std::unordered_map<Path, uint32_t> mTexturePathsIdsMap;
        std::unordered_map<uint32_t, std::unique_ptr<Texture2D>> mTextures;

        uint32_t mNextId { 0 };
    };
}
