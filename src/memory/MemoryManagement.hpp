#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <atomic>
#include <mutex>
#include <stack>
#include <chrono>
#include <thread>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <set>
#include <typeinfo>

namespace nlm {

/**
 * Memory allocation tracking and leak detection system
 * 
 * Provides comprehensive tracking of memory allocations, deallocations,
 * and detection of memory leaks. Thread-safe implementation suitable
 * for long-running simulations.
 */
class MemoryTracker {
public:
    struct AllocationInfo {
        void* pointer;
        size_t size;
        std::string file;
        int line;
        std::string function;
        std::chrono::system_clock::time_point allocationTime;
        std::thread::id threadId;
        bool tracked;
        std::string allocationTag;
        
        AllocationInfo() 
            : pointer(nullptr), size(0), line(0), tracked(false), allocationTag("DEFAULT") {}
        
        AllocationInfo(void* ptr, size_t sz, const std::string& file, int line, 
                      const std::string& func, const std::string& tag)
            : pointer(ptr), size(sz), file(file), line(line), function(func),
              allocationTime(std::chrono::system_clock::now()), threadId(std::this_thread::get_id()),
              tracked(true), allocationTag(tag) {}
    };
    
    struct MemoryBlock {
        std::vector<uint8_t> data;
        size_t alignment;
        std::string tag;
        std::chrono::system_clock::time_point creationTime;
        
        MemoryBlock(size_t size, size_t align, const std::string& tag) 
            : data(size, 0), alignment(align), tag(tag), creationTime(std::chrono::system_clock::now()) {}
    };
    
    enum class AllocationType {
        Malloc,
        New,
        NewArray,
        Memalign,
        PoolAllocate,
        Custom
    };
    
    class MemoryPool;
    class MemoryRegion;
    
    static MemoryTracker& getInstance();
    
    // Allocation tracking
    void* trackAllocation(size_t size, const std::string& file, int line, 
                         const std::string& function, AllocationType type, 
                         const std::string& tag = "DEFAULT", size_t alignment = 0);
    
    void trackDeallocation(void* pointer, AllocationType type);
    
    // Memory pool management
    MemoryPool& getPool(const std::string& tag, size_t objectSize, size_t initialCapacity = 1024);
    void returnPool(const std::string& tag);
    
    // Statistics and reporting
    struct MemoryStats {
        size_t totalAllocated;
        size_t totalFreed;
        size_t currentUsage;
        size_t peakUsage;
        size_t allocationCount;
        size_t deallocationCount;
        size_t poolCount;
        double fragmentationRatio;
        std::unordered_map<std::string, size_t> allocationsByTag;
        std::unordered_map<std::string, size_t> deallocationsByTag;
        
        MemoryStats() : totalAllocated(0), totalFreed(0), currentUsage(0), peakUsage(0),
                       allocationCount(0), deallocationCount(0), poolCount(0),
                       fragmentationRatio(0.0) {}
        
        std::string report() const;
    };
    
    MemoryStats getStats() const;
    
    // Leak detection
    std::vector<AllocationInfo> getLeakedAllocations() const;
    std::vector<AllocationInfo> getLeakedAllocationsByTag(const std::string& tag) const;
    std::vector<AllocationInfo> getLeakedAllocationsByFile(const std::string& file) const;
    
    // Memory validation
    bool validateMemory() const;
    bool checkCorruption() const;
    
    // Debugging and analysis
    void printAllocationTrace(size_t maxEntries = 100) const;
    void printAllocationTraceByTag(const std::string& tag, size_t maxEntries = 100) const;
    std::string generateMemoryReport() const;
    
    // Memory cleanup
    void cleanup() noexcept;
    void reset();
    
    // Thread-local storage
    void setThreadId() {}
    
    // Global memory monitoring
    static void startMemoryMonitoring();
    static void stopMemoryMonitoring();
    static double getSystemMemoryUsage();
    static double getProcessMemoryUsage();
    
    // Allocation hooks
    using AllocationHook = std::function<void(void* ptr, size_t size, const std::string& tag)>;
    void addAllocationHook(const AllocationHook& hook);
    void removeAllocationHook(const AllocationHook& hook);
    
    // Singleton control
    static void enableTracking(bool enable = true);
    static bool isTrackingEnabled();
    
