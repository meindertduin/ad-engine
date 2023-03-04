#include "texture.h"
#include "math/size.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <GL/glew.h>

namespace gfx {
    class TextureImpl : public Texture2D {
    public:
        ~TextureImpl() override {
            glDeleteTextures(1, &mTextureId);
        }

        explicit TextureImpl(const math::Size2D &size, unsigned char const *data) {
            glGenTextures(1, &mTextureId);
            glBindTexture(GL_TEXTURE_2D, mTextureId);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.width(), size.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }

        void render(uint32_t uniformHandle) override {
            glActiveTexture(GL_TEXTURE0+ uniformHandle);
            glBindTexture(GL_TEXTURE_2D, mTextureId);
        }

    private:
        uint32_t mTextureId;
    };

    std::unique_ptr<Texture2D> Texture2D::loadFromFile(const std::string &path) {
        int width, height, channels;
        auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);

        if (!data) {
            return nullptr;
        }

        auto texture = std::make_unique<TextureImpl>(math::Size2D{ width, height }, data);
        stbi_image_free(data);

        return texture;
    }
}
