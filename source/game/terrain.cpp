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
#include <random>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stbi_image_write.h>

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

    template<typename T>
    std::unordered_map<T, Pattern<T>> readBitmapPatterns(T *data, const math::Size2D &size) {
        std::unordered_map<T, Pattern<T>> patterns;

        auto bufferSize = size.width() * size.height();

        auto middle = size.width() / 2 + size.height() / 2 * size.width();
        auto middleValue = data[middle];

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
        glm::ivec2 position;
        std::bitset<BitsetSize> possible;
        std::size_t possibleCount;
        std::optional<T> value = std::nullopt; // is nullopt when not collapsed

        bool collapse(const Pattern<T>* pattern) {
            this->value = pattern->value;
            possible.reset();
            possible.set(pattern->id);
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

        WaveFunctionOutput(const math::Size2D &size, std::size_t possibleCount)
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

    static inline std::random_device rd;
    static inline std::mt19937 gen(rd());

    template<typename T>
    const Pattern<T>* getRandomPattern(const WaveElement<T> &element, std::unordered_map<T, Pattern<T>> &patterns) {
        // Possible patterns
        std::vector<Pattern<T>*> possiblePatterns;

        // Get all possible patterns
        for (auto &[value, pattern] : patterns) {
            if (element.possible[pattern.id]) {
                possiblePatterns.push_back(&pattern);
            }
        }

        // Get a random pattern
        std::uniform_int_distribution<> dis(0, possiblePatterns.size() - 1);
        auto randomIndex = dis(gen);

        return possiblePatterns[randomIndex];
    }

    template<typename T>
    const Pattern<T>* getSinglePossibleValue(const WaveElement<T> &element, std::unordered_map<T, Pattern<T>> &patterns) {
        // Get all possible patterns
        for (auto &[value, pattern] : patterns) {
            if (element.possible[pattern.id]) {
                return &pattern;
            }
        }

        throw std::runtime_error("No possible values");
    }

    template<typename T>
    bool propagateElement(WaveElement<T> &inputElement, WaveFunctionOutput<T> &output, std::unordered_map<T, Pattern<T>> &patterns) {
        std::stack<WaveElement<T>*> elementsStack;
        elementsStack.push(&inputElement);

        bool contradiction = false;

        while (!elementsStack.empty()) {
            auto &element = *elementsStack.top();
            elementsStack.pop();

            // Get non-collapsed neighbours
            std::vector<std::pair<uint32_t, WaveElement<T>*>> neighbours;

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
                    neighbours.push_back(std::make_pair(i, &neighbour));
                }
            }

            // Loop through non-collapsed neighbors
            for (std::pair<uint32_t, WaveElement<T>*> neighborPair : neighbours) {
                auto neighbor = neighborPair.second;

                // Apply possible patterns for neighbor
                auto elementPattern = patterns[element.value.value()];

                if (!neighbor->apply(elementPattern.neighbouring[neighborPair.first])) {
                    // Contradiction
                    contradiction = true;
                    break;
                }

                if (neighbor->possibleCount == 0) {
                    contradiction = true;
                    break;
                } else if (neighbor->possibleCount == 1) {
                    if (!neighbor->collapse(getSinglePossibleValue(*neighbor, patterns))) {
                        contradiction = true;
                        break;
                    }

                    output.collapsedCount++;

                    elementsStack.push(neighbor);
                }
            }
        }

        if (contradiction) {
            return false;
        }

        return true;
    }

    template<typename T>
    bool collapseElement(WaveElement<T> *element, WaveFunctionOutput<T> &output, std::unordered_map<T, Pattern<T>> &patterns) {
        // Collapse the element
        auto pattern = getRandomPattern(*element, patterns);
        if (!element->collapse(pattern)) {
            Logger::error("Failed to collapse element");
            return false;
        }

        if (element->collapsed()) {
            output.collapsedCount++;
        }

        // Propagate the collapsed element
        if (!propagateElement(*element, output, patterns)) {
            Logger::error("Failed to propagate element");
            return false;
        }

        return true;
    }

    template<typename T>
    WaveElement<T>* getNextElement(WaveFunctionOutput<T> &output) {
        std::size_t min = std::numeric_limits<uint32_t>::max();
        WaveElement<T>* next = nullptr;

        for (auto &element : output.elements) {
            if (element.collapsed()) {
                continue;
            }

            if (element.possibleCount < min) {
                min = element.possibleCount;
                next = &element;
            }
        }

        if (next == nullptr) {
            throw std::runtime_error("No next element");
        }

        return next;
    }

    template<typename T>
    WaveFunctionOutput<T> output(std::unordered_map<T, Pattern<T>> patterns, math::Size2D outputSize) {
        WaveFunctionOutput<T> output { outputSize, patterns.size() };

        // Start with top left element
        auto nextElement = &output.elements[0];

        // Collapse the middle element
        while (output.collapsedCount != outputSize.width() * outputSize.height()) {
            if (!collapseElement(nextElement, output, patterns)) {
                break;
            }

            // Find the next element to collapse
            if (output.collapsedCount != outputSize.width() * outputSize.height()) {
                nextElement = getNextElement(output);
            }
        }

        return output;
    }
}

namespace game {
    void Terrain::initialize() {
        int width, height, channels;

        stbi_set_flip_vertically_on_load(false);
        auto data = stbi_load("assets/city.png", &width, &height, &channels, 0);

        if (!data) {
            throw std::runtime_error("Failed to load texture");
        }

        auto patterns = wfc::readBitmapPatterns(reinterpret_cast<uint32_t*>(data), { width, height });
        stbi_image_free(data);

        auto output = wfc::output(patterns, { 32, 32 });
        auto outputData = new uint32_t [output.outputSize.width() * output.outputSize.height()];
        for (auto i = 0u; i < output.elements.size(); i++) {
            outputData[i] = output.elements[i].value.value();
        }
        stbi_write_png("assets/output.png", output.outputSize.width(), output.outputSize.height(), channels, outputData, output.outputSize.width() * channels);
        delete[] outputData;
    }
}