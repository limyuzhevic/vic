#pragma once

/**
 * Memory Optimizer Module
 * 
 * Advanced memory management for neural network simulation with:
 * - Memory pool improvements
 * - Allocation efficiency
 * - Cache-friendly data structures
 * - Memory usage monitoring
 * - Memory leak detection
 * 
 * Features:
 * - Optimized memory pools for neuron/synapse objects
 * - Adaptive allocation strategies
 * - Memory usage tracking
 * - Automatic memory leak detection
 * - Cache alignment for better performance
 */

#ifndef NLM_MEMORY_OPTIMIZER_HPP
#define NLM_MEMORY_OPTIMIZER_HPP

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <thread>
#include <algorithm>

#include "Performance.hpp"

namespace nlm {

/**
 * Memory allocation statistics
 */
struct AllocationStats {
    size_t totalAllocations = 0;
    size_t totalDeallocations = 0;
    size_t currentAllocations = 0;
    size_t peakAllocations = 0;
    size_t totalBytesAllocated = 0;
    size_t peakBytesAllocated = 0;
    std::chrono::steady_clock::time_point lastAllocation;
};

/**
 * Cache line size for alignment optimization
 */
static constexpr size_t CACHE_LINE_SIZE = 64;
static constexpr size_t CACHE_LINE_ALIGN = CACHE_LINE_SIZE - 1;

/**
 * Memory leak detection record
 */
struct MemoryLeakInfo {
    void* address;
    size_t size;
    std::string allocationSite;
    std::chrono::steady_clock::time_point allocationTime;
    std::chrono::steady_clock::time_point lastAccessTime;
    std::unordered_set<void*> dependentAllocations;
};

/**
 * Memory pool statistics
 */
struct PoolStats {
    std::string poolName;
    size_t objectSize;
    size_t blockSize;
    size_t allocatedObjects;
    size_t availableObjects;
    size_t totalMemoryUsage;
    size_t allocationCount;
    double efficiencyScore;
};

/**
 * Advanced memory optimizer implementing:
 * - Memory pool improvements
 * - Allocation efficiency
 * - Cache-friendly data structures
 * - Memory usage monitoring
 * - Memory leak detection
 */
class MemoryOptimizer {
public:
    /**
     * Construct memory optimizer with configuration
     * @param enablePoolOptimization Enable memory pool optimizations
     * @param enableLeakDetection Enable memory leak detection
     * @param monitoringInterval Monitoring interval in milliseconds
     */
    explicit MemoryOptimizer(bool enablePoolOptimization = true,
                           bool enableLeakDetection = true,
                           uint64_t monitoringInterval = 1000);
    
    ~MemoryOptimizer();
    
    // Disable copying and moving
    MemoryOptimizer(const MemoryOptimizer&) = delete;
    MemoryOptimizer& operator=(const MemoryOptimizer&) = delete;
    MemoryOptimizer(MemoryOptimizer&&) = delete;
    MemoryOptimizer& operator=(MemoryOptimizer&&) = delete;
    
    /**
     * Initialize memory optimization system
     * @return true if initialization succeeded
     */
    bool initialize();
    
    /**
     * Finalize memory optimization system
     */
    void finalize();
    
    /**
     * Allocate memory with optimization
     * @param size Number of bytes to allocate
     * @param alignment Memory alignment requirement
     * @param sourceLocation Source location information
     * @return Pointer to allocated memory
     */
    void* allocate(size_t size, size_t alignment = CACHE_LINE_SIZE,
                  const char* sourceLocation = nullptr);
    
    /**
     * Deallocate memory
     * @param ptr Pointer to memory to deallocate
     * @param size Size of memory being deallocated
     */
    void deallocate(void* ptr, size_t size);
    
    /**
     * Record allocation for leak detection
     * @param ptr Pointer to allocated memory
     * @param size Size of allocation
     * @param sourceLocation Source location information
     */
    void recordAllocation(void* ptr, size_t size, const char* sourceLocation);
    
    /**
     * Record deallocation for leak detection
     * @param ptr Pointer to memory being deallocated
     */
    void recordDeallocation(void* ptr);
    
    /**
     * Check for memory leaks
     * @param[out] leaks List of detected memory leaks
     * @return Number of memory leaks detected
     */
    size_t checkMemoryLeaks(std::vector<MemoryLeakInfo>& leaks);
    
    /**
     * Get current memory usage statistics
     * @return Current memory usage in bytes
     */
    size_t getMemoryUsage() const;
    
    /**
     * Get peak memory usage
     * @return Peak memory usage in bytes
     */
    size_t getPeakMemoryUsage() const;
    
    /**
     * Get allocation statistics
     * @return Allocation statistics
     */
    AllocationStats getAllocationStats() const;
    
