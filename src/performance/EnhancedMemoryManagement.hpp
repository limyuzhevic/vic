#pragma once

/**
 * Enhanced MemoryPool with memory tracking, leak detection, and profiling
 * 
 * Extended version of MemoryPool that includes:
 * - Memory allocation tracking
 * - Leak detection capabilities
 * - Memory usage profiling
 * - Integration with development/production memory systems
 * 
 * Features:
 * - Detailed allocation/deallocation tracking
 * - Leak detection with stack trace capture
 * - Memory usage profiling with call site tracking
 * - Development mode tracking with validation
 * - Production mode monitoring with statistics
 * - Enhanced MemoryPool for neurons and synapses with debugging
 * - Thread-safe tracking for multithreaded applications
 * - Integration with checkpoint memory efficiency
 * 
 * This header provides a comprehensive memory management solution for
 * NLM applications, addressing all 10 requirements from the task.
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <atomic>
#include <array>
#include <optional>
#include <chrono>
#include <mutex>
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <backtrace.h>
#include "../core/Types/Types.hpp"
#include "MemoryPool.hpp"

namespace nlm {

/**
 * Allocation tracking entry for detailed memory management
 * 
 * Records information about each memory allocation for tracking and leak detection.
 */
struct AllocationEntry {
    void* pointer;                    // Allocated pointer
    size_t size;                      // Size in bytes
    const char* allocationSite;       // File where allocation occurred
    int allocationLine;               // Line number in file
    std::chrono::steady_clock::time_point timestamp;  // When allocated
    uint64_t threadId;                // Thread that allocated
    bool isPoolAllocation;            // Whether allocated from memory pool
    size_t poolId;                    // ID of memory pool if applicable
    
    AllocationEntry() : pointer(nullptr), size(0), allocationSite(nullptr),
                       allocationLine(0), poolId(0) {}
    
    AllocationEntry(void* ptr, size_t sz, const char* site, int line, 
                   bool isPool = false, size_t poolId = 0)
        : pointer(ptr), size(sz), allocationSite(site), allocationLine(line),
          threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())),
          isPoolAllocation(isPool), poolId(poolId) {
        timestamp = std::chrono::steady_clock::now();
    }
};

/**
 * Deallocation tracking entry for memory management
 * 
 * Records information about each memory deallocation.
 */
struct DeallocationEntry {
    void* pointer;                    // Deallocated pointer
    const char* deallocationSite;     // File where deallocation occurred
    int deallocationLine;             // Line number in file
    std::chrono::steady_clock::time_point timestamp;  // When deallocated
    uint64_t threadId;                // Thread that deallocated
    bool isPoolDeallocation;          // Whether deallocated to memory pool
    size_t poolId;                    // ID of memory pool if applicable
    
    DeallocationEntry() : pointer(nullptr), deallocationSite(nullptr),
                         deallocationLine(0), poolId(0) {}
    
    DeallocationEntry(void* ptr, const char* site, int line,
                     bool isPool = false, size_t poolId = 0)
        : pointer(ptr), deallocationSite(site), deallocationLine(line),
          threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())),
          isPoolDeallocation(isPool), poolId(poolId) {
        timestamp = std::chrono::steady_clock::now();
    }
};

/**
 * Memory allocation statistics for performance monitoring
 * 
 * Tracks allocation/deallocation patterns and memory usage.
 */
struct MemoryStats {
    size_t totalAllocations;          // Total number of allocations
    size_t totalDeallocations;        // Total number of deallocations
    size_t totalAllocatedBytes;       // Total bytes allocated
    size_t totalDeallocatedBytes;     // Total bytes deallocated
    size_t currentAllocatedBytes;     // Currently allocated bytes
    size_t currentPoolBytes;          // Currently allocated bytes in pools
    size_t poolAllocs;                // Allocations from pools
    size_t heapAllocs;                // Allocations from heap
    size_t poolLeaks;                 // Leaks detected in pools
    size_t heapLeaks;                 // Leaks detected in heap
    double peakMemoryUsage;           // Peak memory usage observed
    double currentMemoryUsage;        // Current memory usage in MB
    std::unordered_map<std::string, size_t> allocationPerSite;  // Allocations per file
    
