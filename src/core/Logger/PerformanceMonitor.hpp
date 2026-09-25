#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <map>
#include <vector>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <thread>
#include <iomanip>
#include <sstream>

namespace nlm {

// Network I/O metrics
struct NetworkMetrics {
    double bytesReceived;
    double bytesSent;
    double packetsReceived;
    double packetsSent;
    double latency; // in seconds
    double throughput; // in bytes per second
    std::chrono::system_clock::time_point timestamp;
    
    NetworkMetrics() : bytesReceived(0), bytesSent(0), packetsReceived(0), packetsSent(0), 
                      latency(0.0), throughput(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// CPU metrics
struct CPUMetrics {
    double usagePercent;
    double systemTime;
    double userTime;
    int coreCount;
    std::vector<double> coreUsage;
    std::chrono::system_clock::time_point timestamp;
    
    CPUMetrics() : usagePercent(0.0), systemTime(0.0), userTime(0.0), coreCount(0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Memory metrics
struct MemoryMetrics {
    uint64_t totalMemory;
    uint64_t usedMemory;
    uint64_t freeMemory;
    uint64_t availableMemory;
    uint64_t buffers;
    uint64_t cached;
    double usagePercent;
    std::chrono::system_clock::time_point timestamp;
    
    MemoryMetrics() : totalMemory(0), usedMemory(0), freeMemory(0), availableMemory(0), 
                      buffers(0), cached(0), usagePercent(0.0) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Baseline metrics for comparison
struct PerformanceBaseline {
    std::string operation;
    double averageDuration;
    double medianDuration;\n    double minDuration;
    double maxDuration;
    double stdDevDuration;
    double p95Duration;
    double p99Duration;
    uint64_t totalExecutions;
    uint64_t failedExecutions;
    double failureRate;
    double averageMemoryUsage;
    double averageCPUUsage;
    std::chrono::system_clock::time_point lastUpdated;
    
    PerformanceBaseline() : averageDuration(0.0), medianDuration(0.0), minDuration(0.0), 
                          maxDuration(0.0), stdDevDuration(0.0), p95Duration(0.0), p99Duration(0.0),
                          totalExecutions(0), failedExecutions(0), failureRate(0.0), 
                          averageMemoryUsage(0.0), averageCPUUsage(0.0) {
        lastUpdated = std::chrono::system_clock::now();
    }
};

// Alert thresholds
struct AlertThreshold {
    std::string name;
    std::string metric;
    LogLevel level;
    double threshold;
    std::string message;
    bool enabled;
    
    AlertThreshold() : level(LogLevel::Warning), threshold(0.0), enabled(true) {}
    AlertThreshold(const std::string& n, const std::string& m, LogLevel l, 
                   double t, const std::string& msg, bool e = true)
        : name(n), metric(m), level(l), threshold(t), message(msg), enabled(e) {}
};

// Performance data point
struct PerformanceDataPoint {
    std::string operation;
    std::chrono::system_clock::time_point timestamp;
    double duration; // in seconds
    double cpuTime; // in seconds
    uint64_t memoryUsed; // in bytes
    uint64_t peakMemoryUsed; // in bytes
    double memoryPercent;
    std::map<std::string, double> customMetrics;
    bool success;
    std::string errorMessage;
    std::map<std::string, std::string> tags;
    
    PerformanceDataPoint() : duration(0.0), cpuTime(0.0), memoryUsed(0), peakMemoryUsed(0), 
                            memoryPercent(0.0), success(true) {
        timestamp = std::chrono::system_clock::now();
    }
};

// Main PerformanceMonitor class
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying, enable moving
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    PerformanceMonitor(PerformanceMonitor&&) noexcept;
    PerformanceMonitor& operator=(PerformanceMonitor&&) noexcept;
    
    // Start/stop profiling
    void startProfiling(const std::string& operation, const std::map<std::string, std::string>& tags = {});
    void stopProfiling(const std::string& operation);
    void pauseProfiling(const std::string& operation);
    void resumeProfiling(const std::string& operation);
    
    // Record custom metrics
    void recordMetric(const std::string& name, double value, const std::map<std::string, std::string>& tags = {});
    void recordTiming(const std::string& operation, double duration, const std::map<std::string, std::string>& tags = {});
    void recordMemory(uint64_t memoryUsed, uint64_t peakMemoryUsed);
    
    // Get performance data
    PerformanceDataPoint getCurrentMeasurement(const std::string& operation) const;
    std::vector<PerformanceDataPoint> getMeasurements(const std::string& operation, 
                                                      const std::chrono::system_clock::time_point& since) const;
    std::vector<PerformanceDataPoint> getAllMeasurements(const std::chrono::system_clock::time_point& since) const;
    
    // Baseline management
    void updateBaseline(const std::string& operation);
    PerformanceBaseline getBaseline(const std::string& operation) const;
    void resetBaseline(const std::string& operation);
    void setBaseline(const std::string& operation, const PerformanceBaseline& baseline);
    
    // Alert management
    void addAlert(const AlertThreshold& alert);
    void removeAlert(const std::string& name);
    void clearAlerts();
    std::vector<AlertThreshold> getAlerts() const;
    
    // System monitoring
    void updateSystemMetrics();
    CPUMetrics getCPUMetrics() const;
    MemoryMetrics getMemoryMetrics() const;
    NetworkMetrics getNetworkMetrics() const;
    
    // Configuration
    void setSamplingInterval(std::chrono::milliseconds interval);
    void setCollectionInterval(std::chrono::milliseconds interval);
    void enableSystemMonitoring(bool enable);
    void enableNetworkMonitoring(bool enable);
    
    // State information
    bool isProfiling(const std::string& operation) const;
    size_t getActiveProfileCount() const;
    std::chrono::system_clock::time_point getLastUpdateTime() const;
    
    // Static instance management
    static PerformanceMonitor& getInstance();
    static void setGlobal(std::shared_ptr<PerformanceMonitor> monitor);
    
    // Integration helpers
    void recordBrainMetrics(const class Brain& brain);
    void recordSimulationMetrics(uint64_t step, uint64_t spikeCount, float firingRate);
    void recordMemoryPoolMetrics(const std::string& poolName, size_t currentSize,
                                size_t maxSize, size_t totalAllocated);
    void recordEventMetrics(size_t queueDepth, double avgProcessingTime);
    void recordNeuromodulatorMetrics(const std::string& type, double level, double effect);
    
    // Alert checking
    void checkAlerts();
    std::vector<std::string> getActiveAlerts() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void collectMetrics();
    void updateSystemInfo();
    bool shouldAlert(const PerformanceDataPoint& dataPoint, const AlertThreshold& alert) const;
};

// Network I/O monitoring helper
class NetworkMonitor {
public:
    NetworkMonitor();
    ~NetworkMonitor();
    
    void recordReceived(size_t bytes, double latency = 0.0);
    void recordSent(size_t bytes, double latency = 0.0);
    NetworkMetrics getMetrics() const;
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// CPU monitoring helper
class CPUMonitor {
public:
    CPUMonitor();
    ~CPUMonitor();
    
    void updateMetrics();
    CPUMetrics getMetrics() const;
    double getCurrentUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory monitoring helper
class MemoryMonitor {
public:
    MemoryMonitor();
    ~MemoryMonitor();
    
    void updateMetrics();
    MemoryMetrics getMetrics() const;
    uint64_t getUsedMemory() const;
    double getUsagePercent() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Profiling session for detailed timing
class ProfilingSession {
public:
    ProfilingSession(const std::string& operation, const std::map<std::string, std::string>& tags = {});
    ~ProfilingSession();
    
    void pause();
    void resume();
    void setCustomMetric(const std::string& name, double value);
    double getDuration() const;
    const PerformanceDataPoint& getDataPoint() const;
    
    operator PerformanceDataPoint() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Utility functions for performance tracking
namespace Performance {
    // RAII helper for profiling
    inline ProfilingSession profile(const std::string& operation, 
                                   const std::map<std::string, std::string>& tags = {}) {
        return ProfilingSession(operation, tags);
    }
    
    // Simple timing
    inline double measureTime(std::function<void()> func) {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start).count();
    }
    
    // Memory usage
    inline uint64_t getCurrentMemoryUsage() {
        // Platform-specific implementation would go here
        return 0;
    }
    
    // CPU time
    inline double getCurrentCPUTime() {
        // Platform-specific implementation would go here
        return 0.0;
    }
}

} // namespace nlm