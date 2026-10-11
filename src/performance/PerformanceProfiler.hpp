#pragma once

/**
 * Performance measurement and profiling infrastructure
 * 
 * Provides comprehensive performance measurement for the enhanced Brain implementation,
 * including timing, memory usage, cache performance, and SIMD utilization.
 * 
 * Features:
 * - High-resolution timing measurements
 * - Memory usage tracking
 * - Cache performance monitoring
 * - SIMD utilization metrics
 * - Parallel execution profiling
 * - Thread-safe performance data collection
 * - Performance bottleneck identification
 * - Benchmark and profiling utilities
 * 
 * Biological motivation:
 * - Brains have evolved efficient performance strategies
 * - Neural systems optimize for minimal energy and maximal speed
 * - Event-driven computation reduces unnecessary work
 */

#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include <mutex>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <cstdint>

namespace nlm {

/**
 * High-resolution timer for performance measurement
 */
class PerformanceTimer {
public:
    PerformanceTimer() : startTime_(std::chrono::high_resolution_clock::now()) {}
    
    ~PerformanceTimer() = default;
    
    // Get elapsed time in microseconds
    uint64_t elapsedMicroseconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_).count();
    }
    
    // Get elapsed time in milliseconds
    double elapsedMilliseconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_).count();
    }
    
    // Get elapsed time in seconds
    double elapsedSeconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime_).count();
    }
    
    // Reset timer
    void reset() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    // Get current timestamp for relative timing
    uint64_t timestamp() const {
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now - startTime_).count();
    }
    
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
};

/**
 * Memory usage tracker for performance analysis
 */
class MemoryUsageTracker {
public:
    struct MemoryMetrics {
        uint64_t allocatedBytes;
        uint64_t usedBytes;
        uint64_t peakAllocatedBytes;
        uint64_t deallocatedBytes;
        double allocationRate; // bytes per second
        
        MemoryMetrics() : allocatedBytes(0), usedBytes(0), peakAllocatedBytes(0), 
                         deallocatedBytes(0), allocationRate(0.0) {}
    };
    
    MemoryUsageTracker() : startTime_(std::chrono::high_resolution_clock::now()),
                          allocatedBytes_(0), peakAllocatedBytes_(0), deallocatedBytes_(0) {}
    
    ~MemoryUsageTracker() = default;
    
    // Record memory allocation
    void recordAllocation(uint64_t bytes) {
        std::lock_guard<std::mutex> lock(mutex_);
        allocatedBytes_ += bytes;
        peakAllocatedBytes_ = std::max(peakAllocatedBytes_, allocatedBytes_);
        updateAllocationRate();
    }
    
    // Record memory deallocation
    void recordDeallocation(uint64_t bytes) {
        std::lock_guard<std::mutex> lock(mutex_);
        allocatedBytes_ -= bytes;
        if (allocatedBytes_ < 0) allocatedBytes_ = 0;
        deallocatedBytes_ += bytes;
        updateAllocationRate();
    }
    
    // Get current memory metrics
    MemoryMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        MemoryMetrics metrics;
        metrics.allocatedBytes = allocatedBytes_;
        metrics.peakAllocatedBytes = peakAllocatedBytes_;
        metrics.deallocatedBytes = deallocatedBytes_;
        metrics.usedBytes = allocatedBytes_ - deallocatedBytes_;
        metrics.allocationRate = allocationRate_;
        return metrics;
    }
    
    // Reset tracker
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        allocatedBytes_ = 0;
        peakAllocatedBytes_ = 0;
        deallocatedBytes_ = 0;
        startTime_ = std::chrono::high_resolution_clock::now();
        allocationRate_ = 0.0;
    }
    
private:
    void updateAllocationRate() {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime_).count();
        if (elapsed > 0) {
            allocationRate_ = static_cast<double>(allocatedBytes_ + deallocatedBytes_) / elapsed;
        }
    }
    
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
    mutable std::mutex mutex_;
    uint64_t allocatedBytes_;
    uint64_t peakAllocatedBytes_;
    uint64_t deallocatedBytes_;
    double allocationRate_;
};

/**
 * Cache performance monitor
 */
