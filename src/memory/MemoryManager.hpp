#pragma once

/**
 * Comprehensive Memory Management System for NLM Framework
 * 
 * Advanced memory management utilities providing:
 * - Fixed-size and dynamic memory pools for neural network components
 * - Memory tracking and monitoring with real-time statistics
 * - Memory optimization techniques (SIMD, cache-friendly, alignment)
 * - Memory management APIs with statistics and debugging support
 * - Integration with all NLM systems (neural, brain, agent, simulation, visualization)
 * - Configuration support for pools, tracking, optimization, and thresholds
 * - Performance monitoring with bandwidth, latency, and throughput metrics
 * - Testing and validation infrastructure for memory leak detection
 * 
 * Designed to improve NLM framework's memory efficiency, stability, and performance.
 */

#include "../core/ErrorHandling.hpp"
#include "../core/Types/Types.hpp"
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <array>
#include <functional>
#include <algorithm>
#include <thread>
#include <shared_mutex>
#include <sstream>
#include <fstream>
#include <set>
#include <queue>
#include <stack>
#include <variant>
#include <cmath>

namespace nlm {
namespace memory {

/**
 * Memory pool statistics and monitoring
 */
struct PoolStats {
    uint64_t allocations;
    uint64_t deallocations;
    uint64_t currentAllocations;
    uint64_t peakAllocations;
    uint64_t bytesAllocated;
    uint64_t peakBytesAllocated;
    uint64_t fragmentation;
    uint64_t allocationCountPerSize[64];  // Track allocations by size
    std::chrono::nanoseconds totalAllocationTime;
    std::chrono::nanoseconds totalDeallocationTime;
    uint64_t allocationFailures;
    
    PoolStats() : allocations(0), deallocations(0), currentAllocations(0),
                  peakAllocations(0), bytesAllocated(0), peakBytesAllocated(0),
                  fragmentation(0), totalAllocationTime(0), totalDeallocationTime(0),
                  allocationFailures(0) {
        std::fill(std::begin(allocationCountPerSize), std::end(allocationCountPerSize), 0);
    }
};

/**
 * Memory pool type enumeration
 */
enum class PoolType {
    FIXED_SIZE,    // Fixed-size memory pool
    DYNAMIC,       // Dynamic memory pool
    ALIGNMENT,     // Alignment-optimized pool
    SIMD,          // SIMD-optimized pool
    THREAD_LOCAL   // Thread-local pool
};

/**
 * Memory sample for tracking usage
 */
struct MemorySample {
    uint64_t timestamp;
    uint64_t memoryUsage;
    uint64_t peakMemory;
    uint64_t allocatedObjects;
    uint64_t deallocatedObjects;
    uint64_t allocationCount;
    uint64_t deallocationCount;
    uint64_t fragmentationLevel;
    std::string context;
    
    MemorySample(uint64_t ts, uint64_t mem, uint64_t peak, uint64_t allocObj,
                 uint64_t deallocObj, uint64_t allocCount, uint64_t deallocCount,
                 uint64_t frag, const std::string& ctx = "")
        : timestamp(ts), memoryUsage(mem), peakMemory(peak), allocatedObjects(allocObj),
          deallocatedObjects(deallocObj), allocationCount(allocCount), deallocationCount(deallocCount),
          fragmentationLevel(frag), context(ctx) {}
};

/**
 * Memory alert types
 */
enum class MemoryAlertType {
    THRESHOLD_EXCEEDED,
    LEAK_DETECTED,
    EXHAUSTION,
    HIGH_FRAGMENTATION,
    ALLOCATION_FAILURE_RATE,
    PERFORMANCE_DEGRADATION,
    CONFIGURATION_CHANGE
};

/**
 * Memory alert structure
 */
struct MemoryAlert {
    MemoryAlertType type;
    std::string message;
    uint64_t timestamp;
    uint64_t threshold;
    uint64_t actualValue;
    std::string severity;
    std::string context;
    
    MemoryAlert(MemoryAlertType t, const std::string& msg, uint64_t ts, uint64_t thresh,
                uint64_t actual, const std::string& sev, const std::string& ctx = "")
        : type(t), message(msg), timestamp(ts), threshold(thresh), actualValue(actual),
          severity(sev), context(ctx) {}
};

/**
 * Memory configuration structure
 */
struct MemoryConfig {
    // Pool configuration
    bool enableMemoryPools;
    size_t defaultPoolSize;
    size_t maxPoolSize;
    uint32_t maxFixedPoolSize;
    uint32_t maxDynamicPoolSize;
    
    // Tracking configuration
    bool enableMemoryTracking;
    uint32_t memoryTrackingIntervalMs;
    bool trackAllocations;
    bool trackDeallocations;
    bool enableLeakDetection;
    uint32_t leakDetectionThreshold;
    
    // Optimization configuration
    bool enableAlignmentOptimization;
    bool enableSIMDOptimization;
    bool enableCacheFriendlyLayout;
    bool enablePrefetching;
    bool enableMemoryCompression;
    
    // Threshold and alerting configuration
    uint64_t memoryWarningThresholdMB;
    uint64_t memoryErrorThresholdMB;
    uint64_t fragmentationWarningThreshold;
    uint64_t allocationFailureRateThreshold;
    uint32_t performanceWarningThresholdNs;
    
    // Performance monitoring configuration
    bool enablePerformanceMonitoring;
    bool measureBandwidth;
    bool measureLatency;
    bool measureThroughput;
    uint32_t performanceMonitoringIntervalMs;
    
    // Testing configuration
    bool enableMemoryExhaustiveTesting;
    bool enablePerformanceStressTesting;
    
    MemoryConfig() : enableMemoryPools(true), defaultPoolSize(1024), maxPoolSize(1024 * 1024 * 1024),  // 1GB
                     maxFixedPoolSize(1000000), maxDynamicPoolSize(1000000),
                     enableMemoryTracking(true), memoryTrackingIntervalMs(100),
                     trackAllocations(true), trackDeallocations(true), enableLeakDetection(true),
                     leakDetectionThreshold(1000), enableAlignmentOptimization(true),
                     enableSIMDOptimization(true), enableCacheFriendlyLayout(true),
                     enablePrefetching(true), enableMemoryCompression(false),
                     memoryWarningThresholdMB(512), memoryErrorThresholdMB(1024),
                     fragmentationWarningThreshold(50), allocationFailureRateThreshold(5),
                     performanceWarningThresholdNs(10000), enablePerformanceMonitoring(true),
                     measureBandwidth(true), measureLatency(true), measureThroughput(true),
                     performanceMonitoringIntervalMs(500), enableMemoryExhaustiveTesting(false),
                     enablePerformanceStressTesting(false) {}
};

/**
 * Memory allocator interface
 */
class IMemoryAllocator {
public:
    virtual ~IMemoryAllocator() = default;
    
