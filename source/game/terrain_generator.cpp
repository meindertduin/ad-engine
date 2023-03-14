#include "terrain_generator.h"
#include "engine/file_reader.h"
#include <fastwfc/tiling_wfc.hpp>
#include <json/json.h>
#include <string>
#include <unordered_map>

#include "stb_image.h"
#include "engine/logging.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi_image_write.h>

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
    struct TiledData {
        struct Tile {
            struct Neighbor {
                uint32_t leftOrientation;
                uint32_t rightOrientation;
                std::string right;
            };

            std::string name;
            Symmetry symmetry;
            double weight;
            std::vector<Neighbor> neighbors;
        };

        std::string name;
        std::unordered_map<std::string, Tile> tiles;
    };

    TiledData readJsonData(const std::string &directory) {
        TiledData tiledData;
        FileReader fileReader(directory + "/data.json");
        Json::Reader reader;
        Json::Value obj;

        reader.parse(fileReader.getFileContent(), obj);

        tiledData.name = obj["name"].asString();
        const auto& tiles = obj["tiles"];

        auto getSymmetry = [](const Json::Value &value) {
            std::unordered_map<std::string, Symmetry> symmetries = {
                    { "X", Symmetry::X },
                    { "T", Symmetry::T },
                    { "I", Symmetry::I },
                    { "L", Symmetry::L },
                    { "\\", Symmetry::backslash },
                    { "p", Symmetry::P },
            };

            return symmetries[value["symmetry"].asString()];
        };

        for (const auto& tile : tiles) {
            TiledData::Tile resultTile;
            resultTile.name = tile["name"].asString();
            resultTile.symmetry = getSymmetry(tile);
            resultTile.weight = tile["weight"].isNull() ? 1.0f : tile["weight"].asDouble();

            auto const& neighbors = tile["neighbors"];
            for (const auto& neighbor : neighbors) {
                TiledData::Tile::Neighbor resultNeighbor;

                resultNeighbor.leftOrientation = neighbor["orientation"].isNull() ? 0 : neighbor["orientation"].asUInt();

                resultNeighbor.right = neighbor["right"].asString();
                resultNeighbor.rightOrientation = neighbor["rightOrientation"].isNull() ? 0 : neighbor["rightOrientation"].asUInt();

                resultTile.neighbors.push_back(resultNeighbor);
            }

            tiledData.tiles[resultTile.name] = resultTile;
        }

        return tiledData;
    }

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

    void generate() {
        std::unordered_map<std::string, Tile<Color>> colorTiles;
        std::unordered_map<std::string, uint32_t> tileIds;
        std::vector<Tile<Color>> tileList;

        std::vector<std::tuple<std::string, uint32_t, std::string, uint32_t>> neighbors;
        std::vector<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>> neighborIds;

        auto tiledData = readJsonData("assets/terrain/rooms");

        for (const auto &[tileName, tile] : tiledData.tiles) {
            auto imagePath = "assets/terrain/rooms/" + tileName + ".png";
            auto image = readImage(imagePath);

            if (!image.has_value()) {
                throw std::runtime_error("Failed to load image");
            }

            colorTiles.try_emplace(tileName, *image, tile.symmetry, tile.weight);

            for (const auto &neighbor : tile.neighbors) {
                neighbors.emplace_back(tileName, neighbor.leftOrientation, neighbor.right, neighbor.rightOrientation);
            }
        }

        uint32_t id = 0;
        for (auto const &[tileName, tile] : colorTiles) {
            tileIds.try_emplace(tileName, id);
            tileList.push_back(tile);
            id++;
        }

        for (const auto& [leftName, orientation, rightName, rightOrientation] : neighbors) {
            neighborIds.emplace_back(tileIds[leftName], orientation, tileIds[rightName], rightOrientation);
        }

        for (auto test = 0; test < 10; test++) {
            int seed = std::random_device()();

            TilingWFC<Color> wfc(tileList, neighborIds, 60, 60, { true }, seed);
            if (auto success = wfc.run(); success.has_value()) {
                writeImagePng("assets/output.png", *success);
                Logger::info("Success");
                break;
            }

            Logger::info("Failure");
        }
    }
}