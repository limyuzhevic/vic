#ifndef NLM_TIMEPROFILER_H
#define NLM_TIMEPROFILER_H

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <iomanip>
#include <numeric>

namespace nlm {

class TimeProfiler {
public:
    struct ProfileResult {
        std::string operation;
        std::chrono::microseconds duration;
        size_t callCount;
        double averageTimeUs;
        double minTimeUs;
        double maxTimeUs;
        double stdDevUs;
    };

    TimeProfiler() = default;
    ~TimeProfiler() = default;

    // Start profiling a named operation
    void startProfile(const std::string& operationName);

    // Stop profiling and record the result
    void stopProfile(const std::string& operationName);

    // Get results for a specific operation
    const ProfileResult* getResult(const std::string& operationName) const;

    // Get all results
    const std::vector<ProfileResult>& getAllResults() const { return results_; }

    // Clear all profiling data
    void clear();

    // Print profiling summary to stdout
    void printSummary() const;

    // Print results for a specific operation
    void printOperationResults(const std::string& operationName) const;

    // Get total time for all operations
    double getTotalTime() const;

    // Get total call count for all operations
    size_t getTotalCallCount() const;

    // Reset timers for all operations
    void reset();

    // Set whether profiling is enabled
    void setEnabled(bool enabled) { enabled_ = enabled; }

    // Check if profiling is enabled
    bool isEnabled() const { return enabled_; }

private:
    struct ProfileData {
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
        std::chrono::microseconds accumulatedTime{0};
        size_t callCount{0};
    };

    std::unordered_map<std::string, ProfileData> activeProfiles_;
    std::vector<ProfileResult> results_;
    bool enabled_{true};
    mutable bool printingDisabled_{false};
};

} // namespace nlm

#endif // NLM_TIMEPROFILER_H