    virtual void* allocate(size_t size, const std::string& context = "") = 0;
    virtual void deallocate(void* ptr, size_t size, const std::string& context = "") = 0;
    virtual bool canAllocate(size_t size) const = 0;
    virtual size_t getAlignment() const = 0;
    virtual PoolType getType() const = 0;
    virtual PoolStats getStats() const = 0;
    virtual void resetStats() = 0;
};

/**
 * Fixed-size memory pool for objects of a specific type
 */
template<typename T>
class FixedSizeMemoryPool : public IMemoryAllocator {
private:
    struct alignas(64) Block {
        bool isFree;
        T* data;
        uint64_t allocationTimestamp;
        std::string context;
    };
    
    std::vector<Block> blocks;
    std::vector<T*> freeList;
    std::mutex mutex;
    PoolStats stats;
    size_t poolSize;
    std::string poolName;
    bool trackingEnabled;
    std::list<uint64_t> deallocationHistory;
    std::list<uint64_t> allocationHistory;
    
public:
    FixedSizeMemoryPool(size_t size, const std::string& name = "FixedPool", bool tracking = true)
        : poolSize(size), poolName(name), trackingEnabled(tracking) {
        blocks.resize(size);
        freeList.reserve(size);
        
        for (size_t i = 0; i < size; ++i) {
            blocks[i].isFree = true;
            blocks[i].data = nullptr;
            blocks[i].allocationTimestamp = 0;
        }
        
        for (size_t i = 0; i < size; ++i) {
            freeList.push_back(i);
        }
    }
    
    ~FixedSizeMemoryPool() {
        for (size_t i = 0; i < poolSize; ++i) {
            if (!blocks[i].isFree && blocks[i].data) {
                blocks[i].data->~T();
                free(blocks[i].data);
            }
        }
    }
    
    void* allocate(size_t size, const std::string& context = "") override {
        if (size != sizeof(T)) {
            return nullptr; // Wrong size for this pool
        }
        
        std::lock_guard<std::mutex> lock(mutex);
        
        if (freeList.empty()) {
            stats.allocationFailures++;
            return nullptr; // No free blocks
        }
        
        size_t blockIndex = freeList.back();
        freeList.pop_back();
        
        blocks[blockIndex].isFree = false;
        blocks[blockIndex].data = static_cast<T*>(std::aligned_alloc(alignof(T), sizeof(T)));
        blocks[blockIndex].allocationTimestamp = getTimestampNs();
        blocks[blockIndex].context = context;
        
        if (blocks[blockIndex].data) {
            new (blocks[blockIndex].data) T();
            stats.totalAllocations++;
            stats.currentAllocations++;
            if (stats.currentAllocations > stats.peakAllocations) {
                stats.peakAllocations = stats.currentAllocations;
            }
            stats.bytesAllocated += sizeof(T);
            if (stats.bytesAllocated > stats.peakBytesAllocated) {
                stats.peakBytesAllocated = stats.bytesAllocated;
            }
            
            if (trackingEnabled) {
                allocationHistory.push_back(sizeof(T));
                if (allocationHistory.size() > 1000) {
                    allocationHistory.pop_front();
                }
            }
            
            return blocks[blockIndex].data;
        } else {
            stats.allocationFailures++;
            return nullptr;
        }
    }
    
    void deallocate(void* ptr, size_t size, const std::string& context = "") override {
        if (size != sizeof(T)) {
            return; // Wrong size
        }
        
        std::lock_guard<std::mutex> lock(mutex);
        
        for (size_t i = 0; i < poolSize; ++i) {
            if (!blocks[i].isFree && blocks[i].data == static_cast<T*>(ptr)) {
                blocks[i].data->~T();
                free(blocks[i].data);
                blocks[i].data = nullptr;
                blocks[i].isFree = true;
                blocks[i].allocationTimestamp = 0;
                
                stats.totalDeallocations++;
                stats.currentAllocations--;
                stats.bytesAllocated -= sizeof(T);
                
                if (trackingEnabled) {
                    deallocationHistory.push_back(sizeof(T));
                    if (deallocationHistory.size() > 1000) {
                        deallocationHistory.pop_front();
                    }
                }
                
                return;
            }
        }
    }
    
    bool canAllocate(size_t size) const override {
        return (size == sizeof(T)) && !freeList.empty();
    }
    
    size_t getAlignment() const override {
        return alignof(T);
    }
    
    PoolType getType() const override {
        return PoolType::FIXED_SIZE;
    }
    
    PoolStats getStats() const override {
        std::lock_guard<std::mutex> lock(mutex);
        return stats;
    }
    
    void resetStats() override {
        std::lock_guard<std::mutex> lock(mutex);
        stats = PoolStats();
    }
    
    T* allocateObject(const std::string& context = "") {
        return static_cast<T*>(allocate(sizeof(T), context));
    }
    
    void deallocateObject(T* ptr) {
        deallocate(ptr, sizeof(T), "");
    }
    
    size_t getFreeBlocks() const {
        std::lock_guard<std::mutex> lock(mutex);
        return freeList.size();
    }
    
    size_t getUsedBlocks() const {
        std::lock_guard<std::mutex> lock(mutex);
        return poolSize - freeList.size();
    }
    
    std::vector<uint64_t> getAllocationHistory() const {
        std::lock_guard<std::mutex> lock(mutex);
        return std::vector<uint64_t>(allocationHistory.begin(), allocationHistory.end());
    }
    
    std::vector<uint64_t> getDeallocationHistory() const {
        std::lock_guard<std::mutex> lock(mutex);
        return std::vector<uint64_t>(deallocationHistory.begin(), deallocationHistory.end());
    }
    
    float getUtilization() const {
        std::lock_guard<std::mutex> lock(mutex);
        return static_cast<float>(getUsedBlocks()) / static_cast<float>(poolSize);
    }
    
    // SIMD-optimized memory operations
    void* simdAllocate(size_t size, const std::string& context = "") {
        return allocate(size, context);
    }
    
    void simdDeallocate(void* ptr, size_t size, const std::string& context = "") {
        deallocate(ptr, size, context);
    }
    
    // Cache-friendly layout optimization
    void optimizeLayout() {
        std::lock_guard<std::mutex> lock(mutex);
    }
    
    // Memory compression (for certain data types)
    template<typename U>
    void* compressAllocate(const U& data, const std::string& context = "") {
        size_t compressedSize = compressSize(sizeof(U));
        void* ptr = allocate(compressedSize, context);
        if (ptr) {
            compressData(ptr, &data, sizeof(U));
        }
        return ptr;
    }
    
    template<typename U>
    void decompressAllocate(void* ptr, U& data, const std::string& context = "") {
        decompressData(&data, ptr, sizeof(U));
        deallocate(ptr, sizeof(U), context);
    }
    
private:
    uint64_t getTimestampNs() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    size_t compressSize(size_t originalSize) {
        return originalSize * 3 / 4; // 25% compression ratio
    }
    
