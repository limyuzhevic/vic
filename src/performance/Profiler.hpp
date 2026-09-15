#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <iomanip>
#include <sstream>

namespace nlm {

// High-resolution timer for performance measurement
class Timer {
public:
    Timer() : startTime_(std::chrono::high_resolution_clock::now()) {}
    
    // Start timing from current point
    void restart() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    // Get elapsed time in milliseconds
    double elapsedMs() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
        return static_cast<double>(duration.count());
    }
    
    // Get elapsed time in microseconds
    double elapsedUs() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        return static_cast<double>(duration.count());
    }
    
    // Get elapsed time in seconds
    double elapsedSeconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime_);
        return duration.count();
    }
    
    // Get elapsed time in nanoseconds
    double elapsedNs() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime_);
        return static_cast<double>(duration.count());
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
};

// Performance sample for profiling
struct PerformanceSample {
    std::string operationName;
    double durationMs;
    int callCount;
    double totalTime;
    double averageTime;
    double minTime;
    double maxTime;
    
    PerformanceSample() : durationMs(0.0), callCount(0), totalTime(0.0), 
                         averageTime(0.0), minTime(0.0), maxTime(0.0) {}
    
    PerformanceSample(const std::string& name, double duration) 
        : operationName(name), durationMs(duration), callCount(1), 
          totalTime(duration), averageTime(duration), minTime(duration), maxTime(duration) {}
};

// Performance profiler for monitoring system performance
class Profiler {
public:
    Profiler();
    ~Profiler();
    
    // Disable copying
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
    
    // Start profiling an operation
    void startProfile(const std::string& operationName);
    
    // Stop profiling and record the result
    void stopProfile(const std::string& operationName);
    
    // Get samples for an operation
    const std::vector<PerformanceSample>& getSamples(const std::string& operationName) const;
    
    // Get all operation names
    std::vector<std::string> getOperationNames() const;
    
    // Get total time spent in an operation
    double getTotalTime(const std::string& operationName) const;
    
    // Get average time per call
    double getAverageTime(const std::string& operationName) const;
    
    // Get number of calls
    int getCallCount(const std::string& operationName) const;
    
    // Clear all profiling data
    void clear();
    
    // Get profiling summary
    std::string getSummary() const;
    
    // Get top N slowest operations
    std::vector<PerformanceSample> getTopSlowOperations(size_t n = 10) const;
    
    // Check if profiler is active
    bool isActive() const { return active_; }
    
    // Enable/disable profiler
    void setActive(bool active) { active_ = active; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool active_;
};

// Global profiler instance
inline Profiler& getGlobalProfiler() {
    static Profiler profiler;
    return profiler;
}

// Convenience macros for timing
#define TIME_SCOPE(operationName) nlm::Timer timer##__LINE__; \
    timer##__LINE__.restart();

#define TIME_SCOPE_STOP(operationName) nlm::getGlobalProfiler().stopProfile(operationName);

// High-performance timing macros (inline, no profiler overhead)
#define TIME_NOW() std::chrono::high_resolution_clock::now()
#define TIME_DIFF(start, end) std::chrono::duration_cast<std::chrono::microseconds>((end) - (start)).count()

} // namespace nlm