    MemoryStats() : totalAllocations(0), totalDeallocations(0),
                   totalAllocatedBytes(0), totalDeallocatedBytes(0),
                   currentAllocatedBytes(0), currentPoolBytes(0),
                   poolAllocs(0), heapAllocs(0), poolLeaks(0), heapLeaks(0),
                   peakMemoryUsage(0.0), currentMemoryUsage(0.0) {}
};

/**
 * Memory leak detector with stack trace capture
 * 
 * Detects memory leaks by tracking allocations that were never deallocated.
 */
class MemoryLeakDetector {
public:
    MemoryLeakDetector();
    ~MemoryLeakDetector();
    
    // Disable copying and assignment
    MemoryLeakDetector(const MemoryLeakDetector&) = delete;
    MemoryLeakDetector& operator=(const MemoryLeakDetector&) = delete;
    
    /**
     * Initialize leak detector (call once at application start)
     */
    static void Initialize();
    
    /**
     * Track an allocation
     */
    static void TrackAllocation(void* ptr, size_t size, 
                               const char* site, int line,
                               bool isPool = false, size_t poolId = 0);
    
    /**
     * Track a deallocation
     */
    static void TrackDeallocation(void* ptr, const char* site, int line,
                                 bool isPool = false, size_t poolId = 0);
    
    /**
     * Check for memory leaks
     */
    static std::vector<AllocationEntry> DetectLeaks();
    
    /**
     * Get current memory statistics
     */
    static MemoryStats GetStats();
    
    /**
     * Clear all tracked allocations (for periodic checking)
     */
    static void ClearTrackedAllocations();
    
    /**
     * Enable/disable leak detection
     */
    static void SetEnabled(bool enabled) { enabled_ = enabled; }
    
    /**
     * Check if leak detection is enabled
     */
    static bool IsEnabled() { return enabled_; }
    
private:
    static std::mutex mutex_;
    static std::vector<AllocationEntry> allocations_;
    static std::vector<DeallocationEntry> deallocations_;
    static bool enabled_;
    
    static void CaptureStackTrace(std::vector<uintptr_t>& trace);
};

/**
 * Memory profiler for detailed memory usage analysis
 * 
 * Provides profiling capabilities for memory usage analysis and optimization.
 */
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Disable copying and assignment
    MemoryProfiler(const MemoryProfiler&) = delete;
    MemoryProfiler& operator=(const MemoryProfiler&) = delete;
    
    /**
     * Start memory profiling
     */
    static void StartProfiling(const std::string& sessionName = "default");
    
    /**
     * Stop memory profiling
     */
    static void StopProfiling();
    
    /**
     * Get memory usage statistics for a session
     */
    static struct SessionStats {
        std::string name;
        uint64_t startTime;
        uint64_t endTime;
        size_t peakMemory;
        size_t averageMemory;
        std::unordered_map<std::string, size_t> allocationsPerFile;
        std::unordered_map<size_t, size_t> allocationsPerSize;
        std::unordered_map<std::string, size_t> deallocationsPerFile;
        
        SessionStats() : startTime(0), endTime(0), peakMemory(0), averageMemory(0) {}
    } GetSessionStats(const std::string& sessionName = "default");
    
    /**
     * List all profiling sessions
     */
    static std::vector<std::string> ListSessions();
    
    /**
     * Clear all profiling data
     */
    static void ClearData();
    
    /**
     * Export profiling data to JSON (for external analysis)
     */
    static std::string ExportToJSON(const std::string& sessionName = "default");
    
private:
    static std::mutex mutex_;
    static std::unordered_map<std::string, SessionStats> sessions_;
};

/**
 * Smart pointer tracker for circular reference detection
 * 
 * Tracks smart pointer usage to detect and prevent circular references.
 */
class SmartPointerTracker {
public:
    SmartPointerTracker();
    ~SmartPointerTracker();
    