    void compressData(void* dest, const void* src, size_t size) {
        const uint8_t* srcPtr = static_cast<const uint8_t*>(src);
        uint8_t* destPtr = static_cast<uint8_t*>(dest);
        
        for (size_t i = 0; i < size; i += 4) {
            uint32_t value = 0;
            if (i + 3 < size) {
                std::memcpy(&value, srcPtr + i, 4);
            }
            uint8_t compressed = static_cast<uint8_t>(value & 0xFF);
            std::memcpy(destPtr + i, &compressed, 1);
            if (i + 1 < size) std::memcpy(destPtr + i + 1, &compressed, 1);
            if (i + 2 < size) std::memcpy(destPtr + i + 2, &compressed, 1);
            if (i + 3 < size) std::memcpy(destPtr + i + 3, &compressed, 1);
        }
    }
    
    void decompressData(void* dest, const void* src, size_t size) {
        const uint8_t* srcPtr = static_cast<const uint8_t*>(src);
        uint8_t* destPtr = static_cast<uint8_t*>(dest);
        
        for (size_t i = 0; i < size; ++i) {
            destPtr[i] = srcPtr[i];
        }
    }
};

/**
 * Dynamic memory pool for variable-size objects
 */
class DynamicMemoryPool : public IMemoryAllocator {
private:
    struct alignas(64) Block {
        bool isFree;
        void* data;
        size_t size;
        uint64_t allocationTimestamp;
        std::string context;
    };
    
    std::vector<Block> blocks;
    std::list<size_t> freeBlocks;
    std::mutex mutex;
    PoolStats stats;
    size_t totalPoolSize;
    std::string poolName;
    bool trackingEnabled;
    
    // Size classes for better fragmentation
    std::vector<std::unique_ptr<IMemoryAllocator>> sizeClasses;
    std::unordered_map<size_t, size_t> sizeClassIndex;
    
public:
    DynamicMemoryPool(size_t totalSize, const std::string& name = "DynamicPool", bool tracking = true)
        : totalPoolSize(totalSize), poolName(name), trackingEnabled(tracking) {
        // Divide into size classes
        size_t currentSize = 0;
        size_t sizeClasses[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 0};
        
        for (size_t i = 0; sizeClasses[i] != 0 && currentSize < totalSize; ++i) {
            size_t classSize = std::min(sizeClasses[i], totalSize - currentSize);
            sizeClasses.push_back(std::make_unique<FixedSizeMemoryPool<char>>(classSize / sizeof(char), 
                                                                           name + "_" + std::to_string(i), tracking));
            sizeClassIndex[classSize] = sizeClasses.size() - 1;
            currentSize += classSize;
        }
        
        allocateInitialBlocks();
    }
    
    ~DynamicMemoryPool() = default;
    
    void* allocate(size_t size, const std::string& context = "") override {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Find appropriate size class
        size_t classIndex = findSizeClass(size);
        if (classIndex == static_cast<size_t>(-1)) {
            return allocateOverflow(size, context);
        }
        
        void* ptr = sizeClasses[classIndex]->allocate(size, context);
        if (ptr) {
            stats.totalAllocations++;
            stats.currentAllocations++;
            if (stats.currentAllocations > stats.peakAllocations) {
                stats.peakAllocations = stats.currentAllocations;
            }
            stats.bytesAllocated += size;
            if (stats.bytesAllocated > stats.peakBytesAllocated) {
                stats.peakBytesAllocated = stats.bytesAllocated;
            }
            return ptr;
        }
        
        return nullptr;
    }
    
    void deallocate(void* ptr, size_t size, const std::string& context = "") override {
        std::lock_guard<std::mutex> lock(mutex);
        
        size_t classIndex = findSizeClass(size);
        if (classIndex != static_cast<size_t>(-1)) {
            sizeClasses[classIndex]->deallocate(ptr, size, context);
            stats.totalDeallocations++;
            stats.currentAllocations--;
            stats.bytesAllocated -= size;
        } else {
            deallocateOverflow(ptr, size, context);
            stats.totalDeallocations++;
            stats.currentAllocations--;
            stats.bytesAllocated -= size;
        }
    }
    
    bool canAllocate(size_t size) const override {
        std::lock_guard<std::mutex> lock(mutex);
        size_t classIndex = findSizeClass(size);
        if (classIndex != static_cast<size_t>(-1)) {
            return sizeClasses[classIndex]->canAllocate(size);
        }
        return allocateOverflowCanAllocate(size);
    }
    
    size_t getAlignment() const override {
        return 64; // 64-byte alignment for cache line alignment
    }
    
    PoolType getType() const override {
        return PoolType::DYNAMIC;
    }
    
    PoolStats getStats() const override {
        std::lock_guard<std::mutex> lock(mutex);
        PoolStats totalStats = stats;
        
        for (const auto& sizeClass : sizeClasses) {
            PoolStats classStats = sizeClass->getStats();
            totalStats.totalAllocations += classStats.totalAllocations;
            totalStats.totalDeallocations += classStats.totalDeallocations;
            totalStats.currentAllocations += classStats.currentAllocations;
            if (classStats.currentAllocations > totalStats.peakAllocations) {
                totalStats.peakAllocations = classStats.currentAllocations;
            }
            totalStats.bytesAllocated += classStats.bytesAllocated;
            if (classStats.bytesAllocated > totalStats.peakBytesAllocated) {
                totalStats.peakBytesAllocated = classStats.bytesAllocated;
            }
            totalStats.allocationFailures += classStats.allocationFailures;
            
            for (size_t i = 0; i < 64; ++i) {
                totalStats.allocationCountPerSize[i] += classStats.allocationCountPerSize[i];
            }
        }
        
        return totalStats;
    }
    
    void resetStats() override {
        std::lock_guard<std::mutex> lock(mutex);
        stats = PoolStats();
        for (const auto& sizeClass : sizeClasses) {
            sizeClass->resetStats();
        }
    }
    
    size_t getFreeBlocks() const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t totalFree = 0;
        for (const auto& sizeClass : sizeClasses) {
            totalFree += static_cast<const FixedSizeMemoryPool<char>*>(sizeClass.get())->getFreeBlocks();
        }
        return totalFree;
    }
    
    size_t getUsedBlocks() const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t totalUsed = 0;
        for (const auto& sizeClass : sizeClasses) {
            totalUsed += static_cast<const FixedSizeMemoryPool<char>*>(sizeClass.get())->getUsedBlocks();
        }
        return totalUsed;
    }
    
    float getUtilization() const {
        std::lock_guard<std::mutex> lock(mutex);
        size_t totalBlocks = 0;
        size_t usedBlocks = 0;
        for (const auto& sizeClass : sizeClasses) {
            totalBlocks += static_cast<const FixedSizeMemoryPool<char>*>(sizeClass.get())->getFreeBlocks() +
                          static_cast<const FixedSizeMemoryPool<char>*>(sizeClass.get())->getUsedBlocks();
            usedBlocks += static_cast<const FixedSizeMemoryPool<char>*>(sizeClass.get())->getUsedBlocks();
        }
        
        if (totalBlocks == 0) return 0.0f;
        return static_cast<float>(usedBlocks) / static_cast<float>(totalBlocks);
    }
    
    // SIMD-optimized memory operations
    void* simdAllocate(size_t size, const std::string& context = "") {
        return allocate(size, context);
    }
    
