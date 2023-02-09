#pragma once

#include <cstdarg>

class Logger {
public:
    static void info(const char *format, ...);
    static void debug(const char *format, ...);
    static void warning(const char *format, ...);
    static void error(const char *format, ...);
    static void fatal(const char *format, ...);

private:
    enum class Level {
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };

    static void log(Level level, const char *format, ...);
    static void log(Level level, const char *format, va_list args);
    static void log(Level level, char* message);
};