    ~MemoryTracker();
    
private:
    MemoryTracker();
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Advanced memory pool with thread-local caching
 * 
 * High-performance memory pool optimized for neural network simulations,
 * providing efficient allocation/deallocation for frequently used types
 * like neurons, synapses, and neural states.
 */
class MemoryPool {
public:
    struct PoolStats {
        size_t allocatedBytes;
        size_t allocatedObjects;
        size_t availableObjects;
        size_t totalBlocks;
        size_t maxBlocks;
        double utilizationRatio;
        std::chrono::system_clock::time_point lastAllocation;
        std::chrono::system_clock::time_point lastDeallocation;
        
        PoolStats() : allocatedBytes(0), allocatedObjects(0), availableObjects(0),
                     totalBlocks(0), maxBlocks(0), utilizationRatio(0.0) {}
    };
    
    MemoryPool(size_t objectSize, size_t initialCapacity = 1024,
              const std::string& tag = "DEFAULT",
              bool threadLocalCache = true,
              size_t cacheSize = 16);
    
    ~MemoryPool();
    
    // Disable copying
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
    
    // Allocation/deallocation
    void* allocate();
    void deallocate(void* ptr);
    
    // Bulk operations
    std::vector<void*> allocateBatch(size_t count);
    void deallocateBatch(const std::vector<void*>& ptrs);
    
    // Pool management
    void reserve(size_t capacity);
    void clear();
    
    // Statistics
    PoolStats getStats() const;
    size_t getObjectSize() const { return objectSize_; }
    const std::string& getTag() const { return tag_; }
    
    // Thread-local cache management
    void flushThreadLocalCache();
    size_t getThreadLocalCacheSize() const;
    
    // Memory efficiency
    void enableCompression(bool enable);
    void enableCoalescing(bool enable);
    
    // Debugging
    std::string generateReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Memory region management for large allocations
 * 
 * Manages large memory regions for objects that don't fit well into
 * standard memory pools, such as large neural network graphs or
 * simulation state buffers.
 */
class MemoryRegion {
public:
    struct RegionStats {
        size_t totalAllocated;
        size_t currentAllocated;
        size_t peakAllocated;
        size_t alignment;
        std::string tag;
        std::chrono::system_clock::time_point creationTime;
        bool allocated;
        
        RegionStats() : totalAllocated(0), currentAllocated(0), peakAllocated(0),
                       alignment(0), allocated(false) {}
    };
    
    MemoryRegion(size_t size, size_t alignment = 64,
                const std::string& tag = "DEFAULT");
    
    ~MemoryRegion();
    
    // Allocation within region
    void* allocate(size_t size, size_t alignment = 0);
    void deallocate(void* ptr);
    
    // Region management
    void* getBase() const { return basePointer_; }
    size_t getSize() const { return totalSize_; }
    size_t getAlignment() const { return alignment_; }
    const std::string& getTag() const { return tag_; }
    
    // Statistics
    RegionStats getStats() const;
    
    // Validation
    bool isValid() const;
    bool checkIntegrity() const;
    
    // Debugging
    std::string generateReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void* basePointer_;
    size_t totalSize_;
    size_t alignment_;
    std::string tag_;
};

/**
 * Memory allocation profiler
 * 
 * Profiles memory allocation patterns to identify hotspots and optimize
 * memory usage for neural network simulations.
 */
class AllocationProfiler {
public:
    struct AllocationSample {
        std::chrono::system_clock::time_point timestamp;
        std::string threadId;
        std::string file;
        int line;
        std::string function;
        size_t size;
        std::string tag;
        double cumulativeTime;
    };
    
    AllocationProfiler();
    ~AllocationProfiler();
    
    // Start/stop profiling
    void start();
    void stop();
    bool isRunning() const { return running_; }
    
    // Profile specific allocations
    void profileAllocation(const std::string& tag, size_t size, 
                          const std::string& file, int line,
                          const std::string& function);
    
    // Statistics
    size_t getTotalAllocations() const { return totalAllocations_; }
    size_t getTotalMemoryAllocated() const { return totalMemoryAllocated_; }
    double getAverageAllocationSize() const;
    
    // Hotspots analysis
    std::vector<std::string> getHotspotTags(size_t topN = 10) const;
    std::vector<std::pair<std::string, size_t>> getTopAllocators(size_t topN = 10) const;
    
    // Performance analysis
    double getAllocationFrequency() const;  // allocations per second
    double getMemoryBandwidth() const;  // MB/s
    
    // Reporting
    std::string generateProfileReport() const;
    std::string generateHotspotReport(size_t topN = 10) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool running_;
    size_t totalAllocations_;
    size_t totalMemoryAllocated_;
};

/**
 * RAII memory manager for neural network objects
 * 
 * Smart pointer wrapper for neural network objects with automatic
 * memory tracking, cleanup, and cycle detection.
 */
template<typename T>
class NeuralMemoryManager {
public:
    using Pointer = std::unique_ptr<T>;
    using WeakPointer = std::weak_ptr<T>;
    