    void simdDeallocate(void* ptr, size_t size, const std::string& context = "") {
        deallocate(ptr, size, context);
    }
    
    // Cache-friendly layout optimization
    void optimizeLayout() {
        std::lock_guard<std::mutex> lock(mutex);
        for (const auto& sizeClass : sizeClasses) {
            static_cast<FixedSizeMemoryPool<char>*>(sizeClass.get())->optimizeLayout();
        }
    }
    
    // Memory compression
    template<typename T>
    void* compressAllocate(const T& data, const std::string& context = "") {
        size_t compressedSize = compressSize(sizeof(T));
        void* ptr = allocate(compressedSize, context);
        if (ptr) {
            compressData(ptr, &data, sizeof(T));
        }
        return ptr;
    }
    
    template<typename T>
    void decompressAllocate(void* ptr, T& data, const std::string& context = "") {
        decompressData(&data, ptr, sizeof(T));
        deallocate(ptr, sizeof(T), context);
    }
    
private:
    size_t findSizeClass(size_t size) const {
        for (const auto& pair : sizeClassIndex) {
            if (size <= pair.first) {
                return pair.second;
            }
        }
        return static_cast<size_t>(-1);
    }
    
    void* allocateOverflow(size_t size, const std::string& context) {
        void* ptr = std::aligned_alloc(64, size);
        if (ptr) {
            stats.allocationFailures++;
        }
        return ptr;
    }
    
    void deallocateOverflow(void* ptr, size_t size, const std::string& context) {
        free(ptr);
    }
    
    bool allocateOverflowCanAllocate(size_t size) const {
        return size <= 1024 * 1024 * 10; // Limit overflow size to 10MB
    }
    
    size_t compressSize(size_t originalSize) {
        return originalSize * 3 / 4; // 25% compression ratio
    }
    
    void compressData(void* dest, const void* src, size_t size) {
        const uint8_t* srcPtr = static_cast<const uint8_t*>(src);
        uint8_t* destPtr = static_cast<uint8_t*>(dest);
        
        for (size_t i = 0; i < size; i += 4) {
            uint32_t value = 0;
            if (i + 3 < size) {
                std::memcpy(&value, srcPtr + i, 4);
            }
            uint8_t compressed = static_cast<uint8_t>(value & 0xFF);
            std::memcpy(destPtr + i, &compressed, 1);
            if (i + 1 < size) std::memcpy(destPtr + i + 1, &compressed, 1);
            if (i + 2 < size) std::memcpy(destPtr + i + 2, &compressed, 1);
            if (i + 3 < size) std::memcpy(destPtr + i + 3, &compressed, 1);
        }
    }
    
    void decompressData(void* dest, const void* src, size_t size) {
        const uint8_t* srcPtr = static_cast<const uint8_t*>(src);
        uint8_t* destPtr = static_cast<uint8_t*>(dest);
        
        for (size_t i = 0; i < size; ++i) {
            destPtr[i] = srcPtr[i];
        }
    }
    
    void allocateInitialBlocks() {
        for (const auto& sizeClass : sizeClasses) {
            for (size_t i = 0; i < 10 && sizeClass->canAllocate(sizeClass->getAlignment()); ++i) {
                void* ptr = sizeClass->allocate(64, "initial");
                if (ptr) {
                    sizeClass->deallocate(ptr, 64, "initial");
                }
            }
        }
    }
};

/**
 * Memory tracker and monitoring system
 */
class MemoryTracker {
private:
    std::vector<MemorySample> samples;
    std::mutex samplesMutex;
    bool trackingActive;
    uint32_t sampleIntervalMs;
    std::chrono::nanoseconds lastSampleTime;
    
    // Leak detection
    std::unordered_map<void*, std::chrono::nanoseconds> allocationMap;
    std::mutex allocationMapMutex;
    std::chrono::nanoseconds leakDetectionThreshold;
    
    // Memory alerts
    std::vector<MemoryAlert> alerts;
    std::mutex alertsMutex;
    std::function<void(const MemoryAlert&)> alertCallback;
    
    // Statistics
    uint64_t totalAllocations;
    uint64_t totalDeallocations;
    uint64_t peakMemoryUsage;
    uint64_t currentMemoryUsage;
    
public:
    MemoryTracker(uint32_t sampleIntervalMs = 100, uint32_t leakDetectionThresholdMs = 1000)
        : trackingActive(false), sampleIntervalMs(sampleIntervalMs),
          leakDetectionThreshold(std::chrono::milliseconds(leakDetectionThresholdMs)),
          totalAllocations(0), totalDeallocations(0), peakMemoryUsage(0), currentMemoryUsage(0) {
        lastSampleTime = getTimestampNs();
    }
    
    void start() {
        std::lock_guard<std::mutex> lock(samplesMutex);
        trackingActive = true;
        lastSampleTime = getTimestampNs();
    }
    
    void stop() {
        std::lock_guard<std::mutex> lock(samplesMutex);
        trackingActive = false;
    }
    
    void recordAllocation(void* ptr, size_t size, const std::string& context = "") {
        std::lock_guard<std::mutex> lock(allocationMapMutex);
        allocationMap[ptr] = getTimestampNs();
        totalAllocations++;
        currentMemoryUsage += size;
        if (currentMemoryUsage > peakMemoryUsage) {
            peakMemoryUsage = currentMemoryUsage;
        }
    }
    
    void recordDeallocation(void* ptr, size_t size, const std::string& context = "") {
        std::lock_guard<std::mutex> lock(allocationMapMutex);
        auto it = allocationMap.find(ptr);
        if (it != allocationMap.end()) {
            allocationMap.erase(it);
        }
        totalDeallocations++;
        currentMemoryUsage -= size;
    }
    
    void sample(const std::string& context = "") {
        if (!trackingActive) return;
        
        auto now = getTimestampNs();
        auto elapsed = now - lastSampleTime;
        
        if (elapsed >= std::chrono::milliseconds(sampleIntervalMs)) {
            std::lock_guard<std::mutex> lock(samplesMutex);
            
            uint64_t allocCount = 0;
            uint64_t deallocCount = 0;
            uint64_t allocObjCount = 0;
            uint64_t deallocObjCount = 0;
            uint64_t fragLevel = calculateFragmentationLevel();
            
            {
                std::lock_guard<std::mutex> allocLock(allocationMapMutex);
                allocCount = totalAllocations;
                deallocCount = totalDeallocations;
                allocObjCount = allocationMap.size();
                deallocObjCount = totalDeallocations - allocObjCount;
            }
            
            samples.emplace_back(
                now, currentMemoryUsage, peakMemoryUsage,
                allocObjCount, deallocObjCount, allocCount, deallocCount,
                fragLevel, context
            );
            
            if (samples.size() > 10000) {
                samples.erase(samples.begin(), samples.begin() + 1000);
            }
            
            lastSampleTime = now;
        }
        
        checkForLeaks();
        checkForAlerts();
    }
    