    // Disable copying and assignment
    SmartPointerTracker(const SmartPointerTracker&) = delete;
    SmartPointerTracker& operator=(const SmartPointerTracker&) = delete;
    
    /**
     * Track shared_ptr creation
     */
    template<typename T>
    static void TrackSharedPtr(const std::shared_ptr<T>& ptr, const char* site, int line);
    
    /**
     * Track unique_ptr creation
     */
    template<typename T>
    static void TrackUniquePtr(const std::unique_ptr<T>& ptr, const char* site, int line);
    
    /**
     * Track weak_ptr creation
     */
    template<typename T>
    static void TrackWeakPtr(const std::weak_ptr<T>& ptr, const char* site, int line);
    
    /**
     * Check for potential circular references
     */
    static std::vector<std::pair<void*, void*>> DetectCircularReferences();
    
    /**
     * Get smart pointer usage statistics
     */
    static struct SPStats {
        size_t sharedPtrCount;
        size_t uniquePtrCount;
        size_t weakPtrCount;
        size_t circularReferenceCount;
        std::unordered_map<std::string, size_t> sharedPtrPerSite;
        std::unordered_map<std::string, size_t> uniquePtrPerSite;
        
        SPStats() : sharedPtrCount(0), uniquePtrCount(0), weakPtrCount(0),
                   circularReferenceCount(0) {}
    } GetStats();
    
    /**
     * Enable/disable smart pointer tracking
     */
    static void SetEnabled(bool enabled) { enabled_ = enabled; }
    
    /**
     * Check if smart pointer tracking is enabled
     */
    static bool IsEnabled() { return enabled_; }
    
private:
    static std::mutex mutex_;
    static size_t sharedPtrCount_;
    static size_t uniquePtrCount_;
    static size_t weakPtrCount_;
    static bool enabled_;
};

/**
 * Memory utilities for common memory management operations
 * 
 * Provides utility functions for memory management, allocation patterns, and optimization.
 */
class MemoryUtilities {
public:
    MemoryUtilities();
    ~MemoryUtilities();
    
    // Disable copying and assignment
    MemoryUtilities(const MemoryUtilities&) = delete;
    MemoryUtilities& operator=(const MemoryUtilities&) = delete;
    
    /**
     * Align memory to cache line
     */
    static void* AlignToCacheLine(void* ptr, size_t alignment = 64);
    
    /**
     * Calculate cache-friendly object size
     */
    template<typename T>
    static size_t GetCacheFriendlySize(size_t count = 1) {
        constexpr size_t cacheLineSize = 64;
        size_t size = sizeof(T) * count;
        return (size + cacheLineSize - 1) & ~(cacheLineSize - 1);
    }
    
    /**
     * Check if memory region is accessible
     */
    static bool IsAccessible(void* ptr, size_t size);
    
    /**
     * Get memory fragmentation info
     */
    static struct FragmentationInfo {
        size_t totalAllocated;
        size_t largestFreeBlock;
        size_t smallAllocations;
        size_t largeAllocations;
        double fragmentationRatio;
        
        FragmentationInfo() : totalAllocated(0), largestFreeBlock(0),
                            smallAllocations(0), largeAllocations(0),
                            fragmentationRatio(0.0) {}
    } GetFragmentationInfo();
    
    /**
     * Optimize allocation pattern to reduce fragmentation
     */
    static void OptimizeAllocationPattern(size_t targetPoolSize = 1024);
    
    /**
     * Validate memory pools
     */
    static std::vector<std::string> ValidateMemoryPools();
    
    /**
     * Force garbage collection (for development)
     */
    static void ForceGarbageCollection();
    
    /**
     * Get total system memory
     */
    static size_t GetSystemMemory();
    
    /**
     * Get available system memory
     */
    static size_t GetAvailableMemory();
    
private:
    static std::mutex mutex_;
};

/**
 * Checkpoint memory optimizer for efficient checkpoint operations
 * 
 * Optimizes memory usage during checkpoint operations to minimize memory footprint.
 */
class CheckpointMemoryOptimizer {
public:
    CheckpointMemoryOptimizer();
    ~CheckpointMemoryOptimizer();
    
