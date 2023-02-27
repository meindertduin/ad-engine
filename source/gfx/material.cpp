#include "material.h"

namespace gfx {
    void Material::setShader(ShaderHandle shader) {
        mShader = shader;
    }

    void Material::addTexture(TextureHandle texture) {
        mTextures.push(texture);
    }
}