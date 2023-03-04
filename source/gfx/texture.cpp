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

        explicit TextureImpl(const math::Size2D &size, unsigned short const *data) {
            glBindTexture(GL_TEXTURE_2D, mTextureId);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.width(), size.height(), 0, GL_RGBA, GL_UNSIGNED_SHORT, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            // set the texture wrapping/filtering options (on the currently bound textures object)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                            GL_REPEAT);   // set textures wrapping to GL_REPEAT (default wrapping method)
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // set textures filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
        auto data = stbi_load_16(path.c_str(), &width, &height, &channels, 0);

        if (!data) {
            return nullptr;
        }

        auto texture = std::make_unique<TextureImpl>(math::Size2D{ width, height },  data);
        stbi_image_free(data);

        return texture;
    }
}