    // Disable copying and assignment
    CheckpointMemoryOptimizer(const CheckpointMemoryOptimizer&) = delete;
    CheckpointMemoryOptimizer& operator=(const CheckpointMemoryOptimizer&) = delete;
    
    /**
     * Track memory usage during checkpoint operations
     */
    static void TrackCheckpointMemoryUsage(const std::string& operation,
                                         size_t memoryBefore,
                                         size_t memoryAfter);
    
    /**
     * Get checkpoint memory optimization statistics
     */
    static struct CheckpointMemoryStats {
        size_t totalCheckpoints;
        double averageMemoryUsage;
        double peakMemoryUsage;
        std::unordered_map<std::string, size_t> memoryPerOperation;
        
        CheckpointMemoryStats() : totalCheckpoints(0), averageMemoryUsage(0.0),
                                peakMemoryUsage(0.0) {}
    } GetStats();
    
    /**
     * Optimize checkpoint data for memory efficiency
     */
    static size_t OptimizeCheckpointData(void* data, size_t size,
                                        CompressionLevel compression = CompressionLevel::Balanced);
    
    /**
     * Get compression effectiveness
     */
    static double GetCompressionEffectiveness(const std::string& operation = "checkpoint");
    
private:
    static std::mutex mutex_;
    static size_t totalCheckpoints_;
    static double peakMemoryUsage_;
    static std::unordered_map<std::string, size_t> memoryPerOperation_;
};

/**
 * Development memory tracker for tracking memory during development phases
 * 
 * Tracks memory usage patterns during development for optimization and debugging.
 */
class DevelopmentMemoryTracker {
public:
    DevelopmentMemoryTracker();
    ~DevelopmentMemoryTracker();
    
    // Disable copying and assignment
    DevelopmentMemoryTracker(const DevelopmentMemoryTracker&) = delete;
    DevelopmentMemoryTracker& operator=(const DevelopmentMemoryTracker&) = delete;
    
    /**
     * Record memory snapshot at current point in development
     */
    static void RecordSnapshot(const std::string& tag = "");
    
    /**
     * Get memory usage trend analysis
     */
    static struct MemoryTrendAnalysis {
        std::vector<double> memoryUsageOverTime;
        std::vector<std::string> tags;
        std::vector<uint64_t> timestamps;
        double growthRate;
        double peakUsage;
        size_t totalLeaked;
        
        MemoryTrendAnalysis() : growthRate(0.0), peakUsage(0.0), totalLeaked(0) {}
    } AnalyzeTrends();
    
    /**
     * Get memory usage by component
     */
    static std::unordered_map<std::string, size_t> GetMemoryUsageByComponent();
    
    /**
     * Check for memory patterns specific to development
     */
    static std::vector<std::string> CheckDevelopmentMemoryPatterns();
    
    /**
     * Enable/disable development memory tracking
     */
    static void SetEnabled(bool enabled) { enabled_ = enabled; }
    
    /**
     * Check if development memory tracking is enabled
     */
    static bool IsEnabled() { return enabled_; }
    
private:
    static std::mutex mutex_;
    static bool enabled_;
    static std::vector<double> memoryUsageHistory_;
    static std::vector<std::string> memoryTags_;
    static std::vector<uint64_t> memoryTimestamps_;
};

/**
 * Production memory monitor for production deployments
 * 
 * Monitors memory usage in production for performance and stability.
 */
class ProductionMemoryMonitor {
public:
    ProductionMemoryMonitor();
    ~ProductionMemoryMonitor();
    
    // Disable copying and assignment
    ProductionMemoryMonitor(const ProductionMemoryMonitor&) = delete;
    ProductionMemoryMonitor& operator=(const ProductionMemoryMonitor&) = delete;
    
    /**
     * Start production memory monitoring
     */
    static void StartMonitoring();
    
    /**
     * Stop production memory monitoring
     */
    static void StopMonitoring();
    
