#pragma once

#include <array>
#include <cstdint>
#include "math/size.h"

#include <unordered_map>
#include <vector>
#include <functional>

namespace game {
    static constexpr uint32_t Dimensions = 2;
    static constexpr uint32_t BitsetSize = 64;
    static constexpr uint32_t PatternSize = 1;
    static constexpr uint32_t PatternSizeSquared = PatternSize * PatternSize;

    class Pattern {
    public:
        Pattern(const std::array<uint32_t, PatternSizeSquared> &data)
            : mData(data)
        {}

        auto operator==(const Pattern& other) const {
            return hash() == other.hash() && mData == other.mData;
        }

        auto operator<=>(const Pattern& other) const {
            return (*this) == other ? std::strong_ordering::equal : hash() <=> other.hash();
        }

        uint64_t hash() const {
            if (_hash) {
                return _hash;
            }

            uint64_t v = 0x12345;
            for (const auto &x : mData) {
                v ^= x + 0x9e3779b9 + (v << 6) + (v >> 2);
            }
            return this->_hash = v;
        }
    private:
        std::array<uint32_t, PatternSizeSquared> mData;
        mutable uint64_t _hash;
    };

    class InputData {
    public:
        InputData(uint32_t *data, const math::Size2D &size, int N, bool periodicInput, int symmetry)
        {
            auto bufferSize = size.width() * size.height();

            std::vector<char> sample(bufferSize);

            for (int i = 0; i < bufferSize; i++) {
                uint32_t color = data[i];
                int k = 0;
                for (; k < mColors.size(); k++) {
                    if (mColors[k] == color) {
                        break;
                    }
                }

                if (k == mColors.size()) {
                    mColors.push_back(color);
                }

                sample[i] = static_cast<char>(k);
            }

            std::unordered_map<long, int> patternIndices;
            std::vector<double> weightList;

            int C = mColors.size();
            int xmax = periodicInput ? size.width() : size.width() - N + 1;
            int ymax = periodicInput ? size.height() : size.height() - N + 1;

            for (int y = 0; y < ymax; y++) {
                for (int x = 0; x < xmax; x++) {
                    std::vector<char> ps[8];

                    ps[0] = pattern([&](int dx, int dy) {
                        return sample[(x + dx) % size.width() + (y + dy) % size.height() * size.width()];
                    }, N);
                    ps[1] = reflect(ps[0], N);
                    ps[2] = rotate(ps[0], N);
                    ps[3] = reflect(ps[2], N);
                    ps[4] = rotate(ps[2], N);
                    ps[5] = reflect(ps[4], N);
                    ps[6] = rotate(ps[4], N);
                    ps[7] = reflect(ps[6], N);

                    for (int k = 0; k < symmetry; k++) {
                        std::vector<char> &p = ps[k];
                        auto h = hash(p, C);

                        if (patternIndices.find(h) == patternIndices.end()) {
                            patternIndices[h] = mPatterns.size();
                            weightList.push_back(1.0);
                            mPatterns.push_back(p);
                        } else {
                            weightList[patternIndices[h]] += 1;
                        }
                    }
                }
            }

            mWeights.resize(weightList.size());
            for (int i = 0; i < weightList.size(); i++) {
                mWeights[i] = weightList[i];
            }
        }

    private:
        static std::vector<char> pattern(const std::function<char(int, int)> &f, int n) {
            auto result = std::vector<char>(n * n);

            for (int y = 0; y < n; y++) {
                for (int x = 0; x < n; x++) {
                    result[y * n + x] = f(x, y);
                }
            }

            return result;
        }

        static std::vector<char> rotate(const std::vector<char> &p, int n) {
            return pattern([&p, n](int x, int y) {
                return p[n -1 - y + x * n];
            }, n);
        }

        static std::vector<char> reflect(const std::vector<char> &p, int n) {
            return pattern([&p, n](int x, int y) {
                return p[n - 1 - x + y * n];
            }, n);
        }

        static long hash(const std::vector<char> &p, int c) {
            long result = 0;
            long power = 1;
            for (int i = 0; i < p.size(); i++) {
                result += p[p.size() -1 -i] * power;
                power *= c;
            }

            return result;
        }

        std::vector<std::vector<char>> mPatterns;
        std::vector<uint32_t> mColors;
        std::vector<double> mWeights;
    };
}