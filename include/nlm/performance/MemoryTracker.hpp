#pragma once

/**
 * MemoryTracker - Memory usage monitoring and analysis
 * 
 * Monitors memory allocation patterns, usage statistics, and provides
 * optimization suggestions for memory-efficient neural simulation.
 * 
 * Features:
 * - Real-time memory usage tracking
 * - Heap and pool allocation monitoring
 * - Memory leak detection
 * - Memory pool efficiency analysis
 * - Fragmentation monitoring
 * - Automatic optimization recommendations
 * 
 * Biological Motivation:
 * Neural systems maintain optimal memory efficiency through
 * synaptic pruning and homeostatic plasticity
 */

#include <chrono>
#include <memory>
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_map>
#include <deque>
#include <numeric>
#include <cmath>
#include <algorithm>
#include <stack>
#include <optional>
#include <set>
#include <shared_mutex>

namespace nlm {

/**
 * Memory allocation record for tracking
 */
struct MemoryAllocationRecord {
    std::chrono::high_resolution_clock::time_point timestamp;
    size_t allocationSize;
    size_t totalAllocated;
    size_t totalDeallocated;
    std::string allocationType;
    std::string context;
    std::unordered_map<std::string, std::string> metadata;
    std::vector<std::string> callStack;
};

/**
 * Memory pool efficiency metrics
 */
struct MemoryPoolMetrics {
    std::string poolName;
    size_t capacity;
    size_t used;
    size_t allocated;
    size_t deallocated;
    size_t peakUsage;
    double utilizationRate;
    double allocationEfficiency;
    std::chrono::nanoseconds averageAllocationTime;
    std::chrono::nanoseconds maxAllocationTime;
    std::chrono::nanoseconds minAllocationTime;
    size_t fragmentationLevel;
    bool isHealthy{true};
    std::string healthStatus;
};

/**
 * Memory leak information
 */
struct MemoryLeakInfo {
    std::string allocationSite;
    size_t leakedSize;
    size_t allocatedCount;
    std::chrono::high_resolution_clock::time_point firstLeakTime;
    std::chrono::high_resolution_clock::time_point lastLeakTime;
    std::unordered_map<std::string, std::string> context;
    std::vector<std::string> callStackTrace;
};

/**
 * Memory fragmentation information
 */
struct MemoryFragmentationInfo {
    size_t totalFreeMemory{0};
    size_t largestFreeBlock{0};
    size_t smallFreeBlocks{0};
    double fragmentationRatio{0.0};
    std::vector<size_t> freeBlockSizes;
    std::string fragmentationType;
    std::string recommendation;
};

/**
 * Memory usage alert
 */
struct MemoryAlert {
    enum class AlertType { LEAK_DETECTED, HIGH_USAGE, POOL_FULL, FRAGMENTATION, EFFICIENCY_WARNING };
    
    AlertType type;
    std::string description;
    size_t severity; // 1-10
    std::chrono::high_resolution_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> details;
};

/**
 * MemoryTracker - Monitors memory usage and allocation patterns
 */
class MemoryTracker {
public:
    using AllocationCallback = std::function<void(const MemoryAllocationRecord&)>
;    using MemoryAlertCallback = std::function<void(const MemoryAlert&)>
;    using MemoryPoolCallback = std::function<void(const MemoryPoolMetrics&)>
;    
    /**
     * Create a memory tracker
     * @param enableLeakDetection Enable memory leak detection
     * @param enableFragmentationDetection Enable memory fragmentation detection
     * @param checkIntervalMs Interval for memory health checks (ms)
     */
    MemoryTracker(
        bool enableLeakDetection = true,
        bool enableFragmentationDetection = true,
        std::chrono::milliseconds checkInterval = std::chrono::milliseconds(1000));
    
    /**
     * Start tracking memory usage
     * @param trackHeaps Track heap allocations
     * @param trackPools Track memory pools
     */
    void start(bool trackHeaps = true, bool trackPools = true);
    
    /**
     * Stop tracking memory usage
     */
    void stop();
    
    /**
     * Check if tracker is running
     */
    bool isRunning() const { return running_.load(std::memory_order_relaxed); }
    
    /**
     * Record memory allocation
     * @param size Size of allocation (bytes)
     * @param type Type of allocation
     * @param context Context information
     * @param callStack Call stack for leak detection
     */
    void recordAllocation(size_t size, const std::string& type,
                         const std::string& context = "",
                         const std::vector<std::string>& callStack = {});
    
    /**
     * Record memory deallocation
     * @param size Size of deallocation (bytes)
     * @param type Type of deallocation
     * @param context Context information
     */
    void recordDeallocation(size_t size, const std::string& type,
                           const std::string& context = "");
    