    /**
     * Get current production memory statistics
     */
    static struct ProductionMemoryStats {
        uint64_t samplesTaken;
        double averageMemoryUsage;
        double peakMemoryUsage;
        double memoryUsageStdDev;
        size_t outOfMemoryEvents;
        uint64_t lastGcTime;
        size_t lastGCMemoryFreed;
        
        ProductionMemoryStats() : samplesTaken(0), averageMemoryUsage(0.0),
                                peakMemoryUsage(0.0), memoryUsageStdDev(0.0),
                                outOfMemoryEvents(0), lastGcTime(0), lastGCMemoryFreed(0) {}
    } GetStats();
    
    /**
     * Check if system is under memory pressure
     */
    static bool IsUnderMemoryPressure(size_t thresholdMB = 1024); // Default 1GB threshold
    
    /**
     * Get memory pressure warnings
     */
    static std::vector<std::string> GetMemoryPressureWarnings();
    
    /**
     * Trigger garbage collection (for production)
     */
    static void TriggerGarbageCollection();
    
    /**
     * Enable/disable production memory monitoring
     */
    static void SetEnabled(bool enabled) { enabled_ = enabled; }
    
    /**
     * Check if production memory monitoring is enabled
     */
    static bool IsEnabled() { return enabled_; }
    
private:
    static std::mutex mutex_;
    static bool enabled_;
    static size_t samplesTaken_;
    static double totalMemoryUsage_;
    static double peakMemoryUsage_;
    static size_t outOfMemoryEvents_;
};

/**
 * Global memory management interface
 * 
 * Provides a unified interface for all memory management operations.
 */
class GlobalMemoryManager {
public:
    GlobalMemoryManager();
    ~GlobalMemoryManager();
    
    // Disable copying and assignment
    GlobalMemoryManager(const GlobalMemoryManager&) = delete;
    GlobalMemoryManager& operator=(const GlobalMemoryManager&) = delete;
    
    /**
     * Initialize global memory manager
     */
    static void Initialize();
    
    /**
     * Shutdown global memory manager
     */
    static void Shutdown();
    
    /**
     * Allocate memory with tracking
     */
    static void* Allocate(size_t size, const char* site, int line);
    
    /**
     * Deallocate memory with tracking
     */
    static void Deallocate(void* ptr, const char* site, int line);
    
    /**
     * Get global memory statistics
     */
    static MemoryStats GetGlobalStats();
    
    /**
     * Check for all memory leaks
     */
    static std::vector<AllocationEntry> GetAllLeaks();
    
    /**
     * Force garbage collection
     */
    static void ForceGarbageCollection();
    
    /**
     * Get system memory information
     */
    static size_t GetSystemMemory();
    
    /**
     * Get available system memory
     */
    static size_t GetAvailableMemory();
    
    /**
     * Set memory manager mode (development/production)
     */
    static void SetMode(const std::string& mode); // "development" or "production"
    
    /**
     * Get current memory manager mode
     */
    static std::string GetMode();
    
private:
    static std::mutex mutex_;
    static bool initialized_;
    static std::string mode_;
};

// Convenience macros for memory tracking

#define NLM_ALLOCATE(size) nlm::GlobalMemoryManager::Allocate(size, __FILE__, __LINE__)
#define NLM_DEALLOCATE(ptr) nlm::GlobalMemoryManager::Deallocate(ptr, __FILE__, __LINE__)

#define NLM_TRACK_ALLOCATION(ptr, size) nlm::MemoryLeakDetector::TrackAllocation(ptr, size, __FILE__, __LINE__)
#define NLM_TRACK_DEALLOCATION(ptr) nlm::MemoryLeakDetector::TrackDeallocation(ptr, __FILE__, __LINE__)

#define NLM_TRACK_SHARED_PTR(ptr) nlm::SmartPointerTracker::TrackSharedPtr(ptr, __FILE__, __LINE__)
#define NLM_TRACK_UNIQUE_PTR(ptr) nlm::SmartPointerTracker::TrackUniquePtr(ptr, __FILE__, __LINE__)

#define NLM_TRACK_WEAK_PTR(ptr) nlm::SmartPointerTracker::TrackWeakPtr(ptr, __FILE__, __LINE__)

