#include <cstdio>
#include <iostream>
#include "logging.h"

constexpr std::string EscapeSeqOrange = "\033[33m";
constexpr std::string EscapeSeqGreen = "\033[32m";
constexpr std::string EscapeSeqRed = "\033[31m";
constexpr std::string EscapeSeqReset = "\033[0m";

void Logger::info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(Level::Info, format, args);
    va_end(args);
}

void Logger::debug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(Level::Debug, format, args);
    va_end(args);
}

void Logger::warning(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(Level::Warning, format, args);
    va_end(args);
}

void Logger::error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(Level::Error, format, args);
    va_end(args);
}

void Logger::fatal(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(Level::Fatal, format, args);
    va_end(args);
}

void Logger::log(Logger::Level level, const char *format, ...) {
    va_list args;
    va_start(args, format);
    log(level, format, args);
    va_end(args);
}

void Logger::log(Logger::Level level, const char *format, va_list args) {
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    log(level, buffer);
}

void Logger::log(Level level, char* message) {
    switch (level) {
        case Level::Info:
            std::cout << "[INFO] " << message << std::endl;
            break;
        case Level::Debug:
            std::cout << EscapeSeqGreen << "[DEBUG] " << message << EscapeSeqReset << std::endl;
            break;
        case Level::Warning:
            std::cout << EscapeSeqOrange << "[WARNING] " << message << EscapeSeqReset << std::endl;
            break;
        case Level::Error:
            std::cout << EscapeSeqRed << "[ERROR] " << message << EscapeSeqReset << std::endl;
            break;
        case Level::Fatal:
            std::cout << EscapeSeqRed << "[FATAL] " << message << EscapeSeqReset << std::endl;
            break;
    }
}