    NeuralMemoryManager() : trackAllocations_(true), enableCycleDetection_(true) {}
    
    template<typename... Args>
    std::shared_ptr<T> create(const std::string& tag = "DEFAULT", Args&&... args);
    
    void destroy(const std::shared_ptr<T>& ptr);
    void destroy(WeakPointer weakPtr);
    
    // Memory tracking
    void enableTracking(bool enable) { trackAllocations_ = enable; }
    void enableCycleDetection(bool enable) { enableCycleDetection_ = enable; }
    
    // Statistics
    size_t getActiveObjectCount() const { return activeObjects_.size(); }
    size_t getTotalObjectsCreated() const { return totalObjectsCreated_; }
    double getMemoryEfficiency() const;
    
    // Cleanup
    void cleanup()
    {
        // Remove dead references
        auto it = activeObjects_.begin();
        while (it != activeObjects_.end()) {
            if (it->second.expired()) {
                it = activeObjects_.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    std::string generateReport() const;
    
private:
    std::unordered_map<std::string, std::weak_ptr<T>> activeObjects_;
    std::atomic<size_t> totalObjectsCreated_;
    bool trackAllocations_;
    bool enableCycleDetection_;
    
    std::mutex mutex_;
};

/**
 * Memory leak detector with detailed reporting
 * 
 * Advanced memory leak detection system that provides detailed
 * information about leaked allocations, including stack traces,
 * allocation sites, and memory usage patterns.
 */
class MemoryLeakDetector {
public:
    struct LeakInfo {
        void* pointer;
        size_t size;
        std::string file;
        int line;
        std::string function;
        std::string allocationTag;
        std::chrono::system_clock::time_point allocationTime;
        std::string stackTrace;
        size_t allocationCount;
        
        LeakInfo() : pointer(nullptr), size(0), line(0), allocationCount(0) {}
    };
    
    struct LeakSummary {
        size_t totalLeaks;
        size_t totalLeakSize;
        std::unordered_map<std::string, size_t> leaksByTag;
        std::unordered_map<std::string, size_t> leaksByFile;
        std::vector<LeakInfo> significantLeaks;  // Top N leaks by size
        
        LeakSummary() : totalLeaks(0), totalLeakSize(0) {}
        
        std::string report() const;
    };
    
    MemoryLeakDetector();
    ~MemoryLeakDetector();
    
    // Detection methods
    LeakSummary detectLeaks() const;
    std::vector<LeakInfo> getAllLeaks() const { return getAllLeaksImpl(); }
    
    // Leak filtering and analysis
    std::vector<LeakInfo> getLeaksByTag(const std::string& tag) const;
    std::vector<LeakInfo> getLeaksByFile(const std::string& file) const;
    std::vector<LeakInfo> getLeaksByFunction(const std::string& function) const;
    
    // Leak resolution
    bool attemptResolution(const LeakInfo& leak);
    std::vector<std::string> suggestResolution(const LeakInfo& leak) const;
    
    // Configuration
    void setMinLeakSize(size_t size) { minLeakSize_ = size; }
    void setMaxLeaksToReport(size_t maxLeaks) { maxLeaksToReport_ = maxLeaks; }
    void enableStackTrace(bool enable) { captureStackTraces_ = enable; }
    
    // Reporting
    std::string generateDetailedReport() const;
    std::string generateResolutionReport() const;
    
    // Integration
    void integrateWithMemoryTracker(MemoryTracker& tracker);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    size_t minLeakSize_;
    size_t maxLeaksToReport_;
    bool captureStackTraces_;
    
    std::vector<LeakInfo> getAllLeaksImpl() const;
};

/**
 * Memory fragmentation detector
 * 
 * Detects and analyzes memory fragmentation in neural network simulations,
 * helping to optimize memory usage and improve performance.
 */
class MemoryFragmentationDetector {
public:
    struct FragmentationInfo {
        size_t totalAllocated;
        size_t largestFreeBlock;
        size_t smallFreeBlocks;  // Blocks < 1KB
        size_t mediumFreeBlocks;  // Blocks 1KB - 1MB
        size_t largeFreeBlocks;   // Blocks > 1MB
        double fragmentationIndex;  // 0.0 (perfect) to 1.0 (highly fragmented)
        std::chrono::system_clock::time_point detectionTime;
        
        FragmentationInfo() : totalAllocated(0), largestFreeBlock(0),
                             smallFreeBlocks(0), mediumFreeBlocks(0),
                             largeFreeBlocks(0), fragmentationIndex(0.0) {}
    };
    
    MemoryFragmentationDetector();
    ~MemoryFragmentationDetector();
    
    // Fragmentation detection
    FragmentationInfo detectFragmentation() const;
    double getFragmentationIndex() const { return fragmentationInfo_.fragmentationIndex; }
    
    // Fragmentation analysis
    std::string getFragmentationAnalysis() const;
    std::vector<size_t> getFreeBlockSizes() const;
    std::unordered_map<std::string, size_t> getFragmentationByTag() const;
    
    // Recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    std::vector<std::string> getMemoryPoolRecommendations() const;
    
    // Configuration
    void setBlockSizeThreshold(size_t threshold) { blockSizeThreshold_ = threshold; }
    void enableDetailedAnalysis(bool enable) { detailedAnalysis_ = enable; }
    
    // Integration
    void integrateWithMemoryPools(const std::vector<MemoryPool*>& pools);
    void integrateWithMemoryTracker(MemoryTracker& tracker);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    size_t blockSizeThreshold_;
    bool detailedAnalysis_;
    mutable FragmentationInfo fragmentationInfo_;
};

/**
 * Memory usage monitor
 * 
 * Monitors system and process memory usage in real-time, providing
 * early warning of memory pressure and potential leaks.
 */
class MemoryMonitor {
public:
    struct MemoryUsage {
        double systemMemoryUsed;  // MB
        double systemMemoryAvailable;  // MB
        double systemMemoryTotal;  // MB
        double processMemoryUsed;  // MB
        double processMemoryPeak;  // MB
        size_t allocationCount;
        size_t deallocationCount;
        double memoryGrowthRate;  // MB/s
        std::chrono::system_clock::time_point lastUpdate;
        
        MemoryUsage() : systemMemoryUsed(0), systemMemoryAvailable(0),
                       systemMemoryTotal(0), processMemoryUsed(0),
                       processMemoryPeak(0), allocationCount(0), deallocationCount(0),
                       memoryGrowthRate(0), lastUpdate(std::chrono::system_clock::now()) {}
    };
    
    MemoryMonitor();
    ~MemoryMonitor();
    
    // Monitoring control
    void startMonitoring(double intervalSeconds = 1.0);
    void stopMonitoring();
    bool isMonitoring() const { return monitoring_; }
    
    // Memory usage
    MemoryUsage getCurrentUsage() const;
    double getMemoryUsageMB() const;
    size_t getAllocationCount() const;
    size_t getDeallocationCount() const;
    
    // Alerts and warnings
    bool checkMemoryPressure(double thresholdMB = 1024.0) const;
    std::vector<std::string> getAlerts() const;
    
    // Configuration
    void setMonitoringInterval(double interval) { monitoringInterval_ = interval; }
    void setMemoryPressureThreshold(double threshold) { memoryPressureThreshold_ = threshold; }
    void setGrowthRateThreshold(double threshold) { growthRateThreshold_ = threshold; }
    
    // Reporting
    std::string generateUsageReport() const;
    std::string generateAlertReport() const;
    
    // Integration
    void integrateWithMemoryTracker(MemoryTracker& tracker);
    void integrateWithAllocationProfiler(AllocationProfiler& profiler);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool monitoring_;
    double monitoringInterval_;
    double memoryPressureThreshold_;
    double growthRateThreshold_;
};

/**
 * Memory validation utilities
 * 
 * Provides validation and checking utilities for memory management,
 * including buffer overflow detection and memory corruption detection.
 */
class MemoryValidator {
public:
    enum class ValidationResult {
        PASS,
        FAIL,
        WARNING,
        ERROR
    };
    
    struct ValidationReport {
        ValidationResult result;
        std::string message;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::chrono::system_clock::time_point timestamp;
        
        ValidationReport() : result(ValidationResult::PASS), timestamp(std::chrono::system_clock::now()) {}
        
        std::string report() const;
    };
    
    MemoryValidator();
    ~MemoryValidator();
    
    // Validation methods
    ValidationReport validateMemoryTracker() const;
    ValidationReport validateMemoryPools() const;
    ValidationReport validateMemoryRegions() const;
    ValidationReport validateNeuralMemoryManager() const;
    
    // Buffer overflow detection
    ValidationReport checkBufferOverflows() const;
    ValidationReport checkUseAfterFree() const;
    ValidationReport checkDoubleFree() const;
    
    // Memory corruption detection
    ValidationReport checkMemoryCorruption() const;
    ValidationReport checkAlignment() const;
    ValidationReport checkRedZones() const;
    
    // Configuration
    void enableStrictValidation(bool enable) { strictValidation_ = enable; }
    void setMaxReportSize(size_t maxSize) { maxReportSize_ = maxSize; }
    
    // Integration
    void integrateWithMemoryTracker(MemoryTracker& tracker);
    void integrateWithMemoryLeakDetector(MemoryLeakDetector& detector);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool strictValidation_;
    size_t maxReportSize_;
};

/**
 * Main memory management system
 * 
 * Integrates all memory management components into a cohesive system
 * for neural network simulations with comprehensive tracking, leak
 * detection, and optimization capabilities.
 */
class MemoryManagementSystem {
public:
    MemoryManagementSystem();
    ~MemoryManagementSystem();
    
    // Component access
    MemoryTracker& getMemoryTracker() { return memoryTracker_; }
    MemoryLeakDetector& getMemoryLeakDetector() { return leakDetector_; }
    MemoryFragmentationDetector& getFragmentationDetector() { return fragmentationDetector_; }
    MemoryMonitor& getMemoryMonitor() { return memoryMonitor_; }
    MemoryValidator& getMemoryValidator() { return memoryValidator_; }
    
    // System control
    void start() noexcept;
    void stop() noexcept;
    bool isRunning() const { return running_; }
    
    // System management
    void initializeComponents();
    void cleanupComponents();
    void reset();
    
    // Reporting
    std::string generateSystemReport() const;
    std::string generateDetailedReport() const;
    std::string generateHealthReport() const;
    
    // Configuration
    void enableAllFeatures(bool enable);
    void setLoggingLevel(int level) { logLevel_ = level; }
    void setPerformanceThreshold(double threshold) { performanceThreshold_ = threshold; }
    
    // Integration
    void integrateWithNeuralNetwork(std::shared_ptr<class NeuralNetwork> network);
    void integrateWithBrain(std::shared_ptr<class Brain> brain);
    
    // Singleton
    static MemoryManagementSystem& getInstance();
    
private:
    MemoryTracker memoryTracker_;
    MemoryLeakDetector leakDetector_;
    MemoryFragmentationDetector fragmentationDetector_;
    MemoryMonitor memoryMonitor_;
    MemoryValidator memoryValidator_;
    
    bool running_;
    int logLevel_;
    double performanceThreshold_;
    
    std::mutex mutex_;
};

} // namespace nlm

// Global convenience functions
inline void* trackedNew(size_t size, const std::string& file, int line, 
                       const std::string& function, const std::string& tag = "DEFAULT")
{
    return nlm::MemoryTracker::getInstance().trackAllocation(size, file, line, function,
                                                            nlm::MemoryTracker::AllocationType::New, tag);
}

inline void trackedDelete(void* ptr, nlm::MemoryTracker::AllocationType type = nlm::MemoryTracker::AllocationType::New)
{
    nlm::MemoryTracker::getInstance().trackDeallocation(ptr, type);
}

inline void* trackedMalloc(size_t size, const std::string& file, int line,
                           const std::string& function, const std::string& tag = "DEFAULT")
{
    return nlm::MemoryTracker::getInstance().trackAllocation(size, file, line, function,
                                                            nlm::MemoryTracker::AllocationType::Malloc, tag);
}

inline void trackedFree(void* ptr)
{
    nlm::MemoryTracker::getInstance().trackDeallocation(ptr, nlm::MemoryTracker::AllocationType::Malloc);
}

// RAII wrappers for common patterns
class TrackedAllocation {
public:
    TrackedAllocation(size_t size, const std::string& file, int line,
                     const std::string& function, const std::string& tag = "DEFAULT")
        : ptr_(nullptr)
    {
        ptr_ = trackedNew(size, file, line, function, tag);
    }
    
    ~TrackedAllocation()
    {
        if (ptr_) {
            trackedDelete(ptr_);
        }
    }
    
    void* get() const { return ptr_; }
    
private:
    void* ptr_;
};

#define TRACKED_NEW(type, tag) new(type*) trackedNew(sizeof(type), __FILE__, __LINE__, "", tag)
#define TRACKED_DELETE(ptr) trackedDelete(ptr)

// Memory management macros for debugging
#define MEMORY_CHECK() nlm::MemoryTracker::getInstance().validateMemory()
#define MEMORY_LEAK_CHECK() nlm::MemoryTracker::getInstance().getLeakedAllocations().size()
#define MEMORY_REPORT() nlm::MemoryTracker::getInstance().generateMemoryReport()

#endif // NLM_MEMORY_MANAGEMENT_HPP