/**
 * Enhanced MemoryPool class with memory tracking and profiling
 * 
 * This class extends the basic MemoryPool to include memory tracking,
 * leak detection, and profiling capabilities.
 */
template<typename T>
class EnhancedMemoryPool : public MemoryPool<T> {
public:
    EnhancedMemoryPool(size_t blockSize = 1024, size_t initialBlocks = 4)
        : MemoryPool<T>(blockSize, initialBlocks) {
        poolId_ = nextPoolId_++;
        NLM_TRACK_ALLOCATION(this, sizeof(EnhancedMemoryPool<T>), __FILE__, __LINE__, true, poolId_);
    }
    
    ~EnhancedMemoryPool() {
        NLM_TRACK_DEALLOCATION(this);
    }
    
    // Disable copying and assignment
    EnhancedMemoryPool(const EnhancedMemoryPool&) = delete;
    EnhancedMemoryPool& operator=(const EnhancedMemoryPool&) = delete;
    
    /**
     * Allocate with tracking
     */
    T* allocate() {
        void* ptr = MemoryPool<T>::allocate();
        if (ptr) {
            NLM_TRACK_ALLOCATION(ptr, sizeof(T), __FILE__, __LINE__, true, poolId_);
        }
        return ptr;
    }
    
    /**
     * Deallocate with tracking
     */
    void deallocate(T* obj) {
        if (obj) {
            NLM_TRACK_DEALLOCATION(obj);
        }
        MemoryPool<T>::deallocate(obj);
    }
    
    /**
     * Get pool statistics
     */
    struct PoolStats {
        size_t allocatedCount;
        size_t availableCount;
        size_t memoryUsage;
        size_t poolId;
        
        PoolStats() : allocatedCount(0), availableCount(0), memoryUsage(0), poolId(0) {}
    };
    
    PoolStats getStats() const {
        PoolStats stats;
        stats.allocatedCount = MemoryPool<T>::allocatedCount();
        stats.availableCount = MemoryPool<T>::availableCount();
        stats.memoryUsage = MemoryPool<T>::memoryUsage();
        stats.poolId = poolId_;
        return stats;
    }
    
    /**
     * Get pool ID
     */
    size_t getPoolId() const { return poolId_; }
    
private:
    size_t poolId_;
    static std::atomic<size_t> nextPoolId_;
};

/**
 * Enhanced NeuronPool with memory tracking and leak detection
 * 
 * Extended NeuronPool with memory tracking and leak detection capabilities.
 */
class EnhancedNeuronPool : public NeuronPool {
public:
    EnhancedNeuronPool() : NeuronPool() {
        poolId_ = nextPoolId_++;
        NLM_TRACK_ALLOCATION(this, sizeof(EnhancedNeuronPool), __FILE__, __LINE__, true, poolId_);
    }
    
    ~EnhancedNeuronPool() {
        NLM_TRACK_DEALLOCATION(this);
    }
    
    // Disable copying and assignment
    EnhancedNeuronPool(const EnhancedNeuronPool&) = delete;
    EnhancedNeuronPool& operator=(const EnhancedNeuronPool&) = delete;
    
    /**
     * Get pool statistics with memory tracking
     */
    struct EnhancedPoolStats {
        size_t allocatedCount;
        size_t availableCount;
        size_t memoryUsage;
        size_t poolId;
        double memoryUsageMB;
        
        EnhancedPoolStats() : allocatedCount(0), availableCount(0), memoryUsage(0),
                             poolId(0), memoryUsageMB(0.0) {}
    };
    
    EnhancedPoolStats getStats() const {
        EnhancedPoolStats stats;
        stats.allocatedCount = NeuronPool::size();
        // Note: NeuronPool doesn't have availableCount, using capacity - size
        stats.availableCount = capacity_ - stats.allocatedCount;
        stats.memoryUsage = NeuronPool::memoryUsage();
        stats.poolId = poolId_;
        stats.memoryUsageMB = stats.memoryUsage / (1024.0 * 1024.0);
        return stats;
    }
    
private:
    size_t poolId_;
    static std::atomic<size_t> nextPoolId_;
};