class CachePerformanceMonitor {
public:
    struct CacheMetrics {
        uint64_t cacheHits;
        uint64_t cacheMisses;
        double hitRate;
        uint64_t totalAccesses;
        uint64_t linesFetched;
        uint64_t linesEvicted;
        
        CacheMetrics() : cacheHits(0), cacheMisses(0), hitRate(0.0), 
                         totalAccesses(0), linesFetched(0), linesEvicted(0) {}
    };
    
    CachePerformanceMonitor() : cacheHits_(0), cacheMisses_(0), totalAccesses_(0),
                                linesFetched_(0), linesEvicted_(0) {}
    
    ~CachePerformanceMonitor() = default;
    
    // Record cache access
    void recordAccess(bool hit) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalAccesses_++;
        if (hit) {
            cacheHits_++;
        } else {
            cacheMisses_++;
        }
    }
    
    // Record cache line fetch
    void recordFetch() {
        std::lock_guard<std::mutex> lock(mutex_);
        linesFetched_++;
    }
    
    // Record cache line eviction
    void recordEviction() {
        std::lock_guard<std::mutex> lock(mutex_);
        linesEvicted_++;
    }
    
    // Get current cache metrics
    CacheMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        CacheMetrics metrics;
        metrics.cacheHits = cacheHits_;
        metrics.cacheMisses = cacheMisses_;
        metrics.totalAccesses = totalAccesses_;
        metrics.linesFetched = linesFetched_;
        metrics.linesEvicted = linesEvicted_;
        
        if (totalAccesses_ > 0) {
            metrics.hitRate = static_cast<double>(cacheHits_) / totalAccesses_;
        }
        
        return metrics;
    }
    
    // Reset monitor
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        cacheHits_ = 0;
        cacheMisses_ = 0;
        totalAccesses_ = 0;
        linesFetched_ = 0;
        linesEvicted_ = 0;
    }
    
private:
    mutable std::mutex mutex_;
    uint64_t cacheHits_;
    uint64_t cacheMisses_;
    uint64_t totalAccesses_;
    uint64_t linesFetched_;
    uint64_t linesEvicted_;
};

/**
 * SIMD utilization tracker
 */
class SIMDUtilizationTracker {
public:
    struct SIMDMetrics {
        uint64_t lifSimdOperations;
        uint64_t synapseSimdOperations;
        uint64_t memorySimdOperations;
        uint64_t similaritySimdOperations;
        uint64_t totalSimdOperations;
        double simdUtilizationRatio; // percentage of operations using SIMD
        
        SIMDMetrics() : lifSimdOperations(0), synapseSimdOperations(0), 
                        memorySimdOperations(0), similaritySimdOperations(0),
                        totalSimdOperations(0), simdUtilizationRatio(0.0) {}
    };
    
    SIMDUtilizationTracker() : lifSimdOps_(0), synapseSimdOps_(0), 
                               memorySimdOps_(0), similaritySimdOps_(0),
                               totalSimdOps_(0) {}
    
    ~SIMDUtilizationTracker() = default;
    
    // Record SIMD operation for LIF processing
    void recordLIFSIMD(uint64_t count = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        lifSimdOps_ += count;
        totalSimdOps_ += count;
    }
    
    // Record SIMD operation for synapse processing
    void recordSynapseSIMD(uint64_t count = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        synapseSimdOps_ += count;
        totalSimdOps_ += count;
    }
    
    // Record SIMD operation for memory operations
    void recordMemorySIMD(uint64_t count = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        memorySimdOps_ += count;
        totalSimdOps_ += count;
    }
    
    // Record SIMD operation for similarity computations
    void recordSimilaritySIMD(uint64_t count = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        similaritySimdOps_ += count;
        totalSimdOps_ += count;
    }
    
    // Update SIMD utilization ratio
    void updateUtilizationRatio(uint64_t totalOperations) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (totalOperations > 0) {
            simdUtilizationRatio_ = static_cast<double>(totalSimdOps_) / totalOperations;
        }
    }
    
    // Get current SIMD metrics
    SIMDMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        SIMDMetrics metrics;
        metrics.lifSimdOperations = lifSimdOps_;
        metrics.synapseSimdOperations = synapseSimdOps_;
        metrics.memorySimdOperations = memorySimdOps_;
        metrics.similaritySimdOperations = similaritySimdOps_;
        metrics.totalSimdOperations = totalSimdOps_;
        metrics.simdUtilizationRatio = simdUtilizationRatio_;
        return metrics;
    }
    
    // Reset tracker
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        lifSimdOps_ = 0;
        synapseSimdOps_ = 0;
        memorySimdOps_ = 0;
        similaritySimdOps_ = 0;
        totalSimdOps_ = 0;
        simdUtilizationRatio_ = 0.0;
    }
    
