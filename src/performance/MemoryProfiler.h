// MemoryProfiler.h
// Memory tracking and leak detection for NLM brain system

#ifndef NLM_MEMORY_PROFILER_H
#define NLM_MEMORY_PROFILER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <mutex>
#include <atomic>

// Forward declarations
namespace nlm {
    class Brain;
    class Neuron;
    class Synapse;
}

// Memory allocation tracking
struct AllocationRecord {
    void* address;
    size_t size;
    std::string typeName;
    std::string sourceFile;
    int sourceLine;
    std::string stackTrace;
    std::chrono::high_resolution_clock::time_point allocationTime;
    std::chrono::high_resolution_clock::time_point deallocationTime;
    bool isLeaked;
    
    AllocationRecord(void* addr, size_t sz, const std::string& type, 
                     const std::string& file = "", int line = 0)
        : address(addr), size(sz), typeName(type), sourceFile(file), 
          sourceLine(line), allocationTime(std::chrono::high_resolution_clock::now()),
          deallocationTime(), isLeaked(false) {}
};

// Memory usage statistics
struct MemoryStatistics {
    size_t totalAllocations;
    size_t totalDeallocations;
    size_t netAllocations;
    size_t peakMemoryUsage;
    size_t currentMemoryUsage;
    double allocationRate;
    double deallocationRate;
    size_t largestAllocation;
    size_t smallestAllocation;
    
    MemoryStatistics() : totalAllocations(0), totalDeallocations(0), netAllocations(0),
                        peakMemoryUsage(0), currentMemoryUsage(0), allocationRate(0.0),
                        deallocationRate(0.0), largestAllocation(0), smallestAllocation(0) {}
};

// Memory profiler configuration
struct MemoryProfilerConfig {
    bool enableAllocationTracking = true;        // Track memory allocations
    bool enableDeallocationTracking = true;      // Track memory deallocations
    bool enableLeakDetection = true;             // Detect memory leaks
    bool enableRealTimeMonitoring = true;        // Monitor memory usage in real-time
    bool enableHistogram = true;                // Track allocation size distribution
    bool enableStackTraces = true;              // Capture stack traces for allocations
    bool enableMemoryPools = false;             // Track custom memory pools
    size_t maxTrackedAllocations = 10000;       // Maximum allocations to track
    size_t maxMemorySamples = 1000;            // Maximum memory samples for real-time monitoring
    double leakThreshold = 0.01;                // Leak threshold (1% of allocations)
    bool trackSystemAllocations = false;       // Track system allocations (malloc/free)
    
    MemoryProfilerConfig() {}
};

// Memory profiler class
class MemoryProfiler {
private:
    MemoryProfilerConfig config;
    
    // Thread safety
    mutable std::mutex dataMutex;
    
    // Tracking data
    std::vector<AllocationRecord> allocations;
    std::vector<AllocationRecord> deallocations;
    std::vector<MemoryStatistics> memoryStats;
    
    // Real-time monitoring
    std::vector<size_t> memoryUsageHistory;
    std::chrono::high_resolution_clock::time_point sessionStartTime;
    
    // Leak detection
    std::unordered_map<void*, AllocationRecord> allocationMap;
    
    // Helper methods
    void updateMemoryStatistics();
    void detectLeaks();
    void cleanupDeallocatedRecords();
    void captureStackTrace(AllocationRecord& record);
    size_t calculateCurrentMemoryUsage() const;
    
public:
    MemoryProfiler(const MemoryProfilerConfig& config = MemoryProfilerConfig());
    ~MemoryProfiler();
    
    // Core profiling interface
    void trackAllocation(size_t size, void* address, const std::string& typeName,
                        const std::string& sourceFile = "", int sourceLine = 0);
    void trackDeallocation(void* address);
    
    // Memory statistics
    MemoryStatistics getCurrentStatistics() const;
    MemoryStatistics getStatisticsAt(size_t index) const;
    std::vector<MemoryStatistics> getAllStatistics() const { return memoryStats; }
    
    // Leak detection
    std::vector<AllocationRecord> getLeaks() const;
    std::vector<AllocationRecord> getLeaksByType(const std::string& typeName) const;
    size_t getLeakCount() const;
    double getLeakPercentage() const;
    
    // Real-time monitoring
    void updateMemoryUsage(size_t currentUsage);
    size_t getCurrentMemoryUsage() const { return calculateCurrentMemoryUsage(); }
    size_t getPeakMemoryUsage() const;
    double getAverageMemoryUsage() const;
    
    // Brain component profiling
    void profileBrainMemoryUsage(nlm::Brain* brain);
    void profileNeuronMemory(nlm::Neuron* neuron);
    void profileSynapseMemory(nlm::Synapse* synapse);
    
    // Configuration
    void setConfig(const MemoryProfilerConfig& newConfig);
    const MemoryProfilerConfig& getConfig() const { return config; }
    
    // Analysis and reporting
    void generateMemoryReport(const std::string& filename) const;
    void printMemorySummary() const;
    void printAllocationHistogram() const;
    void printLeakReport() const;
    
    // Utility methods
    void clear();
    size_t getAllocationCount() const { return allocations.size(); }
    size_t getDeallocationCount() const { return deallocations.size(); }
    bool isLeak(void* address) const;
    
    // Singleton pattern for global access
    static MemoryProfiler& getInstance(const MemoryProfilerConfig& config = MemoryProfilerConfig());
    static void cleanupInstance();
};

// Global functions for easy memory tracking
namespace MemoryTracker {
    template<typename T, typename... Args>
    T* trackNew(const std::string& typeName, const std::string& sourceFile, 
               int sourceLine, Args&&... args) {
        T* ptr = new T(std::forward<Args>(args)...);
        MemoryProfiler::getInstance().trackAllocation(sizeof(T), ptr, typeName, sourceFile, sourceLine);
        return ptr;
    }
    
    template<typename T>
    void trackDelete(T* ptr) {
        if (ptr) {
            delete ptr;
            MemoryProfiler::getInstance().trackDeallocation(ptr);
        }
    }
    
    template<typename T, typename... Args>
    T* trackNewArray(size_t count, const std::string& typeName,
                    const std::string& sourceFile, int sourceLine) {
        T* ptr = new T[count];
        MemoryProfiler::getInstance().trackAllocation(sizeof(T) * count, ptr, typeName, sourceFile, sourceLine);
        return ptr;
    }
    
    template<typename T>
    void trackDeleteArray(T* ptr) {
        if (ptr) {
            delete[] ptr;
            MemoryProfiler::getInstance().trackDeallocation(ptr);
        }
    }
}

#endif // NLM_MEMORY_PROFILER_H