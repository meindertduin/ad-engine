#include <iostream>
#include "logging.h"

constexpr std::string EscapeSeqOrange = "\033[33m";
constexpr std::string EscapeSeqGreen = "\033[32m";
constexpr std::string EscapeSeqRed = "\033[31m";
constexpr std::string EscapeSeqReset = "\033[0m";

void Logger::log(Level level, const std::string &message) {
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