    /**
     * Update memory pool metrics
     * @param poolName Name of the memory pool
     * @param capacity Total capacity of pool
     * @param used Current usage
     * @param allocated Total allocated
     * @param deallocated Total deallocated
     * @param peakUsage Peak usage
     */
    void updatePoolMetrics(const std::string& poolName, size_t capacity,
                         size_t used, size_t allocated, size_t deallocated,
                         size_t peakUsage);
    
    /**
     * Record current memory usage
     * @param allocated Total allocated memory
     * @param used Used memory
     * @param peak Peak memory usage
     * @param reserved Reserved memory
     */
    void recordMemoryUsage(size_t allocated, size_t used, size_t peak, size_t reserved);
    
    /**
     * Get current memory usage statistics
     */
    struct CurrentMemoryStats {
        size_t allocated{0};
        size_t used{0};
        size_t peak{0};
        size_t reserved{0};
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    CurrentMemoryStats getCurrentMemoryStats() const;
    
    /**
     * Get memory pool metrics
     * @param poolName Name of pool to retrieve
     * @return Memory pool metrics
     */
    std::optional<MemoryPoolMetrics> getPoolMetrics(const std::string& poolName) const;
    
    /**
     * Get memory pool metrics for all pools
     */
    std::vector<MemoryPoolMetrics> getAllPoolMetrics() const;
    
    /**
     * Get memory leak information
     * @return Vector of memory leak information
     */
    std::vector<MemoryLeakInfo> getMemoryLeaks() const;
    
    /**
     * Get memory fragmentation information
     */
    MemoryFragmentationInfo getMemoryFragmentationInfo() const;
    
    /**
     * Get memory usage history
     * @param timeWindow Time window to look back (0 = all history)
     * @return Vector of memory usage records
     */
    std::vector<MemoryAllocationRecord> getMemoryUsageHistory(
        std::chrono::high_resolution_clock::duration timeWindow = std::chrono::hours(1)) const;
    
    /**
     * Get recent memory alerts
     * @param count Number of recent alerts to retrieve
     * @return Vector of memory alerts
     */
    std::vector<MemoryAlert> getRecentMemoryAlerts(size_t count = 50) const;
    
    /**
     * Add allocation callback
     * @param callback Function to call when allocation is recorded
     */
    void addAllocationCallback(const AllocationCallback& callback);
    
    /**
     * Add memory alert callback
     * @param callback Function to call when memory alert is triggered
     */
    void addMemoryAlertCallback(const MemoryAlertCallback& callback);
    
    /**
     * Add memory pool callback
     * @param callback Function to call when pool metrics are updated
     */
    void addMemoryPoolCallback(const MemoryPoolCallback& callback);
    
    /**
     * Get memory optimization suggestions
     * @return Vector of optimization suggestion strings
     */
    std::vector<std::string> getMemoryOptimizationSuggestions() const;
    
    /**
     * Force garbage collection (if supported)
     */
    void forceGarbageCollection();
    
    /**
     * Get current memory efficiency rating
     * @return Efficiency rating (0.0 to 1.0)
     */
    double getMemoryEfficiencyRating() const;

private:
    // Thread management
    void memoryMonitoringThreadFunc();
    
    // Memory leak detection
    void checkForMemoryLeaks();
    
    // Fragmentation detection
    void checkMemoryFragmentation();
    
    // Health checks
    void performMemoryHealthChecks();
    
    // Statistics calculation
    double calculateAllocationEfficiency(const MemoryPoolMetrics& pool) const;
    
    // Internal data structures
    mutable std::shared_mutex dataMutex_;
    std::atomic<bool> running_{false};
    
    bool enableLeakDetection_;
    bool enableFragmentationDetection_;
    std::chrono::milliseconds checkInterval_;
    
    // Memory tracking
    struct AllocationTracker {
        std::chrono::high_resolution_clock::time_point timestamp;
        size_t size;
        std::string type;
        std::string context;
        std::vector<std::string> callStack;
        bool isDeallocated;
    };
    
    std::deque<AllocationTracker> allocationHistory_;
    std::vector<MemoryAllocationRecord> allocationRecords_;
    
    // Pool tracking
    std::unordered_map<std::string, MemoryPoolMetrics> poolMetrics_;
    
    // Current memory stats
    CurrentMemoryStats currentStats_;
    
    // Leak detection
    std::unordered_map<std::string, MemoryLeakInfo> memoryLeaks_;
    std::unordered_map<std::string, size_t> allocationCounts_;
    
    // Fragmentation info
    MemoryFragmentationInfo fragmentationInfo_;
    
    // Alert tracking
    std::vector<MemoryAlert> memoryAlerts_;
    
    // Callbacks
    std::vector<AllocationCallback> allocationCallbacks_;
    std::vector<MemoryAlertCallback> memoryAlertCallbacks_;
    std::vector<MemoryPoolCallback> memoryPoolCallbacks_;
    
    // Background thread
    std::thread monitoringThread_;
    std::condition_variable stopCondition_;
    std::mutex stopMutex_;
};

} // namespace nlm