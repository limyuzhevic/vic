// Profiler Configuration Header
#pragma once

#include <string>

namespace nlm {

/**
 * Profiler configuration
 */
struct ProfilerConfig {
    bool enableEventRecording;
    bool enableSampling;
    bool enableMetrics;
    double eventRecordingInterval;
    size_t maxEvents;
    std::string outputPath;
    std::string logLevel;
    
    ProfilerConfig();
    bool validate() const;
    std::string getOutputPath(const std::string& suffix = "") const;
};

} // namespace nlm