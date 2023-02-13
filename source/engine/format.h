#pragma once

#include <string>
#include "utils/types.h"

template<typename T = char>
concept BoundedCharArray = std::is_bounded_array_v<T>;

template<typename T>
struct FormatType {
    static std::string format(const T &value) {
        return std::to_string(value);
    }
};

template<>
struct FormatType<std::basic_string_view<char>> {
    static std::string format(const std::basic_string_view<char> &value) {
        return std::string(value);
    }
};

template<BoundedCharArray T>
struct FormatType<T> {
    static std::string format(const T &value) {
        return std::string(value);
    }
};

template<>
struct FormatType<std::string> {
    static std::string format(const std::string &value) {
        return value;
    }
};

template<>
struct FormatType<const char *> {
    static std::string format(const char *value) {
        return value;
    }
};

namespace detail {
    template<typename T>
    void formatString(std::string &result, const std::string &format, int &index, const T &value) {
        auto pos = format.find("{}", index);
        if (pos == std::string::npos) {
            return;
        }

        result += format.substr(index, pos - index);
        result += FormatType<T>::format(value);

        index = static_cast<int>(pos) + 2;
    }
}

template<typename ...Args>
std::string formatString(const std::string &format, Args &&...args) {
    if (format.empty()) {
        return "";
    }

    std::string result;
    result.reserve(format.size());

    int index = 0;
    (detail::formatString(result, format, index, std::forward<Args>(args)), ...);

    result += format.substr(index);

    return result;
}