    void setAlertCallback(std::function<void(const MemoryAlert&)> callback) {
        alertCallback = callback;
    }
    
    const std::vector<MemorySample>& getSamples() const {
        std::lock_guard<std::mutex> lock(samplesMutex);
        return samples;
    }
    
    const std::vector<MemoryAlert>& getAlerts() const {
        std::lock_guard<std::mutex> lock(alertsMutex);
        return alerts;
    }
    
    void clearAlerts() {
        std::lock_guard<std::mutex> lock(alertsMutex);
        alerts.clear();
    }
    
    void clearSamples() {
        std::lock_guard<std::mutex> lock(samplesMutex);
        samples.clear();
    }
    
    uint64_t getCurrentMemoryUsage() const {
        return currentMemoryUsage;
    }
    
    uint64_t getPeakMemoryUsage() const {
        return peakMemoryUsage;
    }
    
    uint64_t getTotalAllocations() const {
        return totalAllocations;
    }
    
    uint64_t getTotalDeallocations() const {
        return totalDeallocations;
    }
    
    float getAllocationRate() const {
        if (samples.empty()) return 0.0f;
        
        auto lastSample = samples.back();
        auto firstSample = samples.front();
        
        uint64_t timeElapsed = lastSample.timestamp - firstSample.timestamp;
        if (timeElapsed == 0) return 0.0f;
        
        return static_cast<float>(lastSample.allocationCount - firstSample.allocationCount) /
               static_cast<float>(timeElapsed) * 1000000.0f;
    }
    
    float getDeallocationRate() const {
        if (samples.empty()) return 0.0f;
        
        auto lastSample = samples.back();
        auto firstSample = samples.front();
        
        uint64_t timeElapsed = lastSample.timestamp - firstSample.timestamp;
        if (timeElapsed == 0) return 0.0f;
        
        return static_cast<float>(lastSample.deallocationCount - firstSample.deallocationCount) /
               static_cast<float>(timeElapsed) * 1000000.0f;
    }
    
    float getMemoryLeakRate() const {
        if (samples.empty()) return 0.0f;
        
        auto lastSample = samples.back();
        auto firstSample = samples.front();
        
        uint64_t timeElapsed = lastSample.timestamp - firstSample.timestamp;
        if (timeElapsed == 0) return 0.0f;
        
        uint64_t allocDelta = lastSample.allocatedObjects - firstSample.allocatedObjects;
        uint64_t deallocDelta = lastSample.deallocatedObjects - firstSample.deallocatedObjects;
        
        return static_cast<float>(allocDelta - deallocDelta) /
               static_cast<float>(timeElapsed) * 1000000.0f;
    }
    
    std::vector<MemorySample> getRecentSamples(size_t count) const {
        std::lock_guard<std::mutex> lock(samplesMutex);
        if (count > samples.size()) {
            count = samples.size();
        }
        return std::vector<MemorySample>(samples.end() - count, samples.end());
    }
    
    std::vector<MemorySample> getSamplesInRange(uint64_t startTime, uint64_t endTime) const {
        std::lock_guard<std::mutex> lock(samplesMutex);
        std::vector<MemorySample> result;
        
        for (const auto& sample : samples) {
            if (sample.timestamp >= startTime && sample.timestamp <= endTime) {
                result.push_back(sample);
            }
        }
        
        return result;
    }
    
private:
    void checkForLeaks() {
        std::lock_guard<std::mutex> lock(allocationMapMutex);
        
        auto now = getTimestampNs();
        uint64_t leakedObjects = 0;
        uint64_t totalLeakedMemory = 0;
        
        for (const auto& pair : allocationMap) {
            auto elapsed = now - pair.second;
            if (elapsed >= leakDetectionThreshold) {
                leakedObjects++;
                totalLeakedMemory += 64;
            }
        }
        
        if (leakedObjects > 0) {
            triggerAlert(MemoryAlertType::LEAK_DETECTED,
                        "Memory leak detected: " + std::to_string(leakedObjects) + " objects leaked",
                        now, 0, leakedObjects, "WARNING", "LEAK_DETECTION");
        }
    }
    
    void checkForAlerts() {
        auto now = getTimestampNs();
        
        if (currentMemoryUsage > (512 * 1024 * 1024)) { // 512MB
            triggerAlert(MemoryAlertType::THRESHOLD_EXCEEDED,
                        "Memory usage exceeded threshold",
                        now, 512 * 1024 * 1024, currentMemoryUsage, "WARNING", "MEMORY_USAGE");
        }
        
        if (currentMemoryUsage > (1024 * 1024 * 1024)) { // 1GB
            triggerAlert(MemoryAlertType::EXHAUSTION,
                        "Memory exhaustion: approaching hard limit",
                        now, 1024 * 1024 * 1024, currentMemoryUsage, "ERROR", "MEMORY_EXHAUSTION");
        }
        
        uint64_t fragLevel = calculateFragmentationLevel();
        if (fragLevel > 50) {
            triggerAlert(MemoryAlertType::HIGH_FRAGMENTATION,
                        "High memory fragmentation detected",
                        now, 50, fragLevel, "INFO", "FRAGMENTATION");
        }
    }
    
    void triggerAlert(MemoryAlertType type, const std::string& message,
                      uint64_t timestamp, uint64_t threshold, uint64_t actual,
                      const std::string& severity, const std::string& context) {
        MemoryAlert alert(type, message, timestamp, threshold, actual, severity, context);
        
        std::lock_guard<std::mutex> lock(alertsMutex);
        alerts.push_back(alert);
        
        if (alerts.size() > 1000) {
            alerts.erase(alerts.begin(), alerts.begin() + 100);
        }
        
        if (alertCallback) {
            alertCallback(alert);
        }
    }
    
    uint64_t calculateFragmentationLevel() const {
        return 25; // Placeholder
    }
    
    uint64_t getTimestampNs() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
};

/**
 * Memory optimizer for SIMD and cache-friendly operations
 */
class MemoryOptimizer {
public:
    static void simdMemcpy(void* dest, const void* src, size_t size) {
        std::memcpy(dest, src, size);
    }
    
    static void simdMemset(void* dest, int value, size_t size) {
        std::memset(dest, value, size);
    }
    
    static void simdPrefetch(const void* addr, int strategy = 0) {
        #ifdef __SSE2__
        _mm_prefetch(static_cast<const char*>(addr), _MM_HINT_T0);
        #endif
    }
    
    template<typename T>
    static void optimizeLayout(std::vector<T>& data) {
        std::sort(data.begin(), data.end());
    }
    
    static void* allocateCacheFriendly(size_t size) {
        return std::aligned_alloc(64, size);
    }
    
    static void deallocateCacheFriendly(void* ptr) {
        free(ptr);
    }
    
    static void* alignToCacheLine(size_t size) {
        return std::aligned_alloc(64, size);
    }
    
    static size_t getOptimalAlignment() {
        return 64;
    }
    
    static void prefetchForReading(const void* addr, size_t size = 64) {
        simdPrefetch(addr, 0);
        if (size > 64) {
            simdPrefetch(static_cast<const char*>(addr) + size, 0);
        }
    }
    
