#pragma once

#include <memory>
#include "shader.h"
#include "engine/resource.h"

namespace gfx {
    class Material {
    public:
        explicit Material(Allocator &allocator)
            : mAllocator(allocator)
            , mUniforms(allocator)
        {
        }

        void setShader(Handle<Shader> shader);
    private:
        Allocator &mAllocator;
        Vector<Uniform> mUniforms;
        Handle<Shader> mShader;
    };
}
