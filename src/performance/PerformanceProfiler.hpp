#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <iomanip>

namespace nlm {

class PerformanceProfiler {
public:
    struct PerformanceMetrics {
        double cpuTime;           // CPU time in seconds
        size_t memoryUsage;       // Memory usage in bytes
        size_t peakMemoryUsage;   // Peak memory usage in bytes
        std::chrono::microseconds totalTime;  // Total time for operation
        size_t operationsCount;   // Number of operations performed
        float operationsPerSecond; // Operations per second
        
        PerformanceMetrics() : cpuTime(0.0), memoryUsage(0), peakMemoryUsage(0), 
                              totalTime(0), operationsCount(0), operationsPerSecond(0.0) {}
    };
    
    struct TimelineEvent {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string operation;
        std::chrono::microseconds duration;
        std::string details;
        
        TimelineEvent(const std::string& op, const std::chrono::microseconds& dur, const std::string& det = "")
            : operation(op), duration(dur), details(det) {
            timestamp = std::chrono::high_resolution_clock::now();
        }
    };
    
    class Timer {
    public:
        Timer(PerformanceProfiler& profiler, const std::string& operation, const std::string& details = "")
            : profiler(profiler), operation(operation), details(details) {
            startTime = std::chrono::high_resolution_clock::now();
        }
        
        ~Timer() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            profiler.recordEvent(operation, duration, details);
        }
        
        void stop() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            profiler.recordEvent(operation, duration, details);
        }
        
        void updateDetails(const std::string& newDetails) {
            details = newDetails;
        }
        
    private:
        PerformanceProfiler& profiler;
        std::string operation;
        std::chrono::high_resolution_clock::time_point startTime;
        std::string details;
    };
    
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Disable copying, enable moving
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    PerformanceProfiler(PerformanceProfiler&&) noexcept;
    PerformanceProfiler& operator=(PerformanceProfiler&&) noexcept;
    
    // Core profiling methods
    Timer createTimer(const std::string& operation, const std::string& details = "");
    
    void recordEvent(const std::string& operation, const std::chrono::microseconds& duration, 
                    const std::string& details = "");
    
    void updateMemoryUsage(size_t memoryUsage, size_t peakMemoryUsage = 0);
    
    void incrementOperations(const std::string& operation, size_t count = 1);
    
    // Performance queries
    PerformanceMetrics getMetrics(const std::string& operation) const;
    
    std::vector<std::string> getAllOperations() const;
    
    double getTotalTime() const;
    size_t getTotalMemoryUsage() const;
    size_t getTotalOperations() const;
    
    // Reporting and analysis
    std::string generateReport() const;
    std::string generateDetailedReport() const;
    
    std::string getOperationSummary(const std::string& operation) const;
    
    void clear();
    
    // Performance thresholds for validation
    void setPerformanceThreshold(const std::string& operation, double maxCpuTime, 
                                size_t maxMemoryUsage, double maxOperationsPerSecond = 0.0);
    
    bool validatePerformance(const std::string& operation) const;
    std::string getPerformanceError(const std::string& operation) const;
    
    // Timeline and event tracking
    void addTimelineEvent(const TimelineEvent& event);
    std::vector<TimelineEvent> getTimeline() const;
    
    // Simulation-specific profiling
    void recordStepMetrics(const size_t step, const size_t spikeCount, 
                          const std::chrono::microseconds& stepTime);
    
    std::string getSimulationProfile(size_t maxSteps = 0) const;
    
    // Statistical analysis
    double getAverageTime(const std::string& operation) const;
    double getMedianTime(const std::string& operation) const;
    size_t getMinTime(const std::string& operation) const;
    size_t getMaxTime(const std::string& operation) const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    size_t getEventCount(const std::string& operation) const;
};

} // namespace nlm