#pragma once

#include <memory>
#include "shader.h"
#include "engine/resource.h"
#include "texture.h"

namespace gfx {
    class Material;
    class MaterialManager;
    using MaterialHandle = Handle<Material>;

    class Material : public Resource<MaterialManager> {
    public:
        explicit Material(Allocator &allocator)
            : mTextures(allocator)
            , mUniforms(allocator)
        {
        }

        void setShader(ShaderHandle shader);
        void addTexture(TextureHandle texture);

        [[nodiscard]] constexpr ALWAYS_INLINE ShaderHandle shader() const { return mShader; }
        [[nodiscard]] constexpr ALWAYS_INLINE const Vector<TextureHandle>& textures() const { return mTextures; }
    private:
        Vector<TextureHandle> mTextures;
        Vector<Uniform> mUniforms;
        ShaderHandle mShader;
    };
}
