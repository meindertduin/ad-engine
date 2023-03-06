#include "texture.h"
#include "math/size.h"

#include "gpu/gpu.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace gfx {
    class TextureImpl : public Texture2D {
    public:
        ~TextureImpl() override {
            gpu::destroyTexture(mTextureId);
        }

        explicit TextureImpl(const std::string &path) {
            int width, height, channels;

            stbi_set_flip_vertically_on_load(true);
            auto data = stbi_load_16(path.c_str(), &width, &height, &channels, 0);

            if (!data) {
                throw std::runtime_error("Failed to load texture");
            }

            mTextureId = gpu::createTexture2D(data, { width, height }, 0, [](auto imageData) {
                stbi_image_free(imageData);
            });
        }

        void render(uint32_t uniformHandle) override {
            gpu::bindTexture(mTextureId, uniformHandle);
        }

    private:
        uint32_t mTextureId;
    };

    std::unique_ptr<Texture2D> Texture2D::loadFromFile(const std::string &path) {
        return std::make_unique<TextureImpl>(path);
    }
}
