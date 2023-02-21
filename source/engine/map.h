#pragma once

#include <cstdint>

template<typename T>
struct HashFunc {
    auto operator()(const T &value) const -> uint32_t {
        return value;
    }
};

template<typename K, typename T>
class HashMap {
public:

private:

};