    /**
     * Get pool statistics
     * @return Vector of pool statistics
     */
    std::vector<PoolStats> getPoolStats() const;
    
    /**
     * Optimize memory pools for specific types
     * @param objectSize Size of objects to optimize pool for
     * @param expectedUsage Expected usage count
     */
    void optimizePoolsForType(size_t objectSize, size_t expectedUsage);
    
    /**
     * Run garbage collection to free unused allocations
     * @return Number of allocations collected
     */
    size_t garbageCollect();
    
    /**
     * Start memory monitoring
     */
    void startMonitoring();
    
    /**
     * Stop memory monitoring
     */
    void stopMonitoring();
    
    /**
     * Generate memory optimization report
     * @return Formatted optimization report
     */
    std::string generateReport() const;
    
    /**
     * Get current cache-friendly allocation strategy
     * @return Recommended cache line alignment
     */
    static size_t getRecommendedAlignment(size_t size);
    
    /**
     * Pre-allocate memory for known patterns
     * @param pattern Pattern identifier
     * @param size Size to pre-allocate
     * @param count Number of allocations to pre-create
     */
    void preAllocatePattern(const std::string& pattern, size_t size, size_t count);
    
    /**
     * Check memory fragmentation level
     * @return Fragmentation score (0.0 to 1.0, higher = more fragmented)
     */
    double checkMemoryFragmentation() const;

private:
    // Internal helper classes and methods
    struct AllocationRecord {
        void* address;
        size_t size;
        std::string sourceLocation;
        std::chrono::steady_clock::time_point time;
        bool isDeallocated;
        std::unordered_set<void*> dependents;
    };
    
    struct MemoryBlock {
        void* address;
        size_t size;
        size_t alignment;
        std::string sourceLocation;
    };
    
    // Thread-safe memory pool implementation
    template<typename T>
    class ThreadSafePool {
    public:
        ThreadSafePool(size_t objectSize, size_t blockSize = 1024)
            : objectSize_(objectSize), blockSize_(blockSize), allocatedCount_(0) {
            cacheLineAlign();
        }
        
        T* allocate() {
            std::lock_guard<std::mutex> lock(mutex_);
            if (freeList_.empty()) {
                addBlock();
            }
            
            T* obj = freeList_.back();
            freeList_.pop_back();
            allocatedCount_++;
            return obj;
        }
        
        void deallocate(T* ptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            freeList_.push_back(ptr);
            allocatedCount_--;
        }
        
        size_t allocatedCount() const { return allocatedCount_; }
        size_t availableCount() const { return freeList_.size(); }
        
    private:
        void cacheLineAlign() {
            if (objectSize_ % CACHE_LINE_SIZE != 0) {
                objectSize_ = ((objectSize_ / CACHE_LINE_SIZE) + 1) * CACHE_LINE_SIZE;
            }
        }
        
        void addBlock() {
            // Allocate a block of objects
            size_t blockSize = blockSize_ * sizeof(T);
            std::vector<uint8_t> block(blockSize);
            
            // Add all objects to free list
            for (size_t i = 0; i < blockSize_ - 1; i++) {
                T* obj = reinterpret_cast<T*>(block.data()) + i;
                freeList_.push_back(obj);
            }
            
            // Keep the last object (will be used for next block if needed)
            // Note: This implementation stores blocks in vectors
            // A real implementation would use more sophisticated pooling
        }
        
        std::mutex mutex_;
        std::vector<T*> freeList_;
        size_t objectSize_;
        size_t blockSize_;
        size_t allocatedCount_;
    };
    
    // Monitoring thread
    void monitoringThreadFunc();
    
    // Update allocation statistics
    void updateAllocationStats(size_t size, bool isAllocation);
    
    // Cache-friendly allocation alignment
    static size_t alignToCacheLine(size_t size);
    
    // Memory leak detection
    void detectMemoryLeaks();
    
    // Generate allocation pattern
    std::string generateAllocationPattern(size_t size) const;
    
    // State variables
    bool initialized_;
    bool poolOptimizationEnabled_;
    bool leakDetectionEnabled_;
    uint64_t monitoringInterval_;
    mutable std::mutex mutex_;
    AllocationStats allocationStats_;
    std::vector<AllocationRecord> allocationRecords_;
    std::unordered_map<void*, AllocationRecord*> activeAllocations_;
    std::unordered_set<void*> deallocatedAllocations_;
    std::vector<std::unique_ptr<ThreadSafePool<void>>> pools_;
    std::thread monitoringThread_;
    std::atomic<bool> monitoringActive_;
    std::chrono::steady_clock::time_point lastLeakCheck_;
    std::vector<MemoryLeakInfo> detectedLeaks_;
};

} // namespace nlm

#endif // NLM_MEMORY_OPTIMIZER_HPP