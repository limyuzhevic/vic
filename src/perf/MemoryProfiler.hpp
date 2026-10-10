#pragma once

#include "PerformanceProfiler.hpp"
#include <chrono>
#include <vector>
#include <unordered_map>
#include <string>
#include <mutex>
#include <atomic>
#include <algorithm>

namespace nlm {

class MemoryProfiler : public PerformanceProfiler {
private:
    struct MemorySnapshot {
        std::chrono::high_resolution_clock::time_point timestamp;
        size_t totalMemory;
        size_t usedMemory;
        size_t peakMemory;
        size_t allocatedCount;
        size_t deallocatedCount;
        std::unordered_map<std::string, size_t> allocationMap;
    };
    
    std::vector<MemorySnapshot> memorySnapshots;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point stopTime;
    bool profilingActive;
    double samplingInterval;
    std::mutex dataMutex;
    std::atomic<bool> stopFlag;
    std::thread samplingThread;
    
    // Memory allocation tracking
    struct AllocationInfo {
        size_t size;
        std::string allocationSite;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string callStack;
    };
    
    std::vector<AllocationInfo> allocations;
    std::vector<AllocationInfo> deallocations;
    
    // Memory leak detection
    size_t calculateCurrentMemoryFootprint() const;
    size_t calculatePeakMemoryUsage() const;
    void collectMemoryData();
    void samplingFunction();
    std::string getCallStack() const;
    
public:
    MemoryProfiler();
    ~MemoryProfiler() override;
    
    // Profiling control
    void startProfiling() override;
    void stopProfiling() override;
    bool isProfiling() const override;
    
    // Performance metrics collection
    double getCpuUsage() const override;
    size_t getMemoryUsage() const override;
    double getThreadUsage() const override;
    double getIoUsage() const override;
    
    // Profiling data
    PerformanceMetrics getCurrentMetrics() const override;
    std::vector<PerformanceMetrics> getHistoricalData() const override;
    
    // Configuration
    void setSamplingInterval(double interval) override;
    double getSamplingInterval() const override;
    
    // Memory-specific profiling
    void startCpuProfiling() override;
    void stopCpuProfiling() override;
    void startMemoryProfiling() override;
    void stopMemoryProfiling() override;
    void startThreadProfiling() override;
    void stopThreadProfiling() override;
    void startIoProfiling() override;
    void stopIoProfiling() override;
    
    // Memory-specific methods
    size_t getTotalMemoryUsage() const;
    size_t getUsedMemory() const;
    size_t getPeakMemoryUsage() const;
    size_t getAllocationCount() const;
    double getMemoryGrowthRate() const;
    bool detectMemoryLeak() const;
    std::vector<AllocationInfo> getAllocations() const;
    std::vector<AllocationInfo> getDeallocations() const;
    size_t calculateMemoryFragmentation() const;
    
    // Memory tracking
    void trackAllocation(size_t size, const std::string& site);
    void trackDeallocation(size_t size, const std::string& site);
    void recordAllocation(const std::string& site, size_t size);
    void recordDeallocation(const std::string& site, size_t size);
    
    // Memory efficiency analysis
    double calculateMemoryEfficiency() const;
    size_t getInternalFragmentation() const;
    size_t getExternalFragmentation() const;
    std::unordered_map<std::string, size_t> getAllocationStats() const;
};

} // namespace nlm