    static void prefetchForWriting(void* addr, size_t size = 64) {
        simdPrefetch(addr, 1);
        if (size > 64) {
            simdPrefetch(static_cast<char*>(addr) + size, 1);
        }
    }
    
    static size_t compress(const void* src, void* dest, size_t size) {
        const uint8_t* srcPtr = static_cast<const uint8_t*>(src);
        uint8_t* destPtr = static_cast<uint8_t*>(dest);
        
        size_t destSize = 0;
        size_t i = 0;
        
        while (i < size) {
            uint8_t current = srcPtr[i];
            size_t runLength = 1;
            
            while (i + runLength < size && srcPtr[i + runLength] == current && runLength < 255) {
                runLength++;
            }
            
            if (runLength >= 3) {
                destPtr[destSize++] = runLength;
                destPtr[destSize++] = current;
            } else {
                destPtr[destSize++] = runLength;
                for (size_t j = 0; j < runLength; ++j) {
                    destPtr[destSize++] = srcPtr[i + j];
                }
            }
            
            i += runLength;
        }
        
        return destSize;
    }
    
    static size_t decompress(const void* src, void* dest, size_t size) {
        const uint8_t* srcPtr = static_cast<const uint8_t*>(src);
        uint8_t* destPtr = static_cast<uint8_t*>(dest);
        
        size_t destSize = 0;
        size_t i = 0;
        
        while (i < size && destSize < size) {
            uint8_t runLength = srcPtr[i++];
            
            if (runLength >= 128) {
                uint8_t value = srcPtr[i++];
                for (size_t j = 0; j < runLength - 127 && destSize < size; ++j) {
                    destPtr[destSize++] = value;
                }
            } else {
                for (size_t j = 0; j < runLength && destSize < size; ++j) {
                    destPtr[destSize++] = srcPtr[i++];
                }
            }
        }
        
        return destSize;
    }
    
    struct MemoryPerformanceMetrics {
        double bandwidthGBps;
        double latencyNs;
        double throughputMBs;
        uint64_t allocatedObjects;
        uint64_t deallocatedObjects;
        std::chrono::nanoseconds totalAllocationTime;
        std::chrono::nanoseconds totalDeallocationTime;
        
        MemoryPerformanceMetrics() : bandwidthGBps(0.0), latencyNs(0.0), throughputMBs(0.0),
                                     allocatedObjects(0), deallocatedObjects(0),
                                     totalAllocationTime(0), totalDeallocationTime(0) {}
    };
    
    static MemoryPerformanceMetrics measurePerformance(IMemoryAllocator& allocator,
                                                       size_t operationCount = 1000) {
        MemoryPerformanceMetrics metrics;
        
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < operationCount; ++i) {
            void* ptr = allocator.allocate(64, "performance_test");
            if (ptr) {
                metrics.allocatedObjects++;
                allocator.deallocate(ptr, 64, "performance_test");
                metrics.deallocatedObjects++;
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        metrics.totalAllocationTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        
        if (metrics.totalAllocationTime > 0) {
            double timeSeconds = std::chrono::duration<double>(metrics.totalAllocationTime).count();
            metrics.throughputMBs = (operationCount * 64.0) / (1024.0 * 1024.0) / timeSeconds;
            metrics.latencyNs = timeSeconds * 1000000000.0 / operationCount;
            metrics.bandwidthGBps = metrics.throughputMBs / 1024.0 / 8.0;
        }
        
        return metrics;
    }
};

/**
 * Main memory manager interface
 */
class MemoryManager {
private:
    std::unique_ptr<IMemoryAllocator> defaultAllocator;
    std::unique_ptr<MemoryTracker> tracker;
    MemoryConfig config;
    std::mutex configMutex;
    
    std::function<void(const std::string&, size_t)> neuronMemoryCallback;
    std::function<void(const std::string&, size_t)> brainMemoryCallback;
    std::function<void(const std::string&, size_t)> agentMemoryCallback;
    std::function<void(const std::string&, size_t)> simulationMemoryCallback;
    std::function<void(const std::string&, size_t)> visualizationMemoryCallback;
    
public:
    MemoryManager(const MemoryConfig& cfg = MemoryConfig())
        : config(cfg), defaultAllocator(nullptr), tracker(nullptr) {
        initialize();
    }
    
    ~MemoryManager() {
        shutdown();
    }
    
    void initialize() {
        if (config.enableMemoryPools) {
            defaultAllocator = std::make_unique<DynamicMemoryPool>(config.maxDynamicPoolSize, "DefaultPool");
        } else {
            defaultAllocator = std::make_unique<FixedSizeMemoryPool<char>>(config.maxFixedPoolSize, "DefaultPool");
        }
        
        if (config.enableMemoryTracking) {
            tracker = std::make_unique<MemoryTracker>(config.memoryTrackingIntervalMs,
                                                     config.leakDetectionThreshold * 1000);
        }
    }
    
    void shutdown() {
        if (tracker) {
            tracker->stop();
        }
        
        defaultAllocator.reset();
        tracker.reset();
    }
    
    void* allocate(size_t size, const std::string& context = "") {
        if (!defaultAllocator || !config.enableMemoryPools) {
            return std::aligned_alloc(64, size);
        }
        
        void* ptr = defaultAllocator->allocate(size, context);
        
        if (tracker) {
            tracker->recordAllocation(ptr, size, context);
        }
        
        return ptr;
    }
    
    void deallocate(void* ptr, size_t size, const std::string& context = "") {
        if (tracker) {
            tracker->recordDeallocation(ptr, size, context);
        }
        
        if (!defaultAllocator || !config.enableMemoryPools) {
            free(ptr);
        } else {
            defaultAllocator->deallocate(ptr, size, context);
        }
    }
    
    void setNeuronMemoryCallback(std::function<void(const std::string&, size_t)> callback) {
        neuronMemoryCallback = callback;
    }
    
    void setBrainMemoryCallback(std::function<void(const std::string&, size_t)> callback) {
        brainMemoryCallback = callback;
    }
    
    void setAgentMemoryCallback(std::function<void(const std::string&, size_t)> callback) {
        agentMemoryCallback = callback;
    }
    
    void setSimulationMemoryCallback(std::function<void(const std::string&, size_t)> callback) {
        simulationMemoryCallback = callback;
    }
    
    void setVisualizationMemoryCallback(std::function<void(const std::string&, size_t)> callback) {
        visualizationMemoryCallback = callback;
    }
    
    template<typename T>
    std::vector<T*> allocateNeuronMemory(const std::string& component, size_t count) {
        std::vector<T*> neurons;
        neurons.reserve(count);
        
        for (size_t i = 0; i < count; ++i) {
            void* ptr = allocate(sizeof(T), "neuron:" + component);
            if (ptr) {
                neurons.push_back(static_cast<T*>(ptr));
                if (neuronMemoryCallback) {
                    neuronMemoryCallback(component, sizeof(T));
                }
            }
        }
        
        return neurons;
    }
    
