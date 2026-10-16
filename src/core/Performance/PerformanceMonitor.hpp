#pragma once

#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <functional>
#include <iomanip>
#include <sstream>

namespace nlm {

// Performance measurement units
enum class TimeUnit {
    Nanoseconds,
    Microseconds,
    Milliseconds,
    Seconds
};

enum class MemoryUnit {
    Bytes,
    Kilobytes,
    Megabytes,
    Gigabytes
};

enum class PerformanceLevel {
    Excellent,
    Good,
    Fair,
    Poor,
    Critical
};

// Performance metrics structure
struct PerformanceMetrics {
    double cpuUsage;           // CPU usage percentage (0-100)
    double memoryUsage;        // Memory usage percentage (0-100)
    uint64_t totalMemory;      // Total system memory in bytes
    uint64_t usedMemory;       // Used memory in bytes
    uint64_t availableMemory;  // Available memory in bytes
    double diskUsage;          // Disk usage percentage (0-100)
    double networkUsage;       // Network usage in Mbps
    double temperature;        // CPU temperature in Celsius
    uint64_t processMemory;    // Current process memory usage in bytes
    
    PerformanceMetrics()
        : cpuUsage(0.0), memoryUsage(0.0), totalMemory(0),
          usedMemory(0), availableMemory(0), diskUsage(0.0),
          networkUsage(0.0), temperature(0.0), processMemory(0) {}
};

// Component timing metrics
struct ComponentTiming {
    std::string name;
    std::chrono::nanoseconds totalDuration;
    std::chrono::nanoseconds minDuration;
    std::chrono::nanoseconds maxDuration;
    size_t callCount;
    double averageDuration;    // In milliseconds
    double p95Duration;        // 95th percentile
    double p99Duration;        // 99th percentile
    std::chrono::nanoseconds lastDuration;
    
    ComponentTiming(const std::string& n) 
        : name(n), totalDuration(0), minDuration(std::chrono::nanoseconds::max()),
          maxDuration(0), callCount(0), averageDuration(0.0), p95Duration(0.0),
          p99Duration(0.0) {}
};

// Memory allocation tracking
struct MemoryAllocation {
    std::string component;
    size_t allocationSize;     // Bytes allocated
    size_t deallocationSize;   // Bytes deallocated
    size_t netAllocation;      // Net allocation (bytes)
    size_t maxAllocated;      // Peak allocation in bytes
    size_t allocationCount;    // Number of allocations
    size_t deallocationCount;  // Number of deallocations
    double timeInSystem;       // Time spent in system (ms)
    
    MemoryAllocation() : allocationSize(0), deallocationSize(0),
                        netAllocation(0), maxAllocated(0),
                        allocationCount(0), deallocationCount(0),
                        timeInSystem(0.0) {}
};

// Performance alert structure
struct PerformanceAlert {
    std::string component;
    std::string metric;
    double value;
    double threshold;
    std::chrono::system_clock::time_point timestamp;
    PerformanceLevel severity;
    std::string message;
    
    PerformanceAlert(const std::string& comp, const std::string& met,
                     double val, double thresh, PerformanceLevel sev)
        : component(comp), metric(met), value(val), threshold(thresh),
          severity(sev), timestamp(std::chrono::system_clock::now()) {
        switch (severity) {
            case PerformanceLevel::Critical:
                message = "CRITICAL: " + metric + " exceeded threshold: " + std::to_string(value) + " > " + std::to_string(threshold);
                break;
            case PerformanceLevel::Poor:
                message = "WARNING: " + metric + " approaching threshold: " + std::to_string(value) + " > " + std::to_string(threshold);
                break;
            case PerformanceLevel::Fair:
                message = "INFO: " + metric + " at " + std::to_string(value) + " (near threshold " + std::to_string(threshold) + ")";
                break;
            case PerformanceLevel::Good:
                message = "INFO: " + metric + " at " + std::to_string(value) + "/" + std::to_string(threshold);
                break;
            case PerformanceLevel::Excellent:
                message = "INFO: " + metric + " performing well at " + std::to_string(value);
                break;
        }
    }
};

// Performance checkpoint
struct PerformanceCheckpoint {
    std::chrono::system_clock::time_point timestamp;
    std::string checkpointName;
    std::string description;
    std::vector<PerformanceMetrics> metrics;
    std::vector<ComponentTiming> timings;
    std::vector<MemoryAllocation> memoryAllocations;
    
    PerformanceCheckpoint(const std::string& name, const std::string& desc)
        : timestamp(std::chrono::system_clock::now()), checkpointName(name), description(desc) {}
};

// Performance profiling configuration
struct PerformanceConfig {
    bool enableRealTimeMonitoring;      // Enable real-time performance monitoring
    bool enableComponentProfiling;      // Enable per-component profiling
    bool enableMemoryTracking;          // Enable memory allocation tracking
    bool enableSystemMonitoring;        // Enable system resource monitoring
    bool enablePerformanceAlerts;       // Enable performance alerting
    double cpuThresholdWarning;         // CPU usage threshold for warning (0-100)
    double cpuThresholdCritical;        // CPU usage threshold for critical (0-100)
    double memoryThresholdWarning;      // Memory usage threshold for warning (0-100)
    double memoryThresholdCritical;     // Memory usage threshold for critical (0-100)
    uint64_t memoryAllocationLimit;     // Memory allocation limit in bytes
    size_t maxComponentProfiles;       // Maximum number of component profiles to retain
    size_t maxCheckpoints;              // Maximum number of performance checkpoints
    size_t samplingIntervalMs;          // Sampling interval for system metrics (milliseconds)
    
    PerformanceConfig()
        : enableRealTimeMonitoring(true), enableComponentProfiling(true),
          enableMemoryTracking(true), enableSystemMonitoring(true),
          enablePerformanceAlerts(true), cpuThresholdWarning(80.0),
          cpuThresholdCritical(90.0), memoryThresholdWarning(85.0),
          memoryThresholdCritical(95.0), memoryAllocationLimit(1024 * 1024 * 1024),  // 1GB
          maxComponentProfiles(1000), maxCheckpoints(100), samplingIntervalMs(1000) {}
};

// Performance monitoring interface
class IPerformanceMonitor {
public:
    virtual ~IPerformanceMonitor() = default;
    
    // Start/stop monitoring
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    
    // Core monitoring functionality
    virtual void recordComponentStart(const std::string& componentName) = 0;
    virtual void recordComponentEnd(const std::string& componentName) = 0;
    virtual void recordMemoryAllocation(const std::string& component, size_t size) = 0;
    virtual void recordMemoryDeallocation(const std::string& component, size_t size) = 0;
    virtual PerformanceMetrics getSystemMetrics() = 0;
    
    // Alerting
    virtual void addAlertThreshold(const std::string& component, const std::string& metric,
                                  double threshold, PerformanceLevel severity) = 0;
    virtual std::vector<PerformanceAlert> getAlerts() const = 0;
    
    // Checkpoints
    virtual void createCheckpoint(const std::string& name, const std::string& description) = 0;
    virtual std::vector<PerformanceCheckpoint> getCheckpoints() const = 0;
    virtual PerformanceCheckpoint getLatestCheckpoint() const = 0;
    
    // Performance optimization suggestions
    virtual std::vector<std::string> getOptimizationSuggestions() const = 0;
    
    // Output formats
    virtual std::string generateReport() const = 0;
    virtual std::string generateJsonReport() const = 0;
    virtual std::string generateHtmlReport() const = 0;
    virtual std::string generateTextReport() const = 0;
};