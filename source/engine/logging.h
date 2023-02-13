#pragma once

#include "format.h"

class Logger {
public:
    template<typename ...Args>
    static void info(const std::string &format, Args &&...args) {
        auto message = formatString(format, std::forward<Args>(args)...);
        log(Level::Info, message.c_str());
    }

    template<typename ...Args>
    static void debug(const std::string &format, Args &&...args) {
        auto message = formatString(format, std::forward<Args>(args)...);
        log(Level::Debug, message.c_str());
    }

    template<typename ...Args>
    static void warning(const std::string &format, Args &&...args) {
        auto message = formatString(format, std::forward<Args>(args)...);
        log(Level::Warning, message.c_str());
    }

    template<typename ...Args>
    static void error(const std::string &format, Args &&...args) {
        auto message = formatString(format, std::forward<Args>(args)...);
        log(Level::Error, message.c_str());
    }

    template<typename ...Args>
    static void fatal(const std::string &format, Args &&...args) {
        auto message = formatString(format, std::forward<Args>(args)...);
        log(Level::Fatal, message.c_str());
    }

private:
    enum class Level {
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    static void log(Level level, const std::string &message);
};

#define LOG_DEBUG(format, ...) \
Logger::debug(formatString("[{}, {}, {}] ", __FILE_NAME__, __FUNCTION__, __LINE__) + format, ##__VA_ARGS__)