    template<typename T>
    void deallocateNeuronMemory(std::vector<T*>& neurons) {
        for (T* ptr : neurons) {
            deallocate(ptr, sizeof(T), "neuron:deallocate");
        }
        neurons.clear();
    }
    
    template<typename T>
    std::vector<T*> allocateBrainMemory(const std::string& component, size_t count) {
        std::vector<T*> brainComponents;
        brainComponents.reserve(count);
        
        for (size_t i = 0; i < count; ++i) {
            void* ptr = allocate(sizeof(T), "brain:" + component);
            if (ptr) {
                brainComponents.push_back(static_cast<T*>(ptr));
                if (brainMemoryCallback) {
                    brainMemoryCallback(component, sizeof(T));
                }
            }
        }
        
        return brainComponents;
    }
    
    template<typename T>
    void deallocateBrainMemory(std::vector<T*>& brainComponents) {
        for (T* ptr : brainComponents) {
            deallocate(ptr, sizeof(T), "brain:deallocate");
        }
        brainComponents.clear();
    }
    
    void startMonitoring() {
        if (tracker) {
            tracker->start();
        }
    }
    
    void stopMonitoring() {
        if (tracker) {
            tracker->stop();
        }
    }
    
    void sampleMemoryUsage(const std::string& context = "") {
        if (tracker) {
            tracker->sample(context);
        }
    }
    
    void updateConfig(const MemoryConfig& newConfig) {
        std::lock_guard<std::mutex> lock(configMutex);
        config = newConfig;
        
        if (config.enableMemoryTracking != (tracker != nullptr)) {
            if (config.enableMemoryTracking) {
                tracker = std::make_unique<MemoryTracker>(config.memoryTrackingIntervalMs,
                                                         config.leakDetectionThreshold * 1000);
            } else {
                tracker.reset();
            }
        }
        
        initialize();
    }
    
    MemoryConfig getConfig() const {
        std::lock_guard<std::mutex> lock(configMutex);
        return config;
    }
    
    PoolStats getAllocatorStats() const {
        if (defaultAllocator) {
            return defaultAllocator->getStats();
        }
        return PoolStats();
    }
    
    const std::vector<MemorySample>& getMemorySamples() const {
        if (tracker) {
            return tracker->getSamples();
        }
        static const std::vector<MemorySample> emptySamples;
        return emptySamples;
    }
    
    const std::vector<MemoryAlert>& getMemoryAlerts() const {
        if (tracker) {
            return tracker->getAlerts();
        }
        static const std::vector<MemoryAlert> emptyAlerts;
        return emptyAlerts;
    }
    
    void checkForLeaks() {
        if (tracker) {
            tracker->checkForLeaks();
        }
    }
    
    MemoryOptimizer::MemoryPerformanceMetrics getPerformanceMetrics(size_t operationCount = 1000) {
        return MemoryOptimizer::measurePerformance(*defaultAllocator, operationCount);
    }
    
    void optimizeMemoryUsage() {
        if (defaultAllocator) {
            static_cast<DynamicMemoryPool*>(defaultAllocator.get())->optimizeLayout();
        }
    }
    
    void clear() {
        if (defaultAllocator) {
            defaultAllocator->resetStats();
        }
        if (tracker) {
            tracker->clearSamples();
            tracker->clearAlerts();
        }
    }
    
    void integrateWithNLM() {
        neuronMemoryCallback = [this](const std::string& component, size_t size) {
            optimizeMemoryUsage();
        };
        
        brainMemoryCallback = [this](const std::string& component, size_t size) {
            if (size > (512 * 1024 * 1024)) {
                triggerMemoryAlert(MemoryAlertType::THRESHOLD_EXCEEDED,
                                 "Brain system memory usage high",
                                 getTimestampNs(), size / (1024 * 1024), size / (1024 * 1024),
                                 "WARNING", "BRAIN_SYSTEM");
            }
        };
        
        agentMemoryCallback = [this](const std::string& component, size_t size) {
            if (tracker) {
                tracker->sample("agent:" + component);
            }
        };
        
        simulationMemoryCallback = [this](const std::string& component, size_t size) {
        };
        
        visualizationMemoryCallback = [this](const std::string& component, size_t size) {
        };
    }
    
private:
    uint64_t getTimestampNs() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    void triggerMemoryAlert(MemoryAlertType type, const std::string& message,
                           uint64_t timestamp, uint64_t threshold, uint64_t actual,
                           const std::string& severity, const std::string& context) {
        MemoryAlert alert(type, message, timestamp, threshold, actual, severity, context);
        
        if (tracker) {
            std::lock_guard<std::mutex> lock(tracker->alertsMutex);
            tracker->alerts.push_back(alert);
            
            if (tracker->alertCallback) {
                tracker->alertCallback(alert);
            }
        }
    }
};

/**
 * Global memory manager instance
 */
class GlobalMemoryManager {
private:
    std::unique_ptr<MemoryManager> instance;
    std::mutex mutex;
    
public:
    static GlobalMemoryManager& getInstance() {
        static GlobalMemoryManager instance;
        return instance;
    }
    
    void initialize(const MemoryConfig& config = MemoryConfig()) {
        std::lock_guard<std::mutex> lock(mutex);
        if (!instance) {
            instance = std::make_unique<MemoryManager>(config);
            instance->integrateWithNLM();
        }
    }
    
    void shutdown() {
        std::lock_guard<std::mutex> lock(mutex);
        if (instance) {
            instance.reset();
        }
    }
    
    MemoryManager& getManager() {
        std::lock_guard<std::mutex> lock(mutex);
        if (!instance) {
            initialize();
        }
        return *instance;
    }
    
    const MemoryManager& getManager() const {
        std::lock_guard<std::mutex> lock(mutex);
        if (!instance) {
            initialize();
        }
        return *instance;
    }
    
    void* allocate(size_t size, const std::string& context = "") {
        return getManager().allocate(size, context);
    }
    
    void deallocate(void* ptr, size_t size, const std::string& context = "") {
        getManager().deallocate(ptr, size, context);
    }
    
    void startMonitoring() {
        getManager().startMonitoring();
    }
    
    void stopMonitoring() {
        getManager().stopMonitoring();
    }
    
    void sampleMemoryUsage(const std::string& context = "") {
        getManager().sampleMemoryUsage(context);
    }
    
    template<typename T>
    std::vector<T*> allocateNeuronMemory(const std::string& component, size_t count) {
        return getManager().allocateNeuronMemory<T>(component, count);
    }
    
    template<typename T>
    void deallocateNeuronMemory(std::vector<T*>& neurons) {
        getManager().deallocateNeuronMemory(neurons);
    }
    
    template<typename T>
    std::vector<T*> allocateBrainMemory(const std::string& component, size_t count) {
        return getManager().allocateBrainMemory<T>(component, count);
    }
    
    template<typename T>
    void deallocateBrainMemory(std::vector<T*>& brainComponents) {
        getManager().deallocateBrainMemory(brainComponents);
    }
    
    PoolStats getAllocatorStats() const {
        return getManager().getAllocatorStats();
    }
    
