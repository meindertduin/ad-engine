#include "material.h"

namespace gfx {
    void Material::setShader(ShaderHandle shader) {
        mShader = shader;
    }
}