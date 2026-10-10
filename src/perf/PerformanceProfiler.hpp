#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <thread>
#include <optional>

namespace nlm {

// Performance metrics structure
struct PerformanceMetrics {
    // Time measurements
    double cpuTime;
    double wallClockTime;
    double userTime;
    double systemTime;
    
    // Memory measurements
    size_t memoryUsage;
    size_t peakMemoryUsage;
    size_t allocatedMemory;
    size_t deallocatedMemory;
    
    // Thread measurements
    size_t activeThreads;
    size_t totalThreads;
    double threadUtilization;
    
    // I/O measurements
    size_t readOperations;
    size_t writeOperations;
    size_t readBytes;
    size_t writeBytes;
    
    // Simulation measurements
    size_t simulationSteps;
    size_t simulationTime;
    double stepsPerSecond;
    double memoryPerStep;
    
    // Timestamp
    std::chrono::high_resolution_clock::time_point timestamp;
};

// Performance profiling framework
class PerformanceProfiler {
public:
    virtual ~PerformanceProfiler() = default;
    
    // Profiling control
    virtual void startProfiling() = 0;
    virtual void stopProfiling() = 0;
    virtual bool isProfiling() const = 0;
    
    // Performance metrics collection
    virtual double getCpuUsage() const = 0;
    virtual size_t getMemoryUsage() const = 0;
    virtual double getThreadUsage() const = 0;
    virtual double getIoUsage() const = 0;
    
    // Profiling data
    virtual PerformanceMetrics getCurrentMetrics() const = 0;
    virtual std::vector<PerformanceMetrics> getHistoricalData() const = 0;
    
    // Configuration
    virtual void setSamplingInterval(double interval) = 0;
    virtual double getSamplingInterval() const = 0;
    
    // Start/stop specific profiling categories
    virtual void startCpuProfiling() = 0;
    virtual void stopCpuProfiling() = 0;
    virtual void startMemoryProfiling() = 0;
    virtual void stopMemoryProfiling() = 0;
    virtual void startThreadProfiling() = 0;
    virtual void stopThreadProfiling() = 0;
    virtual void startIoProfiling() = 0;
    virtual void stopIoProfiling() = 0;
};

} // namespace nlm