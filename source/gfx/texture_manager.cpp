#include "texture_manager.h"

namespace gfx {
    TextureHandle TextureManager::createTexture(const Path &path) {
        if (mTexturePathsIdsMap.contains(path)) {
            return TextureHandle { mTexturePathsIdsMap[path] };
        }

        auto texture = Texture2D::loadFromFile(path.value());
        mTexturePathsIdsMap[path] = mNextId;
        mTextures[mNextId] = std::move(texture);

        return TextureHandle { mNextId++ };
    }

    Texture2D *TextureManager::get(uint32_t id) {
        return mTextures[id].get();
    }
}