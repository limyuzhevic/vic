#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <thread>

namespace nlm {

class PerformanceMonitor {
public:
    struct TimerData {
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::duration<double> elapsedTime;
        bool isRunning;
    };

    struct MemoryData {
        size_t allocationSize;
        size_t currentMemory;
        size_t peakMemory;
        std::chrono::duration<double> sampleTime;
    };

    struct CPUMeasurement {
        double utilization;
        std::chrono::duration<double> duration;
        std::chrono::high_resolution_clock::time_point startTime;
        bool isMeasuring;
    };

    PerformanceMonitor();
    virtual ~PerformanceMonitor();
    
    // Timing profiling
    void startTimer(const std::string& name);
    void stopTimer(const std::string& name);
    double getElapsedTime(const std::string& name) const;
    void resetTimer(const std::string& name);
    bool isTimerRunning(const std::string& name) const;
    
    // Memory profiling
    void trackMemoryAllocation(size_t size);
    void trackMemoryDeallocation(size_t size);
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    void resetMemoryTracking();
    
    // CPU profiling
    void startCPUMeasurement();
    void stopCPUMeasurement();
    double getCPUUtilization() const;
    
    // I/O profiling
    void trackRead(size_t bytes);
    void trackWrite(size_t bytes);
    size_t getTotalBytesRead() const;
    size_t getTotalBytesWritten() const;
    
    // Event profiling
    void recordEvent(const std::string& eventName);
    size_t getEventCount(const std::string& eventName) const;
    
    // Performance analysis
    void generatePerformanceReport(const std::string& outputFile) const;
    void generateBenchmarkReport(const std::string& outputFile) const;
    void optimizePerformance();
    
    // System resource monitoring
    double getSystemCPUUsage() const;
    size_t getSystemMemoryUsage() const;
    size_t getSystemMemoryPeak() const;
    double getSystemDiskUsage() const;
    
    // Statistics and analysis
    std::string getPerformanceStatistics() const;
    std::vector<std::string> getSlowTimers(double thresholdSeconds) const;
    std::vector<std::string> getMemoryLeaks() const;
    
    // Configuration
    void setSamplingInterval(double interval);
    void enableRealTimeMonitoring(bool enable);
    void setProfilingLevel(int level);
    
    // Thread safety
    void setThreadSafe(bool enable);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
