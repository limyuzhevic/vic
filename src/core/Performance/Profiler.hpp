#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>

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

// Performance profiling interface
class IProfiler {
public:
    virtual ~IProfiler() = default;
    
    // Start/stop profiling
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    
    // Component timing
    virtual void startComponent(const std::string& componentName) = 0;
    virtual void endComponent(const std::string& componentName) = 0;
    virtual std::unordered_map<std::string, ComponentTiming> getComponentTimings() const = 0;
    
    // Memory tracking
    virtual void recordMemoryAllocation(const std::string& component, size_t size) = 0;
    virtual void recordMemoryDeallocation(const std::string& component, size_t size) = 0;
    virtual std::unordered_map<std::string, MemoryAllocation> getMemoryAllocations() const = 0;
    
    // Configuration
    virtual void setSamplingInterval(size_t intervalMs) = 0;
    virtual size_t getSamplingInterval() const = 0;
    
    // Report generation
    virtual std::string generateHtmlReport() const = 0;
    virtual std::string generateJsonReport() const = 0;
    virtual std::string generateTextReport() const = 0;
};

// Global profiler interface
class ISystemProfiler {
public:
    virtual ~ISystemProfiler() = default;
    
    // Start/stop system profiling
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    
    // System metrics
    virtual double getCpuUsage() const = 0;
    virtual uint64_t getTotalMemory() const = 0;
    virtual uint64_t getUsedMemory() const = 0;
    virtual double getDiskUsage() const = 0;
    virtual double getNetworkUsage() const = 0;
    virtual double getCpuTemperature() const = 0;
    
    // Component profiling
    virtual void startComponent(const std::string& componentName) = 0;
    virtual void endComponent(const std::string& componentName) = 0;
    
    // Memory tracking
    virtual void recordMemoryAllocation(const std::string& component, size_t size) = 0;
    virtual void recordMemoryDeallocation(const std::string& component, size_t size) = 0;
    
    // Report generation
    virtual std::string generateHtmlReport() const = 0;
    virtual std::string generateJsonReport() const = 0;
    virtual std::string generateTextReport() const = 0;
};

// Global profiler functions
void initializeProfiler(const std::string& name = "DefaultProfiler");
IProfiler& getProfiler(const std::string& name = "DefaultProfiler");
void shutdownProfiler(const std::string& name = "DefaultProfiler");

// System profiler functions
void initializeSystemProfiler();
ISystemProfiler& getSystemProfiler();
void shutdownSystemProfiler();

// Convenience functions for common operations
void startComponentProfiling(const std::string& componentName);
void endComponentProfiling(const std::string& componentName);
void recordMemoryAllocation(const std::string& component, size_t size);
void recordMemoryDeallocation(const std::string& component, size_t size);
std::string generateProfilerReport();

} // namespace nlm