private:
    mutable std::mutex mutex_;
    uint64_t lifSimdOps_;
    uint64_t synapseSimdOps_;
    uint64_t memorySimdOps_;
    uint64_t similaritySimdOps_;
    uint64_t totalSimdOps_;
    double simdUtilizationRatio_;
};

/**
 * Parallel execution profiler
 */
class ParallelExecutionProfiler {
public:
    struct ParallelMetrics {
        uint64_t parallelTasks;
        uint64_t serialTasks;
        uint64_t totalTasks;
        double parallelEfficiency; // ratio of parallel work
        uint64_t threadPoolSize;
        uint64_t averageTaskDuration; // microseconds
        
        ParallelMetrics() : parallelTasks(0), serialTasks(0), totalTasks(0),
                           parallelEfficiency(0.0), threadPoolSize(0), averageTaskDuration(0) {}
    };
    
    ParallelExecutionProfiler() : parallelTasks_(0), serialTasks_(0), totalTasks_(0),
                                  threadPoolSize_(std::thread::hardware_concurrency()),
                                  totalDuration_(0), taskCount_(0) {}
    
    ~ParallelExecutionProfiler() = default;
    
    // Record parallel task execution
    void recordParallelTask(uint64_t duration) {
        std::lock_guard<std::mutex> lock(mutex_);
        parallelTasks_++;
        totalTasks_++;
        totalDuration_ += duration;
    }
    
    // Record serial task execution
    void recordSerialTask(uint64_t duration) {
        std::lock_guard<std::mutex> lock(mutex_);
        serialTasks_++;
        totalTasks_++;
        totalDuration_ += duration;
    }
    
    // Update parallel efficiency
    void updateParallelEfficiency() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (totalTasks_ > 0) {
            parallelEfficiency_ = static_cast<double>(parallelTasks_) / totalTasks_;
        }
    }
    
    // Get current parallel metrics
    ParallelMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        ParallelMetrics metrics;
        metrics.parallelTasks = parallelTasks_;
        metrics.serialTasks = serialTasks_;
        metrics.totalTasks = totalTasks_;
        metrics.parallelEfficiency = parallelEfficiency_;
        metrics.threadPoolSize = threadPoolSize_;
        if (taskCount_ > 0) {
            metrics.averageTaskDuration = totalDuration_ / taskCount_;
        }
        return metrics;
    }
    
    // Reset profiler
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        parallelTasks_ = 0;
        serialTasks_ = 0;
        totalTasks_ = 0;
        parallelEfficiency_ = 0.0;
        totalDuration_ = 0;
        taskCount_ = 0;
    }
    
private:
    mutable std::mutex mutex_;
    uint64_t parallelTasks_;
    uint64_t serialTasks_;
    uint64_t totalTasks_;
    double parallelEfficiency_;
    uint64_t threadPoolSize_;
    uint64_t totalDuration_;
    uint64_t taskCount_;
};

/**
 * Comprehensive performance profiler
 * Combines all performance tracking subsystems
 */
class PerformanceProfiler {
public:
    struct CompleteMetrics {
        uint64_t simulationStep;
        double wallClockTime; // seconds
        MemoryUsageTracker::MemoryMetrics memoryMetrics;
        CachePerformanceMonitor::CacheMetrics cacheMetrics;
        SIMDUtilizationTracker::SIMDMetrics simdMetrics;
        ParallelExecutionProfiler::ParallelMetrics parallelMetrics;
        
        // Performance ratios
        double memoryEfficiency; // used / allocated
        double cacheHitRate; // from cache metrics
        double simdUtilization; // from SIMD metrics
        double parallelEfficiency; // from parallel metrics
        
        CompleteMetrics() : simulationStep(0), wallClockTime(0.0), memoryEfficiency(0.0),
                           cacheHitRate(0.0), simdUtilization(0.0), parallelEfficiency(0.0) {}
    };
    
