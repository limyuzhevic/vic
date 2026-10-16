#pragma once

#include "MemoryProfiler.hpp"
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace nlm {

// Memory allocation tracking structure
struct AllocationTracker {
    std::string component;
    std::string file;
    int line;
    size_t size;
    std::chrono::system_clock::time_point allocationTime;
    std::chrono::system_clock::time_point deallocationTime;
    bool isAllocated;
    
    AllocationTracker(const std::string& comp, const std::string& f, int l, size_t s)
        : component(comp), file(f), line(l), size(s), allocationTime(std::chrono::system_clock::now()),
          deallocationTime(), isAllocated(true) {}
};

// Memory profiling configuration
struct MemoryProfilingConfig {
    bool enableAllocationTracking;      // Enable detailed allocation tracking
    bool enableMemoryLeakDetection;      // Enable memory leak detection
    bool enableAllocationProfiling;      // Enable allocation profiling
    bool enableTimelineTracking;         // Enable memory timeline tracking
    size_t maxTrackedAllocations;        // Maximum number of tracked allocations
    size_t memoryLeakThreshold;          // Memory leak threshold in bytes
    bool trackAllocationStackTrace;      // Track allocation stack traces
    bool enableRealTimeMonitoring;        // Enable real-time memory monitoring
    size_t samplingIntervalMs;            // Sampling interval for monitoring
    
    MemoryProfilingConfig()
        : enableAllocationTracking(true), enableMemoryLeakDetection(true),
          enableAllocationProfiling(true), enableTimelineTracking(true),
          maxTrackedAllocations(10000), memoryLeakThreshold(1024 * 1024),  // 1MB
          trackAllocationStackTrace(false), enableRealTimeMonitoring(true),
          samplingIntervalMs(1000) {}
};

// Memory profile
struct MemoryProfile {
    std::chrono::system_clock::time_point timestamp;
    std::string profileName;
    std::string description;
    size_t totalAllocations;
    size_t totalDeallocations;
    size_t netAllocation;
    size_t currentMemoryUsage;
    size_t peakMemoryUsage;
    std::vector<AllocationTracker> allocations;
    std::unordered_map<std::string, size_t> allocationsByComponent;
    std::unordered_map<std::string, size_t> deallocationsByComponent;
    
    MemoryProfile(const std::string& name, const std::string& desc)
        : timestamp(std::chrono::system_clock::now()), profileName(name), description(desc),
          totalAllocations(0), totalDeallocations(0), netAllocation(0),
          currentMemoryUsage(0), peakMemoryUsage(0) {}
};

// Memory leak information
struct MemoryLeakInfo {
    std::string component;
    std::string file;
    int line;
    size_t leakedSize;
    std::chrono::system_clock::time_point detectedTime;
    std::string stackTrace;
    
    MemoryLeakInfo(const std::string& comp, const std::string& f, int l, size_t s)
        : component(comp), file(f), line(l), leakedSize(s),
          detectedTime(std::chrono::system_clock::now()) {}
};

// Memory profiler implementation
class MemoryProfiler : public IMemoryProfiler {
private:
    MemoryProfilingConfig config_;
    std::atomic<bool> isRunning_{false};
    std::thread monitoringThread_;
    
    // Allocation tracking
    std::vector<AllocationTracker> activeAllocations_;
    std::vector<AllocationTracker> completedAllocations_;
    mutable std::mutex allocationMutex_;
    
    // Current memory state
    size_t currentMemoryUsage_ = 0;
    size_t peakMemoryUsage_ = 0;
    mutable std::mutex memoryMutex_;
    
    // Memory profiles and history
    std::vector<MemoryProfile> profiles_;
    std::vector<MemoryLeakInfo> detectedLeaks_;
    mutable std::mutex profileMutex_;
    
    // Monitoring state
    std::chrono::system_clock::time_point lastMonitoringTime_;
    size_t monitoringCount_ = 0;
    
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // IMemoryProfiler implementation
    void start() override;
    void stop() override;
    bool isRunning() const override;
    
    void startAllocationTracking(const std::string& component, const std::string& file, int line) override;
    void endAllocationTracking(const std::string& component, const std::string& file, int line) override;
    void recordAllocation(const std::string& component, size_t size, const std::string& file, int line) override;
    void recordDeallocation(const std::string& component, size_t size, const std::string& file, int line) override;
    MemoryProfile createProfile(const std::string& name, const std::string& description) override;
    
    std::vector<MemoryProfile> getProfiles() const override;
    MemoryProfile getLatestProfile() const override;
    
    std::vector<MemoryLeakInfo> getDetectedLeaks() const override;
    
    std::string generateHtmlReport() const override;
    std::string generateJsonReport() const override;
    std::string generateTextReport() const override;
    
    // Configuration management
    void setConfig(const MemoryProfilingConfig& config);
    MemoryProfilingConfig getConfig() const;
    
    // Memory analysis
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    void updateMemoryUsage();
    
    // Leak detection
    void detectMemoryLeaks();
    void analyzeLeaks(const std::vector<AllocationTracker>& allocations);
    
    // Report generation helpers
    std::string getCurrentTimestamp() const;
    std::string generateAllocationSummary(const MemoryProfile& profile) const;
    std::string generateLeakSummary() const;
};

// Memory allocator wrapper for tracking allocations
void* memoryAllocWrapper(size_t size, const std::string& component, const std::string& file, int line);
void memoryDeallocWrapper(void* ptr, const std::string& component, const std::string& file, int line);

// Global memory profiler instance
std::unique_ptr<IMemoryProfiler> gMemoryProfiler;

// Initialize global memory profiler
void initializeMemoryProfiler(const MemoryProfilingConfig& config = MemoryProfilingConfig()) {
    gMemoryProfiler = std::make_unique<MemoryProfiler>();
    gMemoryProfiler->setConfig(config);
    gMemoryProfiler->start();
}

void shutdownMemoryProfiler() {
    if (gMemoryProfiler) {
        gMemoryProfiler->stop();
        gMemoryProfiler.reset();
    }
}

// Global allocation functions for use in code
void* trackAllocation(size_t size, const std::string& component = "Unknown", const std::string& file = "", int line = 0);
void trackDeallocation(void* ptr, const std::string& component = "Unknown", const std::string& file = "", int line = 0);

// Implementation of global functions
void* trackAllocation(size_t size, const std::string& component, const std::string& file, int line) {
    if (gMemoryProfiler) {
        gMemoryProfiler->recordAllocation(component, size, file, line);
    }
    return memoryAllocWrapper(size, component, file, line);
}

void trackDeallocation(void* ptr, const std::string& component, const std::string& file, int line) {
    if (gMemoryProfiler) {
        gMemoryProfiler->recordDeallocation(component, 0, file, line);
    }
    memoryDeallocWrapper(ptr, component, file, line);
}

// Macro for easy allocation tracking
#define TRACK_ALLOCATION(size) trackAllocation(size, __func__, __FILE__, __LINE__)
#define TRACK_DEALLOCATION(ptr) trackDeallocation(ptr, __func__, __FILE__, __LINE__)