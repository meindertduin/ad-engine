#include "logging.h"

constexpr std::string EscapeSeqOrange = "\033[33m";
constexpr std::string EscapeSeqGreen = "\033[32m";
constexpr std::string EscapeSeqRed = "\033[31m";
constexpr std::string EscapeSeqReset = "\033[0m";

void Logger::log(Level level, const std::string &message) {
    switch (level) {
        case Level::Info:
            printf("[INFO] %s\n", message.c_str());
            break;
        case Level::Debug:
            printf("%s[DEBUG] %s%s\n", EscapeSeqGreen.c_str(), message.c_str(), EscapeSeqReset.c_str());
            break;
        case Level::Warning:
            printf("%s[WARNING] %s%s\n", EscapeSeqOrange.c_str(), message.c_str(), EscapeSeqReset.c_str());
            break;
        case Level::Error:
            printf("%s[ERROR] %s%s\n", EscapeSeqRed.c_str(), message.c_str(), EscapeSeqReset.c_str());
            break;
        case Level::Fatal:
            printf("%s[FATAL] %s%s\n", EscapeSeqRed.c_str(), message.c_str(), EscapeSeqReset.c_str());
            break;
    }
}
