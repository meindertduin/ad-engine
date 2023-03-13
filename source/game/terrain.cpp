#include "terrain.h"
#include <fastwfc/overlapping_wfc.hpp>

#include "stb_image.h"
#include "engine/logging.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi_image_write.h>
#include <stdexcept>
#include <optional>

struct Color {
    unsigned char r, g, b;

    bool operator==(const Color& other) const noexcept = default;
};

namespace std {
    template <> class hash<Color> {
    public:
        size_t operator()(const Color &c) const {
            return (size_t)c.r + (size_t)256 * (size_t)c.g +
                   (size_t)256 * (size_t)256 * (size_t)c.b;
        }
    };
}

namespace game {
    std::optional<Array2D<Color>> readImage(const std::string &filePath) {
        int width;
        int height;
        int numComponents ;
        unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &numComponents, 3);
        if(data == nullptr) {
            return std::nullopt;
        }
        auto m = Array2D<Color>(height, width);
        for(unsigned i = 0; i < (unsigned)height; i++) {
            for(unsigned j = 0; j < (unsigned)width; j++) {
                unsigned index = 3 * (i * width + j);
                m.data[i * width + j] = {data[index], data[index + 1], data[index + 2]};
            }
        }
        stbi_image_free(data);
        return m;
    }

    void writeImagePng(const std::string& filePath, const Array2D<Color>& m) noexcept {
        stbi_write_png(filePath.c_str(), static_cast<int>(m.width), static_cast<int>(m.height), 3, (const unsigned char*)m.data.data(), 0);
    }

    void Terrain::initialize() {
        stbi_set_flip_vertically_on_load(false);
        auto image = readImage("assets/lake.png");
        if (!image.has_value()) {
            throw std::runtime_error("Failed to load image");
        }

        OverlappingWFCOptions options = { true, true, 60, 60, 8, false, 3 };

        for (int test = 0; test < 10; test++) {
            auto seed = static_cast<int>(std::random_device()());
            OverlappingWFC wfc(*image, options, seed);

            if (auto success = wfc.run(); success.has_value()) {
                writeImagePng("assets/output.png", *success);
                Logger::info("Success");
                break;
            }

            Logger::info("Failure");
        }
    }
}