    PerformanceProfiler() : currentStep_(0), simulationStartTime_(std::chrono::high_resolution_clock::now()) {}
    
    ~PerformanceProfiler() = default;
    
    // Start profiling for a new simulation step
    void startStep(SimulationStep step) {
        currentStep_ = step;
        stepStartTime_ = std::chrono::high_resolution_clock::now();
        timers_.push_back(PerformanceTimer());
    }
    
    // End current simulation step and collect metrics
    CompleteMetrics endStep() {
        auto endTime = std::chrono::high_resolution_clock::now();
        double wallClockTime = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - stepStartTime_).count();
        
        CompleteMetrics metrics;
        metrics.simulationStep = currentStep_;
        metrics.wallClockTime = wallClockTime;
        
        // Collect metrics from subsystems
        metrics.memoryMetrics = memoryTracker_.getMetrics();
        metrics.cacheMetrics = cacheMonitor_.getMetrics();
        metrics.simdMetrics = simdTracker_.getMetrics();
        metrics.parallelMetrics = parallelProfiler_.getMetrics();
        
        // Calculate derived metrics
        if (metrics.memoryMetrics.allocatedBytes > 0) {
            metrics.memoryEfficiency = static_cast<double>(metrics.memoryMetrics.usedBytes) / 
                                      metrics.memoryMetrics.allocatedBytes;
        }
        metrics.cacheHitRate = metrics.cacheMetrics.hitRate;
        metrics.simdUtilization = metrics.simdMetrics.simdUtilizationRatio;
        metrics.parallelEfficiency = metrics.parallelMetrics.parallelEfficiency;
        
        // Update task count for parallel metrics
        parallelProfiler_.updateParallelEfficiency();
        
        // Reset timers for next step
        timers_.clear();
        
