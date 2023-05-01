#pragma once

#include <string>
class AdWindow;

class Profiler {
public:
    explicit Profiler(AdWindow *window) : mWindow(window) {}
    static void createInstance(AdWindow *window);
    static Profiler& instance();

    void begin(const std::string &name);
    void writeEntry(const std::string &name);
private:
    AdWindow *mWindow;
    uint32_t startTimeMs { 0 };
    uint32_t lastEntryTimeMs { 0 };

    static inline Profiler *sInstance { nullptr };
};