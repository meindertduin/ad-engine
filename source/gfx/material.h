#pragma once

#include <memory>
#include "shader.h"

namespace gfx {
    class Material {
    public:
        explicit Material(Allocator &allocator)
            : mAllocator(allocator)
            , mUniforms(allocator)
        {
        }

        void setShader(Shader *shader);

    private:
        Allocator &mAllocator;
        Vector<Uniform> mUniforms;
        Shader* mShader { nullptr };
    };
}