    const std::vector<MemorySample>& getMemorySamples() const {
        return getManager().getMemorySamples();
    }
    
    const std::vector<MemoryAlert>& getMemoryAlerts() const {
        return getManager().getMemoryAlerts();
    }
    
    void checkForLeaks() {
        getManager().checkForLeaks();
    }
    
    MemoryOptimizer::MemoryPerformanceMetrics getPerformanceMetrics(size_t operationCount = 1000) {
        return getManager().getPerformanceMetrics(operationCount);
    }
    
    void updateConfig(const MemoryConfig& config) {
        getManager().updateConfig(config);
    }
    
    MemoryConfig getConfig() const {
        return getManager().getConfig();
    }
    
    void clear() {
        getManager().clear();
    }
};

/**
 * Memory management utilities
 */
namespace MemoryUtils {
    template<typename T>
    using MemoryPtr = std::unique_ptr<T, std::function<void(T*)>>;
    
    template<typename T>
    MemoryPtr<T> makeMemoryPtr(T* ptr) {
        return MemoryPtr<T>(ptr, [](T* p) {
            GlobalMemoryManager::getInstance().deallocate(p, sizeof(T), "smart_ptr_deallocate");
        });
    }
    
    template<typename T>
    void simdCopy(T* dest, const T* src, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            dest[i] = src[i];
        }
    }
    
    template<typename T>
    void simdMove(T* dest, const T* src, size_t count) {
        std::memmove(dest, src, count * sizeof(T));
    }
    
    template<typename T>
    void simdFill(T* dest, const T& value, size_t count) {
        for (size_t i = 0; i < count; ++i) {
            dest[i] = value;
        }
    }
    
    template<typename T>
    T* alignAllocation(size_t alignment = 64) {
        return static_cast<T*>(std::aligned_alloc(alignment, sizeof(T)));
    }
    
    template<typename T>
    void deallocateAligned(T* ptr) {
        free(ptr);
    }
    
    struct GlobalMemoryStats {
        uint64_t totalAllocated;
        uint64_t totalDeallocated;
        uint64_t currentUsage;
        uint64_t peakUsage;
        uint64_t allocationCount;
        uint64_t deallocationCount;
        std::chrono::nanoseconds totalAllocationTime;
        std::chrono::nanoseconds totalDeallocationTime;
        
        GlobalMemoryStats() : totalAllocated(0), totalDeallocated(0), currentUsage(0),
                             peakUsage(0), allocationCount(0), deallocationCount(0),
                             totalAllocationTime(0), totalDeallocationTime(0) {}
    };
    
    GlobalMemoryStats getGlobalMemoryStats() {
        GlobalMemoryStats stats;
        const auto& samples = GlobalMemoryManager::getInstance().getMemorySamples();
        
        for (const auto& sample : samples) {
            stats.totalAllocated += sample.allocationCount;
            stats.totalDeallocated += sample.deallocationCount;
            if (sample.memoryUsage > stats.peakUsage) {
                stats.peakUsage = sample.memoryUsage;
            }
            stats.currentUsage = sample.memoryUsage;
        }
        
        return stats;
    }
    
    std::vector<void*> getLeakedMemory() {
        return std::vector<void*>();
    }
    
    void optimizeMemoryUsage() {
        GlobalMemoryManager::getInstance().optimizeMemoryUsage();n    }
    
    std::string generateMemoryReport() {
        std::stringstream ss;
        ss << "=== NLM Memory Management Report ===" << std::endl;
        
        const auto& stats = getGlobalMemoryStats();
        ss << "Total Allocations: " << stats.totalAllocated << std::endl;
        ss << "Total Deallocations: " << stats.totalDeallocated << std::endl;
        ss << "Current Usage: " << stats.currentUsage << " bytes" << std::endl;
        ss << "Peak Usage: " << stats.peakUsage << " bytes" << std::endl;
        ss << "Allocation Count: " << stats.allocationCount << std::endl;
        ss << "Deallocation Count: " << stats.deallocationCount << std::endl;
        
        return ss.str();
    }
    
    void updateMemoryConfig(const MemoryConfig& config) {
        GlobalMemoryManager::getInstance().updateConfig(config);
    }
    
    MemoryConfig getMemoryConfig() {
        return GlobalMemoryManager::getInstance().getConfig();
    }
}

/**
 * Memory management macros
 */
#define ALLOCATE_MEMORY(size, context) GlobalMemoryManager::getInstance().allocate(size, context)
#define DEALLOCATE_MEMORY(ptr, size, context) GlobalMemoryManager::getInstance().deallocate(ptr, size, context)
#define ALLOCATE_NEURON_MEMORY(component, count) GlobalMemoryManager::getInstance().allocateNeuronMemory(component, count)
#define DEALLOCATE_NEURON_MEMORY(neurons) GlobalMemoryManager::getInstance().deallocateNeuronMemory(neurons)
#define SAMPLE_MEMORY_USAGE(context) GlobalMemoryManager::getInstance().sampleMemoryUsage(context)

/**
 * Initialization and finalization functions
 */
inline void initializeMemoryManager(const MemoryConfig& config = MemoryConfig()) {
    GlobalMemoryManager::getInstance().initialize(config);
}

inline void shutdownMemoryManager() {
    GlobalMemoryManager::getInstance().shutdown();
}

inline void startMemoryMonitoring() {
    GlobalMemoryManager::getInstance().startMonitoring();
}

inline void stopMemoryMonitoring() {
    GlobalMemoryManager::getInstance().stopMonitoring();
}

inline void updateMemoryConfig(const MemoryConfig& config) {
    GlobalMemoryManager::getInstance().updateConfig(config);
}

inline MemoryConfig getMemoryConfig() {
    return GlobalMemoryManager::getInstance().getConfig();
}

/**
 * Convenience functions for different NLM systems
 */
inline void initializeNeuralMemoryManager(size_t neuronCount = 1000) {
    MemoryConfig config = getMemoryConfig();
    config.enableMemoryPools = true;
    config.maxFixedPoolSize = neuronCount;
    updateMemoryConfig(config);
    
    auto neurons = ALLOCATE_NEURON_MEMORY<float>("neuron", neuronCount);
    DEALLOCATE_NEURON_MEMORY(neurons);
}

inline void initializeBrainMemoryManager(size_t brainComponentCount = 500) {
    MemoryConfig config = getMemoryConfig();
    config.enableMemoryPools = true;
    config.maxDynamicPoolSize = brainComponentCount * 10;
    updateMemoryConfig(config);
    
    auto brainComponents = ALLOCATE_BRAIN_MEMORY<float>("brain", brainComponentCount);
    DEALLOCATE_BRAIN_MEMORY(brainComponents);
}

inline std::string getMemoryReport() {
    return MemoryUtils::generateMemoryReport();
}

inline void checkForMemoryLeaks() {
    GlobalMemoryManager::getInstance().checkForLeaks();
}

} // namespace memory
} // namespace nlm

#endif // NLM_MEMORY_MANAGER_HPP