        return metrics;
    }
    
    // Record memory allocation
    void recordMemoryAllocation(uint64_t bytes) {
        memoryTracker_.recordAllocation(bytes);
    }
    
    // Record memory deallocation
    void recordMemoryDeallocation(uint64_t bytes) {
        memoryTracker_.recordDeallocation(bytes);
    }
    
    // Record cache access
    void recordCacheAccess(bool hit) {
        cacheMonitor_.recordAccess(hit);
    }
    
    // Record cache line fetch
    void recordCacheFetch() {
        cacheMonitor_.recordFetch();
    }
    
    // Record cache line eviction
    void recordCacheEviction() {
        cacheMonitor_.recordEviction();
    }
    
    // Record SIMD operation
    void recordSIMDOperation(const std::string& operationType, uint64_t count = 1) {
        if (operationType == "LIF") {
            simdTracker_.recordLIFSIMD(count);
        } else if (operationType == "Synapse") {
            simdTracker_.recordSynapseSIMD(count);
        } else if (operationType == "Memory") {
            simdTracker_.recordMemorySIMD(count);
        } else if (operationType == "Similarity") {
            simdTracker_.recordSimilaritySIMD(count);
        }
    }
    
    // Record parallel task
    void recordParallelTask(uint64_t duration) {
        parallelProfiler_.recordParallelTask(duration);
    }
    
    // Record serial task
    void recordSerialTask(uint64_t duration) {
        parallelProfiler_.recordSerialTask(duration);
    }
    
    // Update SIMD utilization
    void updateSIMDUtilization(uint64_t totalOperations) {
        simdTracker_.updateUtilizationRatio(totalOperations);
    }
    
    // Get complete metrics
    CompleteMetrics getCompleteMetrics() const {
        CompleteMetrics metrics;
        metrics.memoryMetrics = memoryTracker_.getMetrics();
        metrics.cacheMetrics = cacheMonitor_.getMetrics();
        metrics.simdMetrics = simdTracker_.getMetrics();
        metrics.parallelMetrics = parallelProfiler_.getMetrics();
        
        if (metrics.memoryMetrics.allocatedBytes > 0) {
            metrics.memoryEfficiency = static_cast<double>(metrics.memoryMetrics.usedBytes) / 
                                      metrics.memoryMetrics.allocatedBytes;
        }
        metrics.cacheHitRate = metrics.cacheMetrics.hitRate;
        metrics.simdUtilization = metrics.simdMetrics.simdUtilizationRatio;
        metrics.parallelEfficiency = metrics.parallelMetrics.parallelEfficiency;
        
        return metrics;
    }
    
    // Print performance report
    void printPerformanceReport(const CompleteMetrics& metrics) const {
        std::cout << "=== Performance Report ===" << std::endl;
        std::cout << "Simulation Step: " << metrics.simulationStep << std::endl;
        std::cout << "Wall Clock Time: " << std::fixed << std::setprecision(4) 
                  << metrics.wallClockTime << " seconds" << std::endl;
        
        std::cout << "\nMemory Usage:" << std::endl;
        std::cout << "  Allocated: " << metrics.memoryMetrics.allocatedBytes << " bytes" << std::endl;
        std::cout << "  Used:      " << metrics.memoryMetrics.usedBytes << " bytes" << std::endl;
        std::cout << "  Peak:      " << metrics.memoryMetrics.peakAllocatedBytes << " bytes" << std::endl;
        std::cout << "  Efficiency: " << std::fixed << std::setprecision(2) 
                  << metrics.memoryEfficiency * 100 << "%" << std::endl;
        
        std::cout << "\nCache Performance:" << std::endl;
        std::cout << "  Hits:       " << metrics.cacheMetrics.cacheHits << std::endl;
        std::cout << "  Misses:     " << metrics.cacheMetrics.cacheMisses << std::endl;
        std::cout << "  Hit Rate:   " << std::fixed << std::setprecision(2) 
                  << metrics.cacheHitRate * 100 << "%" << std::endl;
        std::cout << "  Total Accesses: " << metrics.cacheMetrics.totalAccesses << std::endl;
        
        std::cout << "\nSIMD Utilization:" << std::endl;
        std::cout << "  LIF Operations:     " << metrics.simdMetrics.lifSimdOperations << std::endl;
        std::cout << "  Synapse Operations: " << metrics.simdMetrics.synapseSimdOperations << std::endl;
        std::cout << "  Memory Operations:  " << metrics.simdMetrics.memorySimdOperations << std::endl;
        std::cout << "  Similarity Ops:    " << metrics.simdMetrics.similaritySimdOperations << std::endl;
        std::cout << "  Total SIMD Ops:    " << metrics.simdMetrics.totalSimdOperations << std::endl;
        std::cout << "  Utilization Ratio: " << std::fixed << std::setprecision(2) 
                  << metrics.simdUtilization * 100 << "%" << std::endl;
        
        std::cout << "\nParallel Execution:" << std::endl;
        std::cout << "  Parallel Tasks:  " << metrics.parallelMetrics.parallelTasks << std::endl;
        std::cout << "  Serial Tasks:    " << metrics.parallelMetrics.serialTasks << std::endl;
        std::cout << "  Total Tasks:     " << metrics.parallelMetrics.totalTasks << std::endl;
        std::cout << "  Efficiency:      " << std::fixed << std::setprecision(2) 
                  << metrics.parallelEfficiency * 100 << "%" << std::endl;
        std::cout << "  Thread Pool Size:" << metrics.parallelMetrics.threadPoolSize << std::endl;
        std::cout << "  Avg Task Duration:" << metrics.parallelMetrics.averageTaskDuration << " μs" << std::endl;
        
        std::cout << "\n=== End Performance Report ===" << std::endl;
    }
    
    // Reset profiler for new simulation
    void reset() {
        currentStep_ = 0;
        simulationStartTime_ = std::chrono::high_resolution_clock::now();
        memoryTracker_.reset();
        cacheMonitor_.reset();
        simdTracker_.reset();
        parallelProfiler_.reset();
    }
    
private:
    SimulationStep currentStep_;
    std::chrono::time_point<std::chrono::high_resolution_clock> simulationStartTime_;
    std::chrono::time_point<std::chrono::high_resolution_clock> stepStartTime_;
    std::vector<PerformanceTimer> timers_;
    
    MemoryUsageTracker memoryTracker_;
    CachePerformanceMonitor cacheMonitor_;
    SIMDUtilizationTracker simdTracker_;
    ParallelExecutionProfiler parallelProfiler_;
    
    mutable std::mutex mutex_;
};

} // namespace nlm