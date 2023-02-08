#include "material.h"

namespace gfx {
    void Material::setShader(Handle<Shader> shader) {
        mShader = shader;
    }
}