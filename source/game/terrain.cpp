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
#include <algorithm>

namespace wfc {
    static constexpr uint32_t Dimensions = 2;
    static constexpr uint32_t BitsetSize = 8;
    static constexpr uint32_t PatternSize = 1;
    static constexpr uint32_t PatternSizeSquared = PatternSize * PatternSize;

    static constexpr uint32_t TotalDimensions = 4;

    constexpr uint32_t LeftDimension = 0;
    constexpr uint32_t TopDimension = 1;
    constexpr uint32_t RightDimension = 2;
    constexpr uint32_t BottomDimension = 3;

    float getRandomFloat(float min, float max)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(min, max);

        return dis(gen);
    }

    template<typename T, std::size_t V>
    struct PatternValue {
        std::array<T, V> data;

        PatternValue() = default;
        explicit PatternValue(std::array<T, V> values)
            : data(values)
        {
        }

        auto operator==(const PatternValue<T, V>& other) const {
            return (*this) == other ? std::strong_ordering::equal : this->hash() <=> other.hash();
        }

        auto operator<=>(const PatternValue<T, V>& other) const {
            return hash() <=> other.hash();
        }

        uint64_t hash() {
            if (_hash) {
                return _hash;
            }

            uint64_t v = 0x12345;
            for (const auto &x : data) {
                v ^= x + 0x9e3779b9 + (v << 6) + (v >> 2);
            }

            return _hash = v;
        }
    private:
        mutable uint64_t _hash { 0 };
    };

    template<typename T>
    struct Pattern;

    template<typename T>
    using SizedPatternValue = PatternValue<T, PatternSizeSquared>;

    template<typename T>
    using SizedPattern = Pattern<uint32_t>;

    template<typename T>
    using SizedPatternMap = std::unordered_map<uint64_t, SizedPattern<T>>;

    template<typename T>
    struct Pattern {
        uint32_t id { 0 };
        float frequency { 0 };
        SizedPatternValue<T> value;
        std::array<std::bitset<8>, TotalDimensions> neighbouring;

        Pattern() = default;
        Pattern(uint32_t id, const SizedPatternValue<T> &value)
            : id(id)
            , value(value)
        {}

        auto operator==(const Pattern<T>& other) const {
            return id == other.id;
        }

        auto operator<=>(const Pattern<T>& other) const {
            return id <=> other.id;
        }
    };

    template<typename T>
    std::vector<SizedPatternValue<T>> getPatternValues(T *data, const math::Size2D &size) {
        std::vector<SizedPatternValue<T>> patternValues;

        auto bufferSize = size.width() * size.height();
        if (bufferSize % PatternSizeSquared != 0) {
            throw std::runtime_error("Invalid buffer size");
        }

        auto patternCount = bufferSize / PatternSizeSquared;
        patternValues.reserve(patternCount);

        for (auto i = 0; i < bufferSize; i += PatternSizeSquared) {
            std::array<T, PatternSizeSquared> values {};
            auto startX = i % size.width();
            auto startY = i / size.width();

            for (auto y = 0; y < PatternSize; y++) {
                for (auto x = 0; x < PatternSize; x++) {
                    auto index = (startY + y) * size.width() + (startX + x);
                    values[y * PatternSize + x] = data[index];
                }
            }

            auto patternValue = PatternValue<T, PatternSizeSquared>(values);
            patternValues.push_back(patternValue);
        }

        return patternValues;
    }

    template<typename T>
    SizedPatternMap<T> readBitmapPatterns(T *data, const math::Size2D &size) {
        auto patternValues = getPatternValues(data, size);
        SizedPatternMap<T> patterns;
        std::unordered_map<uint64_t, int> frequencies;

        auto valuesSize = patternValues.size();

        uint32_t id = 0;
        for (auto i = 0u; i < valuesSize; i++) {
            auto x = i % size.width();
            auto y = i / size.width();

            auto value = patternValues[i];

            // Increment frequency
            frequencies[value.hash()]++;

            // Add the pattern if it doesn't exist
            if (!patterns.contains(value.hash())) {
                patterns[value.hash()] = SizedPattern<T>(id++, value);
            }

            // Check for left pattern
            if (x > 0) {
                auto left = patternValues[i - 1];
                if (!patterns.contains(left.hash())) {
                    patterns[left.hash()] = SizedPattern<T>(id++, left);
                }

                patterns[value.hash()].neighbouring[LeftDimension].set(patterns[left.hash()].id);
                patterns[left.hash()].neighbouring[RightDimension].set(patterns[value.hash()].id);
            }

            // Check for top pattern
            if (y > 0) {
                auto top = patternValues[i - size.width()];
                if (!patterns.contains(top.hash())) {
                    patterns[top.hash()] = SizedPattern<T>(id++, top);
                }

                patterns[value.hash()].neighbouring[TopDimension].set(patterns[top.hash()].id);
                patterns[top.hash()].neighbouring[BottomDimension].set(patterns[value.hash()].id);
            }

            // Check for right pattern
            if (x < size.width() - 1) {
                auto right = patternValues[i + 1];
                if (!patterns.contains(right.hash())) {
                    patterns[right.hash()] = SizedPattern<T>(id++, right);
                }

                patterns[value.hash()].neighbouring[RightDimension].set(patterns[right.hash()].id);
                patterns[right.hash()].neighbouring[LeftDimension].set(patterns[value.hash()].id);
            }

            // Check for bottom pattern
            if (y < size.height() - 1) {
                auto bottom = patternValues[i + size.width()];
                if (!patterns.contains(bottom.hash())) {
                    patterns[bottom.hash()] = SizedPattern<T>(id++, bottom);
                }

                patterns[value.hash()].neighbouring[BottomDimension].set(patterns[bottom.hash()].id);
                patterns[bottom.hash()].neighbouring[TopDimension].set(patterns[value.hash()].id);
            }
        }

        // Calculate frequencies
        for (const auto& [hash, frequency] : frequencies) {
            patterns[hash].frequency = static_cast<float>(frequency);
        }

        // Normalize frequencies
        auto totalFrequency = 0.0f;
        for (auto& [value, pattern] : patterns) {
            totalFrequency += pattern.frequency;
        }

        for (auto& [value, pattern] : patterns) {
            pattern.frequency /= totalFrequency;
        }

        return patterns;
    }

    template<typename T>
    SizedPattern<T>& findPatternWithId(SizedPatternMap<T>& patterns, uint32_t id) {
        for (auto& [value, pattern] : patterns) {
            if (pattern.id == id) {
                return pattern;
            }
        }

        throw std::runtime_error("Pattern not found");
    }

    template<typename T>
    struct WaveElement {
        glm::ivec2 position;
        std::bitset<BitsetSize> possible;
        std::size_t possibleCount;
        std::optional<SizedPatternValue<T>> value = std::nullopt; // is nullopt when not collapsed

        float sumWeights = 0.0f;
        float sumWeightsLogWeights = 0.0f;
        float entropy = 0.0f;

        void initialize(std::size_t possibleCount, SizedPatternMap<T> &patterns) {
            for (auto i = 0u; i < possibleCount; i++) {
                possible.set(i);
            }

            this->possibleCount = possibleCount;

            for (auto i = 0u; i < BitsetSize; i++) {
                if (possible[i]) {
                    auto &pattern = findPatternWithId<T>(patterns, i);

                    const auto weight = pattern.frequency;
                    sumWeights += weight;
                    sumWeightsLogWeights += weight * std::log(weight);
                }
            }
        }

        bool apply(const std::bitset<BitsetSize> &mask, SizedPatternMap<T> &patterns) {
            auto const diff = possible & mask;

            if (diff == possible) {
                return true;
            }

            possible &= mask;

            for (auto i = 0u; i < BitsetSize; i++) {
                if (diff[i]) {
                    auto &pattern = findPatternWithId<T>(patterns, i);

                    const auto weight = pattern.frequency;
                    sumWeights -= weight;
                    sumWeightsLogWeights -= weight * std::log(weight);
                }
            }

            entropy = std::log(sumWeights) - sumWeightsLogWeights / sumWeights;
            possibleCount = possible.count();

            return possibleCount !=0;
        }

        bool collapse(const SizedPattern<T>* pattern) {
            if (!possible[pattern->id]) {
                return false;
            }

            value = pattern->value;
            possible.reset();
            possible.set(pattern->id);
            possibleCount = 1;
            sumWeights = 0.0f;
            sumWeightsLogWeights = 0.0f;
            entropy = 0.0f;

            return true;
        }

        [[nodiscard]] constexpr bool collapsed() const {
            return value.has_value();
        }
    };


    template<typename T>
    struct WaveFunctionOutput {
        std::vector<WaveElement<T>> elements;
        math::Size2D outputSize;
        uint32_t collapsedCount { 0 };

        WaveFunctionOutput(const math::Size2D &size, std::size_t possibleCount, SizedPatternMap<T> &patterns)
            : outputSize(size)
        {
            auto totalSize = size.width() * size.height();
            elements = std::vector<WaveElement<T>>(totalSize);

            if (size.width() % PatternSize != 0 || size.height() % PatternSize != 0) {
                throw std::runtime_error("Size must be a multiple of pattern size");
            }

            for (auto i = 0; i < totalSize; i += PatternSizeSquared) {
                auto startX = i % size.width();
                auto startY = i / size.width();

                auto index = startY / PatternSize * (size.width() / PatternSize) + startX / PatternSize;
                elements[index].position = { startX, startY };
                elements[index].initialize(possibleCount, patterns);
            }
        }
    };

    template<typename T>
    const SizedPattern<T>* getRandomPattern(const WaveElement<T> &element, SizedPatternMap<T> &patterns) {
        // Possible patterns
        std::vector<SizedPattern<T>*> possiblePatterns;
        std::vector<float> possibleFrequencies;
        float totalFrequency = 0.0f;

        // Get all possible patterns
        for (auto &[value, pattern] : patterns) {
            if (element.possible[pattern.id]) {
                possiblePatterns.push_back(&pattern);
                possibleFrequencies.push_back(pattern.frequency);
                totalFrequency += pattern.frequency;
            }
        }

        // Get a random pattern
        auto randomIndex = getRandomFloat(0.0f, totalFrequency);
        float acc = 0.0f;
        for (auto i = 0u; i < possiblePatterns.size(); i++) {
            acc += possibleFrequencies[i];
            if (acc >= randomIndex) {
                return possiblePatterns[i];
            }
        }

        throw std::runtime_error("Could not get random pattern");
    }

    template<typename T>
    const SizedPattern<T>* getSinglePossibleValue(const WaveElement<T> &element, SizedPatternMap<T> &patterns) {
        // Get all possible patterns
        for (auto &[value, pattern] : patterns) {
            if (element.possible[pattern.id]) {
                return &pattern;
            }
        }

        throw std::runtime_error("No possible data");
    }

    template<typename T>
    bool propagateElement(WaveElement<T> &inputElement, WaveFunctionOutput<T> &output, SizedPatternMap<T> &patterns) {
        std::stack<WaveElement<T>*> elementsStack;
        elementsStack.push(&inputElement);

        bool contradiction = false;

        while (!elementsStack.empty()) {
            auto &element = *elementsStack.top();
            elementsStack.pop();

            // Get non-collapsed neighbours
            std::vector<std::pair<uint32_t, WaveElement<T>*>> neighbours;

            for (auto i = 0; i < TotalDimensions; i++) {
                auto neighbourPosition = element.position;
                if (i == LeftDimension) {
                    neighbourPosition.x -= 1;
                } else if (i == TopDimension) {
                    neighbourPosition.y -= 1;
                } else if (i == RightDimension) {
                    neighbourPosition.x += 1;
                } else if (i == BottomDimension) {
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
                auto elementPattern = patterns[element.value.value().hash()];

                if (!neighbor->apply(elementPattern.neighbouring[neighborPair.first], patterns)) {
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
    bool collapseElement(WaveElement<T> *element, WaveFunctionOutput<T> &output, SizedPatternMap<T> &patterns) {
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
        float min = 1e4;
        WaveElement<T>* next = nullptr;

        for (auto &element : output.elements) {
            if (element.collapsed()) {
                continue;
            }

            auto noise = getRandomFloat(0.0f, 1e-6f);
            if (element.entropy + noise < min) {
                min = element.entropy + noise;
                next = &element;
            }
        }

        if (next == nullptr) {
            throw std::runtime_error("No next element");
        }

        return next;
    }

    template<typename T>
    WaveFunctionOutput<T> output(SizedPatternMap<T> patterns, math::Size2D outputSize) {
        WaveFunctionOutput<T> output { outputSize, patterns.size(), patterns };

        // Start with top left element
        auto middle = outputSize.width() * outputSize.height() / 2;
        auto nextElement = &output.elements[middle];

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
        auto data = stbi_load("assets/lake.png", &width, &height, &channels, 0);

        if (!data) {
            throw std::runtime_error("Failed to load texture");
        }

        auto patterns = wfc::readBitmapPatterns(reinterpret_cast<uint32_t*>(data), { width, height });
        stbi_image_free(data);

        auto output = wfc::output<uint32_t>(patterns, { 18, 18 });

        auto outputData = new uint32_t [output.outputSize.width() * output.outputSize.height()];
        for (auto i = 0u; i < output.elements.size(); i++) {
            auto &element = output.elements[i];
            outputData[i] = element.value.value().data[0];
        }

        for (auto &element : output.elements) {
            auto startX = element.position.x;
            auto startY = element.position.y;

            for (auto y = 0; y < wfc::PatternSize; y++) {
                for (auto x = 0; x < wfc::PatternSize; x++) {
                    auto index = (startY * wfc::PatternSize + y) * output.outputSize.width() * wfc::PatternSize + startX * wfc::PatternSize + x;
                    outputData[index] = element.value.value().data[y * wfc::PatternSize + x];
                }
            }
        }

        stbi_write_png("assets/output.png", output.outputSize.width(), output.outputSize.height(), channels, outputData, output.outputSize.width() * channels);
        delete[] outputData;
    }
}