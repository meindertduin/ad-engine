#pragma once

#include <memory>
#include "shader.h"
#include "engine/resource.h"

namespace gfx {
    class Material;
    class MaterialManager;
    using MaterialHandle = Handle<Material>;

    class Material : public Resource<MaterialManager> {
    public:
        explicit Material(Allocator &allocator)
            : mAllocator(allocator)
            , mUniforms(allocator)
        {
        }

        void setShader(ShaderHandle shader);
    private:
        Allocator &mAllocator;
        Vector<Uniform> mUniforms;
        ShaderHandle mShader;
    };
}
