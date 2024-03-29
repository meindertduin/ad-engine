#include "terrain_generator.h"

#include "engine/file_reader.h"
#include <fastwfc/tiling_wfc.hpp>
#include <json/json.h>
#include <unordered_map>

#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi_image_write.h>

struct Color {
    unsigned char r, g, b;

    bool operator==(const Color& other) const noexcept = default;
};

namespace std {
    template <>
    class hash<Color> {
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

    struct TilesMapData {
        struct Tile {
            int id;
            Color color;
            glm::vec2 textCoordinatesPos;
            glm::vec2 textureSize;
        };

        std::string textureMap;
        math::Size2D size;
        std::unordered_map<Color, Tile> tiles;
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

    Array2D<Color> generateWfcImage(const std::string &folder) {
        std::unordered_map<std::string, Tile<Color>> colorTiles;
        std::unordered_map<std::string, uint32_t> tileIds;
        std::vector<Tile<Color>> tileList;

        std::vector<std::tuple<std::string, uint32_t, std::string, uint32_t>> neighbors;
        std::vector<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>> neighborIds;

        auto tiledData = readJsonData(folder);

        for (const auto &[tileName, tile] : tiledData.tiles) {
            auto imagePath = folder + "/" + tileName + ".png";
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
                return *success;
            }

            Logger::info("Failure");
        }
    }

    TilesMapData readMap(const std::string &directory) {
        TilesMapData result;

        FileReader fileReader(directory + "/map.json");
        Json::Reader reader;
        Json::Value obj;

        reader.parse(fileReader.getFileContent(), obj);

        result.textureMap = obj["textureMap"].asString();

        auto width = obj["width"].asInt();
        auto height = obj["height"].asInt();
        result.size = { width, height };

        int id = 0;
        for (const auto& tile : obj["tiles"]) {
            TilesMapData::Tile tileData {
                .color = { static_cast<uint8_t>(tile["r"].asInt()), static_cast<uint8_t>(tile["g"].asInt()), static_cast<uint8_t>(tile["b"].asInt()) },
                .textCoordinatesPos = {tile["x"].asInt(), tile["y"].asInt() },
                .textureSize = { tile["width"].asInt(), tile["height"].asInt() },
            };

            tileData.id = id++;
            result.tiles[tileData.color] = tileData;
        }

        return result;
    }

    std::unique_ptr<gfx::Mesh> generateTileMesh(const TilesMapData &map, const TilesMapData::Tile &tile, float tileSize) {
        // This function can be optimized greatly
        std::vector<gfx::Vertex> vertices;

        auto normalizedXStart = tile.textCoordinatesPos.x / (float) map.size.width();
        auto normalizedYStart = tile.textCoordinatesPos.y / (float) map.size.height();
        auto normalizedXEnd = (tile.textCoordinatesPos.x + tile.textureSize.x) / (float) map.size.width();
        auto normalizedYEnd = (tile.textCoordinatesPos.y + tile.textureSize.y) / (float) map.size.height();

        float tileStart = -tileSize / 2.0f;
        float tileEnd = tileSize / 2.0f;

        gfx::Vertex rightTop = { { tileEnd, 0.0f, tileStart }, { 0.0f, 1.0f, 0.0f }, { normalizedXEnd, normalizedYStart } };
        gfx::Vertex rightBottom = { { tileEnd, 0.0f, tileEnd }, { 0.0f, 1.0f, 0.0f }, { normalizedXEnd, normalizedYEnd } };
        gfx::Vertex leftBottom = { { tileStart, 0.0f, tileEnd }, { 0.0f, 1.0f, 0.0f }, { normalizedXStart, normalizedYEnd } };
        gfx::Vertex leftTop = { { tileStart, 0.0f, tileStart }, { 0.0f, 1.0f, 0.0f }, { normalizedXStart, normalizedYStart } };

        vertices.push_back(rightTop);
        vertices.push_back(rightBottom);
        vertices.push_back(leftTop);

        vertices.push_back(rightBottom);
        vertices.push_back(leftBottom);
        vertices.push_back(leftTop);

        for (auto i = 0; i < vertices.size(); i += 3) {
            auto& v1 = vertices[i];
            auto& v2 = vertices[i + 1];
            auto& v3 = vertices[i + 2];

            auto normal = glm::normalize(glm::cross(v3.position - v2.position, v3.position - v1.position));

            v1.normal = normal;
            v2.normal = normal;
            v3.normal = normal;
        }

        return std::make_unique<gfx::Mesh>(vertices);
    }

    std::unique_ptr<TerrainData> TerrainGenerator::generateTerrain(const std::string &folder) {
        auto output = generateWfcImage(folder);
        auto map = readMap(folder);

        auto result = std::make_unique<TerrainData>();
        result->size = { static_cast<int>(output.width), static_cast<int>(output.height) };
        result->tileSize = 1.0f;

        result->tiles.reserve(output.height * output.width);

        for (auto i = 0; i < output.height; i++) {
            for (auto j = 0; j < output.width; j++) {
                auto color = output.data[i * output.width + j];
                auto tile = map.tiles[color];

                if (!result->tilesSet.contains(tile.id)) {
                    auto resultTile = std::make_shared<TerrainTile>();
                    auto tileMesh = generateTileMesh(map, tile, 1.0f);

                    resultTile->setMesh(std::move(tileMesh));
                    resultTile->setMaterial(Path {"assets/material_scripts/background.lua"});

                    result->tilesSet.try_emplace(tile.id, resultTile);
                }

                result->tiles.push_back(tile.id);
            }
        }

        return result;
    }
}