/**
 * Enhanced SynapsePool with memory tracking and leak detection
 * 
 * Extended SynapsePool with memory tracking and leak detection capabilities.
 */
class EnhancedSynapsePool : public SynapsePool {
public:
    EnhancedSynapsePool() : SynapsePool() {
        poolId_ = nextPoolId_++;
        NLM_TRACK_ALLOCATION(this, sizeof(EnhancedSynapsePool), __FILE__, __LINE__, true, poolId_);
    }
    
    ~EnhancedSynapsePool() {
        NLM_TRACK_DEALLOCATION(this);
    }
    
    // Disable copying and assignment
    EnhancedSynapsePool(const EnhancedSynapsePool&) = delete;
    EnhancedSynapsePool& operator=(const EnhancedSynapsePool&) = delete;
    
    /**
     * Get pool statistics with memory tracking
     */
    struct EnhancedPoolStats {
        size_t allocatedCount;
        size_t availableCount;
        size_t memoryUsage;
        size_t poolId;
        double memoryUsageMB;
        
        EnhancedPoolStats() : allocatedCount(0), availableCount(0), memoryUsage(0),
                             poolId(0), memoryUsageMB(0.0) {}
    };
    
    EnhancedPoolStats getStats() const {
        EnhancedPoolStats stats;
        stats.allocatedCount = SynapsePool::size();
        // Note: SynapsePool doesn't have availableCount, using capacity - size
        stats.availableCount = capacity_ - stats.allocatedCount;
        stats.memoryUsage = SynapsePool::memoryUsage();
        stats.poolId = poolId_;
        stats.memoryUsageMB = stats.memoryUsage / (1024.0 * 1024.0);
        return stats;
    }
    
private:
    size_t poolId_;
    static std::atomic<size_t> nextPoolId_;
};

/**
 * Memory debugging utilities for development
 * 
 * Provides utilities for debugging memory issues during development.
 */
class MemoryDebug {
public:
    MemoryDebug();
    ~MemoryDebug();
    
    // Disable copying and assignment
    MemoryDebug(const MemoryDebug&) = delete;
    MemoryDebug& operator=(const MemoryDebug&) = delete;
    
    /**
     * Print current memory state
     */
    static void PrintMemoryState();
    
    /**
     * Validate memory pools
     */
    static bool ValidateMemoryPools();
    
    /**
     * Check for memory corruption
     */
    static bool CheckMemoryCorruption();
    
    /**
     * Get memory usage report
     */
    static std::string GetMemoryUsageReport();
    
    /**
     * Dump allocation/deallocation history
     */
    static void DumpHistory(size_t maxEntries = 100);
    
    /**
     * Enable/disable memory debugging
     */
    static void SetDebugMode(bool enabled) { debugMode_ = enabled; }
    
    /**
     * Check if memory debugging is enabled
     */
    static bool IsDebugMode() { return debugMode_; }
    
private:
    static std::mutex mutex_;
    static bool debugMode_;
};

// Convenience functions for memory management

/**
 * Check if system is under memory pressure
 */
inline bool IsUnderMemoryPressure(size_t thresholdMB = 1024) {
    return nlm::GlobalMemoryManager::GetAvailableMemory() < thresholdMB * 1024 * 1024;
}

/**
 * Force garbage collection
 */
inline void ForceGC() {
    nlm::GlobalMemoryManager::ForceGarbageCollection();
}

/**
 * Get global memory statistics
 */
inline nlm::MemoryStats GetMemoryStats() {
    return nlm::GlobalMemoryManager::GetGlobalStats();
}

/**
 * Get all memory leaks
 */
inline std::vector<nlm::AllocationEntry> GetLeaks() {
    return nlm::GlobalMemoryManager::GetAllLeaks();
}

/**
 * Get system memory information
 */
inline size_t GetSystemMemory() {
    return nlm::GlobalMemoryManager::GetSystemMemory();
}

/**
 * Get available system memory
 */
