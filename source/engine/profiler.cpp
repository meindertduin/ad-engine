#include "profiler.h"
#include "window.h"

void Profiler::begin(const std::string &name) {
    startTimeMs = mWindow->getTicks();
    lastEntryTimeMs = startTimeMs;
    writeEntry(name);
}

void Profiler::writeEntry(const std::string &name) {
    uint32_t endTimeMs = mWindow->getTicks();
    uint32_t durationMs = endTimeMs - lastEntryTimeMs;
    lastEntryTimeMs = endTimeMs;
    Logger::info("{} took {} ms", name, durationMs);
}

Profiler &Profiler::instance() {
    return *sInstance;
}

void Profiler::createInstance(AdWindow *window) {
    sInstance = new Profiler(window);
}
