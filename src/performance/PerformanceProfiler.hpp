// PerformanceProfiler.hpp - New performance profiler
#pragma once

#include "Performance.hpp"
#include <string>
#include <chrono>
#include <unordered_map>
#include <limits>

namespace nlm {

class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler() = default;
    
    // Disable copying
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
    // Start/stop profiling
    void startProfiling(const std::string& operationName);
    void stopProfiling(const std::string& operationName);
    
    // Profile a function call
    template<typename Func, typename... Args>
    auto profile(const std::string& operationName, Func&& func, Args&&... args);
    
    // Get performance statistics
    double getAverageTime(const std::string& operationName) const;
    double getMinTime(const std::string& operationName) const;
    double getMaxTime(const std::string& operationName) const;
    size_t getCallCount(const std::string& operationName) const;
    
    // Get overall statistics
    const PerformanceMetrics& getOverallMetrics() const { return overallMetrics_; }
    
    // Get all metrics
    const std::unordered_map<std::string, PerformanceMetrics>& getDetailedMetrics() const { return detailedMetrics_; }
    
    // Reset profiler state
    void reset();
    
    // Enable/disable profiling
    void enable(bool enable) { enabled_ = enable; }
    bool isEnabled() const { return enabled_; }
    
    // Get singleton instance
    static PerformanceProfiler& getInstance() {
        static PerformanceProfiler instance;
        return instance;
    }
    
private:
    struct OperationMetrics {
        double totalTime;
        double minTime;
        double maxTime;
        size_t callCount;
        std::chrono::steady_clock::time_point startTime;
        
        OperationMetrics() : totalTime(0.0), minTime(std::numeric_limits<double>::max()),
                            maxTime(0.0), callCount(0) {}
    };
    
    bool enabled_;
    PerformanceMetrics overallMetrics_;
    std::unordered_map<std::string, PerformanceMetrics> detailedMetrics_;
    std::unordered_map<std::string, OperationMetrics> operationMetrics_;
    bool isProfiling_;
    std::string currentOperationName_;
};

} // namespace nlm