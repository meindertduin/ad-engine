#include "texture.h"
#include "math/size.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


namespace gfx {
    class TextureImpl : public Texture {
    public:
        ~TextureImpl() override {
            bgfx::destroy(mTextureHandle);
        }

        explicit TextureImpl(const math::Size2D &size, unsigned char *data)
        {
            mTextureHandle = bgfx::createTexture2D(size.width(), size.height(), false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_NONE, bgfx::makeRef(data, size.width() * size.height() * 4));
        }

        void render(bgfx::UniformHandle uniformHandle) override {
            bgfx::setTexture(0, uniformHandle, mTextureHandle);
        }

    private:
        bgfx::TextureHandle mTextureHandle = BGFX_INVALID_HANDLE;
    };

    std::unique_ptr<Texture> Texture::loadFromFile(const std::string &path) {
        int width, height, channels;
        auto data = stbi_load(path.c_str(), &width, &height, &channels, 4);

        if (!data) {
            return nullptr;
        }

        return std::make_unique<TextureImpl>(math::Size2D{ width, height }, data);
    }
}
