#include <bitset>
#include <array>
#include "terrain.h"
#include "engine/path.h"
#include "math/size.h"

#include <glm/glm.hpp>
#include <optional>
#include <vector>
#include <unordered_map>
#include <stb_image.h>

namespace wfc {
    static constexpr uint32_t Dimensions = 2;
    static constexpr uint32_t BitsetSize = 8;

    constexpr uint32_t LeftDimension = 0;
    constexpr uint32_t TopDimension = 1;
    constexpr uint32_t RightDimension = 2;
    constexpr uint32_t BottmDimension = 3;

    template<typename T>
    struct Pattern {
        uint32_t id;
        T value;
        std::array<std::bitset<8>, 2 * Dimensions> neighbouring;

        auto operator==(const Pattern<T>& other) const {
            return value == other.value;
        }

        auto operator<=>(const Pattern<T>& other) const {
            return value <=> other.value;
        }
    };

    std::unordered_map<ushort, Pattern<ushort>> readBitmapPatterns(ushort *data, const math::Size2D &size) {
        std::unordered_map<ushort, Pattern<ushort>> patterns;

        auto bufferSize = size.width() * size.height();

        uint32_t id = 0;
        for (auto i = 0u; i < bufferSize; i++) {
            auto x = i % size.width();
            auto y = i / size.width();

            auto value = data[i];

            // Add the pattern if it doesn't exist
            if (!patterns.contains(value)) {
                patterns[value] = { id++, value, {} };
            }

            // Check for left pattern
            if (x > 0) {
                auto left = data[i - 1];
                if (!patterns.contains(left)) {
                    patterns[left] = { id++, left, {} };
                }

                patterns[value].neighbouring[LeftDimension].set(patterns[left].id);
                patterns[left].neighbouring[RightDimension].set(patterns[value].id);
            }

            // Check for top pattern
            if (y > 0) {
                auto top = data[i - size.width()];
                if (!patterns.contains(top)) {
                    patterns[top] = { id++, top, {} };
                }

                patterns[value].neighbouring[TopDimension].set(patterns[top].id);
                patterns[top].neighbouring[BottmDimension].set(patterns[value].id);
            }

            // Check for right pattern
            if (x < size.width() - 1) {
                auto right = data[i + 1];
                if (!patterns.contains(right)) {
                    patterns[right] = { id++, right, {} };
                }

                patterns[value].neighbouring[RightDimension].set(patterns[right].id);
                patterns[right].neighbouring[LeftDimension].set(patterns[value].id);
            }

            // Check for bottom pattern
            if (y < size.height() - 1) {
                auto bottom = data[i + size.width()];
                if (!patterns.contains(bottom)) {
                    patterns[bottom] = { id++, bottom, {} };
                }

                patterns[value].neighbouring[BottmDimension].set(patterns[bottom].id);
                patterns[bottom].neighbouring[TopDimension].set(patterns[value].id);
            }
        }

        return patterns;
    }

    template<typename T>
    struct WaveElement {
        glm::vec2 position;
        std::bitset<BitsetSize> possible;
        std::size_t possibleCount;
        std::optional<T> value = std::nullopt;
    };
}

namespace game {
    void Terrain::initialize() {

    }
}