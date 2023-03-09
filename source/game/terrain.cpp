#include <bitset>
#include <array>
#include "terrain.h"
#include "engine/path.h"
#include "math/size.h"
#include "engine/logging.h"

#include <glm/glm.hpp>
#include <optional>
#include <vector>
#include <unordered_map>
#include <stb_image.h>
#include <stack>

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
            return id == other.id;
        }

        auto operator<=>(const Pattern<T>& other) const {
            return id <=> other.id;
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
        std::optional<T> value = std::nullopt; // is nullopt when not collapsed

        bool collapse(const T &inputValue) {
            this->value = inputValue;
            possible.reset();
            possibleCount = 1;

            return true;
        }

        [[nodiscard]] constexpr bool collapsed() const {
            return value.has_value();
        }

        bool apply(const std::bitset<BitsetSize> &mask) {
            auto const diff = possible & mask;

            if (diff.none()) {
                return true;
            }

            possible &= mask;
            possibleCount = possible.count();

            return possible.count() !=0;
        }
    };


    template<typename T>
    struct WaveFunctionOutput {
        std::vector<WaveElement<T>> elements;
        math::Size2D outputSize;
        uint32_t collapsedCount { 0 };

        explicit WaveFunctionOutput(const math::Size2D &size, uint32_t possibleCount)
            : elements(size.width() * size.height())
            , outputSize(size)
        {
            for (auto y = 0u; y < size.height(); y++) {
                for (auto x = 0u; x < size.width(); x++) {
                    auto index = y * size.width() + x;
                    elements[index].position = { x, y };
                    elements[index].possible.set();
                    elements[index].possibleCount = possibleCount;
                }
            }
        }
    };

    template<typename T>
    Pattern<T>* getRandomPattern(const WaveElement<T> &element, const std::unordered_map<ushort, Pattern<T>> &patterns) {
        // Possible patterns
        std::vector<Pattern<T>*> possiblePatterns;

        // Get all possible patterns
        for (auto i = 0u; i < patterns.size(); i++) {
            if (element.possible[patterns[i].id]) {
                possiblePatterns.push_back(&patterns[i]);
            }
        }

        // Get a random pattern
        auto randomIndex = rand() % possiblePatterns.size();

        return possiblePatterns[randomIndex];
    }

    template<typename T>
    T& getSinglePossibleValue(const WaveElement<T> &element, const std::unordered_map<ushort, Pattern<T>> &patterns) {
        // Get all possible patterns
        for (auto i = 0u; i < patterns.size(); i++) {
            if (element.possible[patterns[i].id]) {
                return patterns[i].value;
            }
        }

        throw std::runtime_error("No possible values");
    }

    template<typename T>
    bool propagateElement(const WaveElement<T> &inputElement, WaveFunctionOutput<T> &output, const std::unordered_map<ushort, Pattern<T>> &patterns) {
        std::stack<WaveElement<T>*> elementsStack;
        elementsStack.push(&inputElement);

        bool contradiction = false;

        while (!elementsStack.empty()) {
            auto &element = *elementsStack.top();
            elementsStack.pop();

            // Get non-collapsed neighbours
            std::vector<WaveElement<T>*> neighbours;
            for (auto i = 0; i < Dimensions * 2; i++) {
                auto neighbourPosition = element.position;
                if (i == LeftDimension) {
                    neighbourPosition.x -= 1;
                } else if (i == TopDimension) {
                    neighbourPosition.y -= 1;
                } else if (i == RightDimension) {
                    neighbourPosition.x += 1;
                } else if (i == BottmDimension) {
                    neighbourPosition.y += 1;
                }

                // Check if the neighbour is valid
                if (neighbourPosition.x < 0 || neighbourPosition.x >= output.outputSize.width() ||
                    neighbourPosition.y < 0 || neighbourPosition.y >= output.outputSize.height()) {
                    // Invalid neighbour
                    continue;
                }

                auto &neighbour = output.elements[neighbourPosition.y * output.outputSize.width() + neighbourPosition.x];
                if (!neighbour.collapsed()) {
                    neighbours.push_back(&neighbour);
                }
            }

            for (WaveElement<T>* neighbor : neighbours) {
                auto oldPossibleCount = neighbor->possibleCount;

                neighbor->apply(element.possible);

                if (neighbor->possibleCount != oldPossibleCount) {
                    if (neighbor->possibleCount == 0) {
                        contradiction = true;
                        break;
                    } else if (neighbor->possibleCount == 1) {
                        auto collapseSucceeded = neighbor->collapse(getSinglePossibleValue(*neighbor, patterns));

                        if (!collapseSucceeded) {
                            contradiction = true;
                            break;
                        }

                        elementsStack.push(neighbor);
                    }
                }
            }
        }

        if (contradiction) {
            return false;
        }

        return true;
    }

    template<typename T>
    bool collapseElement(WaveElement<T> &element, WaveFunctionOutput<T> &output, const std::unordered_map<ushort, Pattern<T>> &patterns) {
        output.collapsedCount++;

        auto elementPosIndex = element.position.y * output.outputSize.width() + element.position.x;

        // Collapse the element
        auto pattern = getRandomPattern(element, patterns);

        // propagate collapse

        // Collapse the left element
        if (element.position.x > 0) {
            auto &leftElement = output.elements[elementPosIndex - 1];
            if (!leftElement.collapsed()) {
                leftElement.possible &= pattern->neighbouring[LeftDimension];
                leftElement.possibleCount = leftElement.possible.count();
            }
        }

        // Collapse the top element
        if (element.position.y > 0) {
            auto &topElement = output.elements[elementPosIndex - output.outputSize.width()];
            if (!topElement.collapsed()) {
                topElement.possible &= pattern->neighbouring[TopDimension];
                topElement.possibleCount = topElement.possible.count();
            }
        }

        // Collapse the right element
        if (element.position.x < output.outputSize.width() - 1) {
            auto &rightElement = output.elements[elementPosIndex + 1];
            if (!rightElement.collapsed()) {
                rightElement.possible &= pattern->neighbouring[RightDimension];
                rightElement.possibleCount = rightElement.possible.count();
            }
        }

        // Collapse the bottom element
        if (element.position.y < output.outputSize.height() - 1) {
            auto &bottomElement = output.elements[elementPosIndex + output.outputSize.width()];
            if (!bottomElement.collapsed()) {
                bottomElement.possible &= pattern->neighbouring[BottmDimension];
                bottomElement.possibleCount = bottomElement.possible.count();
            }
        }
    }

    template<typename T>
    WaveFunctionOutput<T> output(const std::unordered_map<T, Pattern<T>> patterns, math::Size2D outputSize) {
        WaveFunctionOutput<T> output { outputSize, patterns.size() };

        // Get the middle element
        auto middle = outputSize.width() * outputSize.height() / 2;
        auto middleElement = output.elements[middle];

        // Collapse the middle element
        while (output.collapsedCount != outputSize.width() * outputSize.height()) {
            // Collapse the middle element
            if (collapseElement(middleElement, output, patterns)) {
                output.collapsedCount++;
                continue;
            }

            Logger::error("Failed to collapse element");
            break;
        }

        return output;
    }
}

namespace game {
    void Terrain::initialize() {
        int width, height, channels;

        stbi_set_flip_vertically_on_load(false);
        auto data = stbi_load_16("assets/city.png", &width, &height, &channels, 0);

        if (!data) {
            throw std::runtime_error("Failed to load texture");
        }

        auto patterns = wfc::readBitmapPatterns((ushort*)data, { width, height });
        stbi_image_free(data);
    }
}