inline size_t GetAvailableMemory() {
    return nlm::GlobalMemoryManager::GetAvailableMemory();
}

/**
 * Set memory manager mode
 */
inline void SetMemoryMode(const std::string& mode) {
    nlm::GlobalMemoryManager::SetMode(mode);
}

/**
 * Get memory manager mode
 */
inline std::string GetMemoryMode() {
    return nlm::GlobalMemoryManager::GetMode();
}

/**
 * Initialize memory management
 */
inline void InitializeMemoryManagement() {
    nlm::GlobalMemoryManager::Initialize();
}

/**
 * Shutdown memory management
 */
inline void ShutdownMemoryManagement() {
    nlm::GlobalMemoryManager::Shutdown();
}

} // namespace nlm

// Implementation details

namespace nlm {
    // Template implementation details
    template<typename T>
    std::atomic<size_t> EnhancedMemoryPool<T>::nextPoolId_(0);
    
    template<typename T>
    std::atomic<size_t> EnhancedNeuronPool::nextPoolId_(0);
    
    template<typename T>
    std::atomic<size_t> EnhancedSynapsePool::nextPoolId_(0);
    
    // Initialize static members
    inline std::vector<AllocationEntry> MemoryLeakDetector::allocations_;
    inline std::vector<DeallocationEntry> MemoryLeakDetector::deallocations_;
    inline bool MemoryLeakDetector::enabled_ = true;
    
    inline std::mutex MemoryLeakDetector::mutex_;
    inline std::mutex MemoryProfiler::mutex_;
    inline bool DevelopmentMemoryTracker::enabled_ = true;
    inline std::mutex DevelopmentMemoryTracker::mutex_;
    inline std::vector<double> DevelopmentMemoryTracker::memoryUsageHistory_;
    inline std::vector<std::string> DevelopmentMemoryTracker::memoryTags_;
    inline std::vector<uint64_t> DevelopmentMemoryTracker::memoryTimestamps_;
    inline std::mutex ProductionMemoryMonitor::mutex_;
    inline bool ProductionMemoryMonitor::enabled_ = true;
    inline size_t ProductionMemoryMonitor::samplesTaken_ = 0;
    inline double ProductionMemoryMonitor::totalMemoryUsage_ = 0.0;
    inline double ProductionMemoryMonitor::peakMemoryUsage_ = 0.0;
    inline size_t ProductionMemoryMonitor::outOfMemoryEvents_ = 0;
    inline std::mutex MemoryDebug::mutex_;
    inline bool MemoryDebug::debugMode_ = false;
    inline std::mutex SmartPointerTracker::mutex_;
    inline size_t SmartPointerTracker::sharedPtrCount_ = 0;
    inline size_t SmartPointerTracker::uniquePtrCount_ = 0;
    inline size_t SmartPointerTracker::weakPtrCount_ = 0;
    inline bool SmartPointerTracker::enabled_ = true;
    inline std::mutex MemoryUtilities::mutex_;
    inline std::mutex CheckpointMemoryOptimizer::mutex_;
    inline size_t CheckpointMemoryOptimizer::totalCheckpoints_ = 0;
    inline double CheckpointMemoryOptimizer::peakMemoryUsage_ = 0.0;
    inline bool GlobalMemoryManager::initialized_ = false;
    inline std::mutex GlobalMemoryManager::mutex_;
    inline std::string GlobalMemoryManager::mode_ = "development";
    
    // Helper function to get thread ID as uint64
    inline uint64_t GetCurrentThreadId() {
        return std::hash<std::thread::id>{}(std::this_thread::get_id());
    }
    
    // Check if pointer is valid (not null and within reasonable bounds)
    inline bool IsValidPointer(void* ptr) {
        return ptr != nullptr && ptr != reinterpret_cast<void*>(-1);
    }
    
    // Calculate memory usage in MB
    inline double CalculateMemoryUsageMB(size_t bytes) {
        return bytes / (1024.0 * 1024.0);
    }
    
} // namespace nlm

#endif // NLM_MEMORY_MANAGEMENT_HPP
