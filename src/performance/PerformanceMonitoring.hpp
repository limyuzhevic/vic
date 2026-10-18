#pragma once

/**
 * Performance Monitoring and Benchmarking Infrastructure
 * 
 * This module provides comprehensive performance monitoring capabilities for the
 * NLM neural simulation framework, including:
 * 
 * 1. **SIMD Performance Monitoring**:
 *    - SIMD availability detection and profiling
 *    - Vectorization efficiency metrics
 *    - Architecture-specific performance characteristics
 *    
 * 2. **Memory Pool Monitoring**:
 *    - Memory pool efficiency tracking
 *    - Allocation/deallocation statistics
 *    - Memory fragmentation measurement
 *    
 * 3. **Benchmarking Utilities**:
 *    - Step timing and throughput measurement
 *    - Memory usage tracking
 *    - Comparative benchmarking
 *    
 * 4. **Resource-Aware Optimization Selection**:
 *    - Dynamic optimization level selection based on system resources
 *    - Adaptive performance tuning
 *    - Fallback management
 *    
 * This infrastructure enables the performance targets specified in the requirements:
 * - SIMD acceleration should provide 2-4x speedup for vector operations
 * - Memory pools should reduce allocation overhead by 50-90%
 * - Overall simulation performance improvement of 1.5-2x
 * - Memory usage reduction of 20-40%
 */

#include <chrono>
#include <map>
#include <vector>
#include <atomic>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace nlm {

/**
 * Performance Measurement Units
 */
enum class PerformanceUnit {
    FLOPS,                    // Floating point operations per second
    NEURONS_PER_SECOND,        // Neural computation rate
    SPIKES_PER_SECOND,         // Spike emission rate
    SYNAPTIC_EVENTS_PER_SECOND, // Synaptic event rate
    MEMORY_BANDWIDTH_GBPS,     // Memory bandwidth
    CPU_CYCLES,               // CPU cycle measurements
    INSTRUCTIONS_PER_CYCLE     // Instruction throughput
};

/**
 * Performance Counter for precise measurement
 */
class PerformanceCounter {
public:
    using TimePoint = std::chrono::high_resolution_clock::time_point;
    
    PerformanceCounter()
        : startTime_(std::chrono::high_resolution_clock::now()),
          paused_(false),
          pausedTime_(TimePoint::clock::duration::zero()),
          totalPausedDuration_(TimePoint::clock::duration::zero()) {}
    
    /**
     * Start or restart timing measurement
     */
    void start() {
        if (paused_) {
            auto now = std::chrono::high_resolution_clock::now();
            pausedTime_ = now - pausedTime_;
            totalPausedDuration_ += pausedTime_;
            paused_ = false;
        }
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    /**
     * Pause timing measurement
     */
    void pause() {
        if (!paused_) {
            auto now = std::chrono::high_resolution_clock::now();
            pausedTime_ = now;
            paused_ = true;
        }
    }
    
    /**
     * Stop timing measurement
     */
    void stop() {
        pause();
    }
    
    /**
     * Get elapsed time in nanoseconds
     */
    uint64_t elapsedNanoseconds() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now - startTime_ - totalPausedDuration_;
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
    
    /**
     * Get elapsed time in microseconds
     */
    uint64_t elapsedMicroseconds() const {
        return elapsedNanoseconds() / 1000;
    }
    
    /**
     * Get elapsed time in milliseconds
     */
    uint64_t elapsedMilliseconds() const {
        return elapsedNanoseconds() / 1'000'000;
    }
    
    /**
     * Get elapsed time in seconds
     */
    double elapsedSeconds() const {
        return elapsedNanoseconds() / 1'000'000'000.0;
    }
    
    /**
     * Check if counter is currently running
     */
    bool isRunning() const { return !paused_; }
    
    /**
     * Reset counter to initial state
     */
    void reset() {
        startTime_ = std::chrono::high_resolution_clock::now();
        paused_ = false;
        pausedTime_ = TimePoint::clock::duration::zero();
        totalPausedDuration_ = TimePoint::clock::duration::zero();
    }
    
private:
    TimePoint startTime_;
    bool paused_;
    TimePoint pausedTime_;
    TimePoint::clock::duration totalPausedDuration_;
};

/**
 * SIMD Performance Profiler
 * 
 * Tracks SIMD-specific performance metrics including vectorization efficiency,
 * instruction throughput, and architecture-specific characteristics.
 */
class SIMDProfiler {
public:
    struct SIMDMetrics {
        uint64_t scalarOperations;        // Operations executed in scalar fallback
        uint64_t vectorOperations;       // Operations executed using SIMD
        double vectorizationEfficiency;  // SIMD utilization ratio
        std::string simdArchitecture;    // Detected SIMD architecture (AVX-512, AVX2, SSE, Scalar)
        uint64_t theoreticalFlops;       // Theoretical peak FLOPS for current architecture
        double actualFlops;              // Measured FLOPS
        uint64_t cacheMisses;            // L1/L2 cache misses
        uint64_t memoryAccesses;         // Total memory accesses
        double memoryBandwidthGBPS;      // Measured memory bandwidth
    };
    
    SIMDProfiler()
        : enabled_(false),
          metrics_(),
          vectorizationCounter_(0),
          scalarFallbackCounter_(0),
          totalOperations_(0),
          lastMeasurementTime_(std::chrono::high_resolution_clock::now()) {}
    
    /**
     * Initialize SIMD profiler with architecture detection
     */
    void initialize() {
        enabled_ = SIMDUtils::isSIMDAvailable();
        metrics_.simdArchitecture = SIMDUtils::getSIMDType();
        
        // Set theoretical FLOPS based on SIMD architecture
        if (metrics_.simdArchitecture == "AVX-512") {
            metrics_.theoreticalFlops = 16ULL * 4 * 3;  // 16 floats, 4 ops per float, 3 cycles per flop (optimistic)
        } else if (metrics_.simdArchitecture == "AVX2") {
            metrics_.theoreticalFlops = 8ULL * 4 * 3;
        } else if (metrics_.simdArchitecture == "SSE") {
            metrics_.theoreticalFlops = 4ULL * 4 * 3;
        } else {
            metrics_.theoreticalFlops = 1;  // Scalar only
        }
        
        NLM_LOG_INFO("SIMD profiler initialized: " + metrics_.simdArchitecture + " vector width = " + 
                    std::to_string(SIMDUtils::SIMD_WIDTH) + " floats");
    }
    
    /**
     * Record vectorized operation
     */
    void recordVectorOperation(uint64_t count = 1) {
        if (!enabled_) return;
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_.vectorOperations += count;
        vectorizationCounter_++;
        totalOperations_ += count;
    }
    
    /**
     * Record scalar fallback operation
     */
    void recordScalarOperation(uint64_t count = 1) {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_.scalarOperations += count;
        scalarFallbackCounter_++;
        totalOperations_ += count;
    }
    
    /**
     * Update cache and memory statistics
     */
    void updateMemoryStats(uint64_t cacheMisses, uint64_t memoryAccesses, double bandwidthGBPS) {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_.cacheMisses += cacheMisses;
        metrics_.memoryAccesses += memoryAccesses;
        metrics_.memoryBandwidthGBPS = bandwidthGBPS;
    }
    
    /**
     * Calculate vectorization efficiency
     */
    double calculateVectorizationEfficiency() {
        if (totalOperations_ == 0) return 0.0;
        return static_cast<double>(metrics_.vectorOperations) / totalOperations_;
    }
    
    /**
     * Update measured FLOPS based on elapsed time
     */
    void updateFlops(double elapsedSeconds) {
        if (elapsedSeconds > 0.0) {
            metrics_.actualFlops = static_cast<double>(metrics_.vectorOperations) * SIMDUtils::SIMD_WIDTH * 4 / elapsedSeconds;
            metrics_.vectorizationEfficiency = calculateVectorizationEfficiency();
        }
    }
    
    /**
     * Get current metrics
     */
    SIMDMetrics getMetrics() const {
        SIMDMetrics copy = metrics_;
        copy.vectorizationEfficiency = calculateVectorizationEfficiency();
        return copy;
    }
    
    /**
     * Check if SIMD is available and enabled
     */
    bool isAvailable() const { return enabled_; }
    
    /**
     * Get SIMD architecture name
     */
    const std::string& getArchitecture() const { return metrics_.simdArchitecture; }
    
    /**
     * Reset metrics for new benchmark run
     */
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        metrics_ = SIMDMetrics();
        vectorizationCounter_ = 0;
        scalarFallbackCounter_ = 0;
        totalOperations_ = 0;
        lastMeasurementTime_ = std::chrono::high_resolution_clock::now();
        initialize();
    }
    
    /**
     * Format metrics as human-readable string
     */
    std::string formatMetrics() const {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "SIMD Performance Metrics:" << std::endl;
        ss << "  Architecture: " << metrics_.simdArchitecture << std::endl;
        ss << "  Vector Operations: " << metrics_.vectorOperations << std::endl;
        ss << "  Scalar Operations: " << metrics_.scalarOperations << std::endl;
        ss << "  Vectorization Efficiency: " << metrics_.vectorizationEfficiency * 100.0 << "%" << std::endl;
        ss << "  Measured FLOPS: " << metrics_.actualFlops << std::endl;
        ss << "  Theoretical FLOPS: " << metrics_.theoreticalFlops << std::endl;
        ss << "  Efficiency Ratio: " << (metrics_.actualFlops / metrics_.theoreticalFlops) * 100.0 << "%" << std::endl;
        ss << "  Cache Misses: " << metrics_.cacheMisses << std::endl;
        ss << "  Memory Bandwidth: " << metrics_.memoryBandwidthGBPS << " GB/s" << std::endl;
        
        return ss.str();
    }
    
    /**
     * Get optimization recommendation based on metrics
     */
    std::string getOptimizationRecommendation() const {
        if (metrics_.vectorizationEfficiency < 0.5) {
            return "Vectorization efficiency is low (<50%). Consider reducing problem size or checking for SIMD bottlenecks.";
        } else if (metrics_.vectorizationEfficiency > 0.9) {
            return "Excellent vectorization efficiency (>90%). Optimal SIMD utilization.";
        } else {
            return "Good vectorization efficiency (50-90%). SIMD utilization is within expected range.";
        }
    }
    
private:
    bool enabled_;
    SIMDMetrics metrics_;
    std::atomic<uint64_t> vectorizationCounter_;
    std::atomic<uint64_t> scalarFallbackCounter_;
    std::atomic<uint64_t> totalOperations_;
    TimePoint lastMeasurementTime_;
    mutable std::mutex mutex_;
};

/**
 * Memory Pool Performance Monitor
 * 
 * Tracks memory pool efficiency, allocation overhead, and memory usage patterns
 * to validate the 50-90% allocation overhead reduction target.
 */
class MemoryPoolMonitor {
public:
    struct MemoryPoolMetrics {
        uint64_t totalAllocations;        // Total number of allocation requests
        uint64_t successfulAllocations;   // Successfully fulfilled allocations
        uint64_t failedAllocations;       // Failed allocations (pool empty)
        uint64_t totalDeallocations;      // Total number of deallocation requests
        double allocationSuccessRate;     // Success rate (0.0 to 1.0)
        uint64_t currentAllocatedObjects; // Number of objects currently allocated
        uint64_t peakAllocatedObjects;    // Peak number of allocated objects
        uint64_t totalObjectsAllocated;   // Total objects ever allocated
        uint64_t memoryUsageBytes;        // Current memory usage in bytes
        uint64_t peakMemoryUsageBytes;    // Peak memory usage
        uint64_t cacheHits;              // Cache hits (if implemented)
        uint64_t cacheMisses;            // Cache misses (if implemented)
        double memoryEfficiencyScore;    // Score 0.0-1.0 reflecting memory efficiency
        std::chrono::nanoseconds avgAllocationTime;  // Average allocation time
        std::chrono::nanoseconds avgDeallocationTime; // Average deallocation time
    };
    
    MemoryPoolMonitor()
        : totalAllocations_(0),
          successfulAllocations_(0),
          failedAllocations_(0),
          totalDeallocations_(0),
          currentAllocatedObjects_(0),
          peakAllocatedObjects_(0),
          totalObjectsAllocated_(0),
          memoryUsageBytes_(0),
          peakMemoryUsageBytes_(0),
          cacheHits_(0),
          cacheMisses_(0),
          totalAllocationTime_(std::chrono::nanoseconds(0)),
          totalDeallocationTime_(std::chrono::nanoseconds(0)) {}
    
    /**
     * Record allocation attempt
     */
    void recordAllocation(bool success, uint64_t objectSize = 0, 
                         const std::chrono::nanoseconds& duration = std::chrono::nanoseconds(0)) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalAllocations_++;
        if (success) {
            successfulAllocations_++;
            currentAllocatedObjects_++;
            peakAllocatedObjects_ = std::max(peakAllocatedObjects_, currentAllocatedObjects_);
            totalObjectsAllocated_++;
            memoryUsageBytes_ += objectSize;
            peakMemoryUsageBytes_ = std::max(peakMemoryUsageBytes_, memoryUsageBytes_);
            totalAllocationTime_ += duration;
        } else {
            failedAllocations_++;
        }
    }
    
    /**
     * Record deallocation
     */
    void recordDeallocation(uint64_t objectSize = 0,
                           const std::chrono::nanoseconds& duration = std::chrono::nanoseconds(0)) {
        std::lock_guard<std::mutex> lock(mutex_);
        totalDeallocations_++;
        currentAllocatedObjects_ = std::max<uint64_t>(0, currentAllocatedObjects_ - 1);
        memoryUsageBytes_ -= objectSize;
        totalDeallocationTime_ += duration;
    }
    
    /**
     * Record cache statistics
     */
    void recordCacheStats(uint64_t hits, uint64_t misses) {
        std::lock_guard<std::mutex> lock(mutex_);
        cacheHits_ += hits;
        cacheMisses_ += misses;
    }
    
    /**
     * Calculate allocation success rate
     */
    double getAllocationSuccessRate() const {
        if (totalAllocations_ == 0) return 0.0;
        return static_cast<double>(successfulAllocations_) / totalAllocations_;
    }
    
    /**
     * Calculate average allocation time
     */
    std::chrono::nanoseconds getAverageAllocationTime() const {
        if (successfulAllocations_ == 0) return std::chrono::nanoseconds(0);
        return totalAllocationTime_ / successfulAllocations_;
    }
    
    /**
     * Calculate average deallocation time
     */
    std::chrono::nanoseconds getAverageDeallocationTime() const {
        if (totalDeallocations_ == 0) return std::chrono::nanoseconds(0);
        return totalDeallocationTime_ / totalDeallocations_;
    }
    
    /**
     * Calculate memory efficiency score
     */
    double calculateMemoryEfficiencyScore() const {
        double successRate = getAllocationSuccessRate();
        double cacheHitRate = 0.0;
        uint64_t totalCacheOps = cacheHits_ + cacheMisses_;
        if (totalCacheOps > 0) {
            cacheHitRate = static_cast<double>(cacheHits_) / totalCacheOps;
        }
        
        // Weighted score: 50% success rate, 30% cache efficiency, 20% allocation/deallocation speed
        double allocSpeed = 1.0 / (getAverageAllocationTime().count() / 1000000.0 + 1.0);  // Normalize
        double deallocSpeed = 1.0 / (getAverageDeallocationTime().count() / 1000000.0 + 1.0);
        
        return (successRate * 0.5) + (cacheHitRate * 0.3) + ((allocSpeed + deallocSpeed) * 0.5 * 0.2);
    }
    
    /**
     * Get current metrics
     */
    MemoryPoolMetrics getMetrics() const {
        MemoryPoolMetrics metrics;
        metrics.totalAllocations = totalAllocations_;
        metrics.successfulAllocations = successfulAllocations_;
        metrics.failedAllocations = failedAllocations_;
        metrics.totalDeallocations = totalDeallocations_;
        metrics.allocationSuccessRate = getAllocationSuccessRate();
        metrics.currentAllocatedObjects = currentAllocatedObjects_;
        metrics.peakAllocatedObjects = peakAllocatedObjects_;
        metrics.totalObjectsAllocated = totalObjectsAllocated_;
        metrics.memoryUsageBytes = memoryUsageBytes_;
        metrics.peakMemoryUsageBytes = peakMemoryUsageBytes_;
        metrics.cacheHits = cacheHits_;
        metrics.cacheMisses = cacheMisses_;
        metrics.memoryEfficiencyScore = calculateMemoryEfficiencyScore();
        metrics.avgAllocationTime = getAverageAllocationTime();
        metrics.avgDeallocationTime = getAverageDeallocationTime();
        
        return metrics;
    }
    
    /**
     * Format metrics as human-readable string
     */
    std::string formatMetrics() const {
        std::stringstream ss;
        MemoryPoolMetrics metrics = getMetrics();
        
        ss << "Memory Pool Performance Metrics:" << std::endl;
        ss << "  Allocations: " << metrics.totalAllocations << std::endl;
        ss << "  Successful: " << metrics.successfulAllocations << std::endl;
        ss << "  Failed: " << metrics.failedAllocations << std::endl;
        ss << "  Success Rate: " << (metrics.allocationSuccessRate * 100.0) << "%" << std::endl;
        ss << "  Currently Allocated: " << metrics.currentAllocatedObjects << std::endl;
        ss << "  Peak Allocated: " << metrics.peakAllocatedObjects << std::endl;
        ss << "  Total Objects Ever Allocated: " << metrics.totalObjectsAllocated << std::endl;
        ss << "  Current Memory Usage: " << (metrics.memoryUsageBytes / 1024.0) << " KB" << std::endl;
        ss << "  Peak Memory Usage: " << (metrics.peakMemoryUsageBytes / 1024.0) << " KB" << std::endl;
        ss << "  Cache Hits: " << metrics.cacheHits << std::endl;
        ss << "  Cache Misses: " << metrics.cacheMisses << std::endl;
        ss << "  Cache Hit Rate: " << ((metrics.cacheHits / (metrics.cacheHits + metrics.cacheMisses)) * 100.0) << "%" << std::endl;
        ss << "  Memory Efficiency Score: " << (metrics.memoryEfficiencyScore * 100.0) << "%" << std::endl;
        ss << "  Avg Allocation Time: " << metrics.avgAllocationTime.count() << " ns" << std::endl;
        ss << "  Avg Deallocation Time: " << metrics.avgDeallocationTime.count() << " ns" << std::endl;
        
        // Allocation overhead reduction estimate
        double overheadReduction = 0.0;
        if (metrics.totalObjectsAllocated > 0) {
            overheadReduction = 1.0 - static_cast<double>(metrics.peakMemoryUsageBytes) / 
                              (metrics.totalObjectsAllocated * sizeof(Neuron));
        }
        ss << "  Estimated Allocation Overhead Reduction: " << (overheadReduction * 100.0) << "%" << std::endl;
        
        return ss.str();
    }
    
    /**
     * Reset metrics for new benchmark run
     */
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        totalAllocations_ = 0;
        successfulAllocations_ = 0;
        failedAllocations_ = 0;
        totalDeallocations_ = 0;
        currentAllocatedObjects_ = 0;
        peakAllocatedObjects_ = 0;
        totalObjectsAllocated_ = 0;
        memoryUsageBytes_ = 0;
        peakMemoryUsageBytes_ = 0;
        cacheHits_ = 0;
        cacheMisses_ = 0;
        totalAllocationTime_ = std::chrono::nanoseconds(0);
        totalDeallocationTime_ = std::chrono::nanoseconds(0);
    }
    
private:
    mutable std::mutex mutex_;
    uint64_t totalAllocations_;
    uint64_t successfulAllocations_;
    uint64_t failedAllocations_;
    uint64_t totalDeallocations_;
    uint64_t currentAllocatedObjects_;
    uint64_t peakAllocatedObjects_;
    uint64_t totalObjectsAllocated_;
    uint64_t memoryUsageBytes_;
    uint64_t peakMemoryUsageBytes_;
    uint64_t cacheHits_;
    uint64_t cacheMisses_;
    std::chrono::nanoseconds totalAllocationTime_;
    std::chrono::nanoseconds totalDeallocationTime_;
};

/**
 * Comprehensive Performance Monitor
 * 
 * Integrates SIMD and memory pool monitoring with overall brain performance tracking
 * to provide complete performance analysis for the NLM neural simulation framework.
 */
class PerformanceMonitor {
public:
    struct ComprehensiveMetrics {
        // Core simulation metrics
        uint64_t totalSteps;                    // Total simulation steps executed
        double totalSimulationTimeSeconds;     // Total simulation time in seconds
        double realTimeFactor;                  // Simulation speed relative to real-time
        uint64_t totalSpikes;                   // Total spikes emitted
        uint64_t neuronsPerSecond;              // Neural computation rate
        uint64_t spikesPerSecond;               // Spike emission rate
        uint64_t synapticEventsPerSecond;       // Synaptic event rate
        
        // Performance metrics
        float averageStepTimeMs;                // Average step execution time
        float minStepTimeMs;                    // Minimum step time
        float maxStepTimeMs;                    // Maximum step time
        float stepTimeStandardDeviationMs;       // Step time variability
        
        // Resource usage metrics
        size_t memoryUsageMB;                   // Current memory usage in MB
        size_t peakMemoryUsageMB;               // Peak memory usage in MB
        uint32_t cpuUsagePercent;               // CPU usage percentage
        double memoryEfficiencyPercent;        // Memory usage efficiency
        
        // SIMD metrics
        SIMDProfiler::SIMDMetrics simdMetrics;  // SIMD-specific metrics
        
        // Memory pool metrics
        MemoryPoolMonitor::MemoryPoolMetrics neuronPoolMetrics;   // Neuron pool metrics
        MemoryPoolMonitor::MemoryPoolMetrics synapsePoolMetrics;   // Synapse pool metrics
        
        // Performance targets achievement
        double simdPerformanceTarget;           // SIMD performance target achievement (0.0-1.0)
        double memoryPoolTarget;                // Memory pool efficiency target achievement (0.0-1.0)
        double overallPerformanceTarget;       // Overall performance improvement target (0.0-1.0)
    };
    
    PerformanceMonitor()
        : stepCounter_(0),
          totalSteps_(0),
          startTime_(std::chrono::high_resolution_clock::now()),
          lastStepTime_(std::chrono::high_resolution_clock::now()),
          minStepTime_(std::chrono::nanoseconds::max()),
          maxStepTime_(std::chrono::nanoseconds(0)),
          simdProfiler_(),
          neuronPoolMonitor_(),
          synapsePoolMonitor_() {
        simdProfiler_.initialize();
    }
    
    /**
     * Record simulation step execution
     */
    void recordStep(double stepTimeMs) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto stepTime = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::duration<double, std::milli>(stepTimeMs));
        
        totalSteps_++;
        stepCounter_++;
        lastStepTime_ = std::chrono::high_resolution_clock::now();
        
        // Update min/max step times
        if (stepTime < minStepTime_) {
            minStepTime_ = stepTime;
        }
        if (stepTime > maxStepTime_) {
            maxStepTime_ = stepTime;
        }
        
        // Record step timing statistics
        stepTimes_.push_back(stepTime);
        
        // Keep only recent step times for rolling average
        if (stepTimes_.size() > 1000) {
            stepTimes_.erase(stepTimes_.begin());
        }
    }
    
    /**
     * Record spike emission
     */
    void recordSpike() {
        std::lock_guard<std::mutex> lock(mutex_);
        totalSpikes_++;
    }
    
    /**
     * Update SIMD profiler metrics
     */
    void updateSIMDMetrics() {
        simdProfiler_.updateFlops(getElapsedTimeSeconds());
    }
    
    /**
     * Get comprehensive performance metrics
     */
    ComprehensiveMetrics getMetrics() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        ComprehensiveMetrics metrics;
        
        // Core simulation metrics
        double elapsedSeconds = getElapsedTimeSeconds();
        metrics.totalSteps = totalSteps_;
        metrics.totalSimulationTimeSeconds = elapsedSeconds;
        metrics.realTimeFactor = (elapsedSeconds > 0.0) ? 
                                 (static_cast<double>(totalSteps_) * 0.001) / elapsedSeconds : 1.0;
        metrics.totalSpikes = totalSpikes_;
        
        if (elapsedSeconds > 0.0) {
            metrics.neuronsPerSecond = static_cast<uint64_t>(getTotalNeuronCount() / elapsedSeconds);
            metrics.spikesPerSecond = static_cast<uint64_t>(totalSpikes_ / elapsedSeconds);
        }
        
        // Performance metrics
        calculateStepTimeStatistics(metrics);
        
        // Resource usage metrics
        metrics.memoryUsageMB = static_cast<size_t>(getMemoryUsage() / (1024 * 1024));
        metrics.peakMemoryUsageMB = static_cast<size_t>(getPeakMemoryUsage() / (1024 * 1024));
        metrics.cpuUsagePercent = getCPUUsagePercent();
        metrics.memoryEfficiencyPercent = calculateMemoryEfficiency();
        
        // SIMD metrics
        metrics.simdMetrics = simdProfiler_.getMetrics();
        
        // Memory pool metrics
        metrics.neuronPoolMetrics = neuronPoolMonitor_.getMetrics();
        metrics.synapsePoolMetrics = synapsePoolMonitor_.getMetrics();
        
        // Performance target achievement
        metrics.simdPerformanceTarget = calculateSIMDTargetAchievement();
        metrics.memoryPoolTarget = calculateMemoryPoolTargetAchievement();
        metrics.overallPerformanceTarget = calculateOverallPerformanceTarget();
        
        return metrics;
    }
    
    /**
     * Calculate step time statistics
     */
    void calculateStepTimeStatistics(ComprehensiveMetrics& metrics) {
        if (stepTimes_.empty()) {
            metrics.averageStepTimeMs = 0.0f;
            metrics.minStepTimeMs = 0.0f;
            metrics.maxStepTimeMs = 0.0f;
            metrics.stepTimeStandardDeviationMs = 0.0f;
            return;
        }
        
        // Calculate statistics
        double sumMs = 0.0;
        for (const auto& stepTime : stepTimes_) {
            double ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(stepTime).count();
            sumMs += ms;
        }
        
        metrics.averageStepTimeMs = static_cast<float>(sumMs / stepTimes_.size());
        metrics.minStepTimeMs = static_cast<float>(std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(minStepTime_).count());
        metrics.maxStepTimeMs = static_cast<float>(std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(maxStepTime_).count());
        
        // Calculate standard deviation
        double variance = 0.0;
        for (const auto& stepTime : stepTimes_) {
            double ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(stepTime).count();
            variance += (ms - metrics.averageStepTimeMs) * (ms - metrics.averageStepTimeMs);
        }
        metrics.stepTimeStandardDeviationMs = static_cast<float>(std::sqrt(variance / stepTimes_.size()));
    }
    
    /**
     * Calculate SIMD performance target achievement
     */
    double calculateSIMDTargetAchievement() {
        SIMDProfiler::SIMDMetrics simdMetrics = simdProfiler_.getMetrics();
        double vectorizationEfficiency = simdMetrics.vectorizationEfficiency;
        
        // Target: Vectorization efficiency >= 0.8 for good SIMD utilization
        if (vectorizationEfficiency >= 0.8) {
            return 1.0;
        } else if (vectorizationEfficiency >= 0.6) {
            return 0.75;
        } else if (vectorizationEfficiency >= 0.4) {
            return 0.5;
        } else {
            return 0.25;
        }
    }
    
    /**
     * Calculate memory pool target achievement
     */
    double calculateMemoryPoolTargetAchievement() {
        MemoryPoolMonitor::MemoryPoolMetrics neuronMetrics = neuronPoolMonitor_.getMetrics();
        MemoryPoolMonitor::MemoryPoolMetrics synapseMetrics = synapsePoolMonitor_.getMetrics();
        
        // Target: Allocation overhead reduction >= 50%
        double neuronOverheadReduction = 1.0 - 
                                        (static_cast<double>(neuronMetrics.peakMemoryUsageBytes) / 
                                         neuronMetrics.totalObjectsAllocated);
        double synapseOverheadReduction = 1.0 - 
                                          (static_cast<double>(synapseMetrics.peakMemoryUsageBytes) / 
                                           synapseMetrics.totalObjectsAllocated);
        
        double avgOverheadReduction = (neuronOverheadReduction + synapseOverheadReduction) * 0.5;
        
        if (avgOverheadReduction >= 0.5) {
            return 1.0;
        } else if (avgOverheadReduction >= 0.4) {
            return 0.75;
        } else if (avgOverheadReduction >= 0.3) {
            return 0.5;
        } else {
            return 0.25;
        }
    }
    
    /**
     * Calculate overall performance target achievement
     */
    double calculateOverallPerformanceTarget() {
        double simdScore = calculateSIMDTargetAchievement();
        double memoryScore = calculateMemoryPoolTargetAchievement();
        double stepTimeScore = calculateStepTimeScore();
        
        return (simdScore * 0.4) + (memoryScore * 0.4) + (stepTimeScore * 0.2);
    }
    
    /**
     * Calculate step time score (faster steps are better)
     */
    double calculateStepTimeScore() {
        if (stepTimes_.empty()) return 0.0;
        
        double avgStepTimeMs = 0.0;
        double sumMs = 0.0;
        for (const auto& stepTime : stepTimes_) {
            double ms = std::chrono::duration_cast<std::chrono::duration<double, std::milli>>(stepTime).count();
            sumMs += ms;
        }
        avgStepTimeMs = sumMs / stepTimes_.size();
        
        // Score based on step time (target < 10ms per step)
        if (avgStepTimeMs < 10.0) {
            return 1.0;
        } else if (avgStepTimeMs < 20.0) {
            return 0.75;
        } else if (avgStepTimeMs < 50.0) {
            return 0.5;
        } else {
            return 0.25;
        }
    }
    
    /**
     * Get elapsed time since start
     */
    double getElapsedTimeSeconds() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - startTime_);
        return static_cast<double>(duration.count());
    }
    
    /**
     * Get current memory usage
     */
    size_t getMemoryUsage() const {
        // This would interface with system memory tracking
        // For now, return a placeholder value
        return 0;
    }
    
    /**
     * Get peak memory usage
     */
    size_t getPeakMemoryUsage() const {
        // This would interface with system memory tracking
        // For now, return a placeholder value
        return 0;
    }
    
    /**
     * Get CPU usage percentage
     */
    uint32_t getCPUUsagePercent() const {
        // This would interface with system CPU monitoring
        // For now, return a placeholder value
        return 0;
    }
    
    /**
     * Calculate memory efficiency
     */
    double calculateMemoryEfficiency() const {
        // This would calculate memory efficiency based on usage patterns
        // For now, return a placeholder value
        return 0.0;
    }
    
    /**
     * Get total neuron count from brain (if available)
     */
    size_t getTotalNeuronCount() const {
        // This would interface with the brain instance
        // For now, return a placeholder value
        return 0;
    }
    
    /**
     * Format comprehensive metrics as human-readable string
     */
    std::string formatMetrics(const ComprehensiveMetrics& metrics) const {
        std::stringstream ss;
        
        ss << "=== NLM Neural Simulation Framework - Performance Report ===" << std::endl;
        ss << std::endl;
        
        ss << "Core Simulation Metrics:" << std::endl;
        ss << "  Total Steps: " << metrics.totalSteps << std::endl;
        ss << "  Simulation Time: " << metrics.totalSimulationTimeSeconds << " seconds" << std::endl;
        ss << "  Real-time Factor: " << metrics.realTimeFactor << std::endl;
        ss << "  Total Spikes: " << metrics.totalSpikes << std::endl;
        ss << "  Neurons/second: " << metrics.neuronsPerSecond << std::endl;
        ss << "  Spikes/second: " << metrics.spikesPerSecond << std::endl;
        ss << "  Synaptic Events/second: " << metrics.synapticEventsPerSecond << std::endl;
        ss << std::endl;
        
        ss << "Performance Metrics:" << std::endl;
        ss << "  Average Step Time: " << metrics.averageStepTimeMs << " ms" << std::endl;
        ss << "  Min Step Time: " << metrics.minStepTimeMs << " ms" << std::endl;
        ss << "  Max Step Time: " << metrics.maxStepTimeMs << " ms" << std::endl;
        ss << "  Step Time Std Dev: " << metrics.stepTimeStandardDeviationMs << " ms" << std::endl;
        ss << std::endl;
        
        ss << "Resource Usage:" << std::endl;
        ss << "  Memory Usage: " << metrics.memoryUsageMB << " MB" << std::endl;
        ss << "  Peak Memory: " << metrics.peakMemoryUsageMB << " MB" << std::endl;
        ss << "  CPU Usage: " << static_cast<int>(metrics.cpuUsagePercent) << "%" << std::endl;
        ss << "  Memory Efficiency: " << (metrics.memoryEfficiencyPercent * 100.0) << "%" << std::endl;
        ss << std::endl;
        
        ss << "SIMD Performance:" << std::endl;
        ss << metrics.simdMetrics.vectorizationEfficiency * 100.0 << "% vectorization efficiency" << std::endl;
        ss << "  " << metrics.simdMetrics.actualFlops << " FLOPS" << std::endl;
        ss << "  Architecture: " << metrics.simdMetrics.simdArchitecture << std::endl;
        ss << std::endl;
        
        ss << "Memory Pool Performance:" << std::endl;
        ss << "  Neuron Pool Success Rate: " << 
           (metrics.neuronPoolMetrics.allocationSuccessRate * 100.0) << "%" << std::endl;
        ss << "  Synapse Pool Success Rate: " << 
           (metrics.synapsePoolMetrics.allocationSuccessRate * 100.0) << "%" << std::endl;
        ss << "  Memory Efficiency Score: " << (metrics.neuronPoolMetrics.memoryEfficiencyScore * 100.0) << "%" << std::endl;
        ss << std::endl;
        
        ss << "Performance Target Achievement:" << std::endl;
        ss << "  SIMD Performance Target: " << (metrics.simdPerformanceTarget * 100.0) << "%" << std::endl;
        ss << "  Memory Pool Target: " << (metrics.memoryPoolTarget * 100.0) << "%" << std::endl;
        ss << "  Overall Performance Target: " << (metrics.overallPerformanceTarget * 100.0) << "%" << std::endl;
        ss << std::endl;
        
        ss << "Performance Recommendations:" << std::endl;
        ss << simdProfiler_.getOptimizationRecommendation() << std::endl;
        ss << std::endl;
        
        return ss.str();
    }
    
    /**
     * Reset metrics for new benchmark run
     */
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        stepCounter_ = 0;
        totalSteps_ = 0;
        startTime_ = std::chrono::high_resolution_clock::now();
        lastStepTime_ = std::chrono::high_resolution_clock::now();
        minStepTime_ = std::chrono::nanoseconds::max();
        maxStepTime_ = std::chrono::nanoseconds(0);
        stepTimes_.clear();
        totalSpikes_ = 0;
        
        simdProfiler_.reset();
        neuronPoolMonitor_.reset();
        synapsePoolMonitor_.reset();
    }
    
    /**
     * Log performance metrics to file
     */
    bool logMetrics(const std::string& filename) {
        try {
            std::ofstream file(filename);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Failed to open performance log file: " + filename);
                return false;
            }
            
            ComprehensiveMetrics metrics = getMetrics();
            file << formatMetrics(metrics);
            file.close();
            
            NLM_LOG_INFO("Performance metrics logged to: " + filename);
            return true;
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception logging performance metrics: ") + e.what());
            return false;
        }
    }
    
    /**
     * Get SIMD profiler reference
     */
    SIMDProfiler& getSIMDProfiler() { return simdProfiler_; }
    
    /**
     * Get neuron pool monitor reference
     */
    MemoryPoolMonitor& getNeuronPoolMonitor() { return neuronPoolMonitor_; }
    
    /**
     * Get synapse pool monitor reference
     */
    MemoryPoolMonitor& getSynapsePoolMonitor() { return synapsePoolMonitor_; }
    
    /**
     * Record allocation in neuron pool monitor
     */
    void recordNeuronAllocation(bool success, uint64_t objectSize = 0) {
        auto start = std::chrono::high_resolution_clock::now();
        neuronPoolMonitor_.recordAllocation(success, objectSize);
        if (success) {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            neuronPoolMonitor_.recordAllocation(success, objectSize, duration);
        }
    }
    
    /**
     * Record deallocation in neuron pool monitor
     */
    void recordNeuronDeallocation(uint64_t objectSize = 0) {
        auto start = std::chrono::high_resolution_clock::now();
        neuronPoolMonitor_.recordDeallocation(objectSize);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        neuronPoolMonitor_.recordDeallocation(objectSize, duration);
    }
    
    /**
     * Record allocation in synapse pool monitor
     */
    void recordSynapseAllocation(bool success, uint64_t objectSize = 0) {
        auto start = std::chrono::high_resolution_clock::now();
        synapsePoolMonitor_.recordAllocation(success, objectSize);
        if (success) {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            synapsePoolMonitor_.recordAllocation(success, objectSize, duration);
        }
    }
    
    /**
     * Record deallocation in synapse pool monitor
     */
    void recordSynapseDeallocation(uint64_t objectSize = 0) {
        auto start = std::chrono::high_resolution_clock::now();
        synapsePoolMonitor_.recordDeallocation(objectSize);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        synapsePoolMonitor_.recordDeallocation(objectSize, duration);
    }
    
private:
    mutable std::mutex mutex_;
    uint64_t stepCounter_;
    uint64_t totalSteps_;
    TimePoint startTime_;
    TimePoint lastStepTime_;
    std::chrono::nanoseconds minStepTime_;
    std::chrono::nanoseconds maxStepTime_;
    std::vector<std::chrono::nanoseconds> stepTimes_;
    uint64_t totalSpikes_;
    
    SIMDProfiler simdProfiler_;
    MemoryPoolMonitor neuronPoolMonitor_;
    MemoryPoolMonitor synapsePoolMonitor_;
};

} // namespace nlm

// Performance monitoring macros for easy integration

#define NLM_PERF_RECORD_STEP() \
    static nlm::PerformanceMonitor perfMonitor; \
    static auto stepStart = std::chrono::high_resolution_clock::now();

#define NLM_PERF_END_STEP() \
    { \
        auto stepEnd = std::chrono::high_resolution_clock::now(); \
        auto stepDuration = std::chrono::duration_cast<std::chrono::milliseconds>(stepEnd - stepStart); \
        perfMonitor.recordStep(static_cast<double>(stepDuration.count())); \
        stepStart = std::chrono::high_resolution_clock::now(); \
    }

#define NLM_PERF_RECORD_SPIKE() \
    perfMonitor.recordSpike()

#define NLM_PERF_LOG_METRICS(filename) \
    perfMonitor.logMetrics(filename)

#define NLM_PERF_GET_METRICS() \
    perfMonitor.getMetrics()

#define NLM_PERF_RESET() \
    perfMonitor.reset()

// SIMD performance macros
#define NLM_SIMD_VECTOR_OPERATION() \
    simdProfiler.recordVectorOperation()

#define NLM_SIMD_SCALAR_FALLBACK() \
    simdProfiler.recordScalarOperation()

// Memory pool performance macros
#define NLM_POOL_ALLOCATE(type, size) \
    do { \
        bool success = /* allocation logic */; \
        perfMonitor.record##type##Allocation(success, size); \
        if (success) { /* allocate object */ } \
    } while(0)

#define NLM_POOL_DEALLOCATE(type, size) \
    do { \
        perfMonitor.record##type##Deallocation(size); \
        /* deallocate object */ \
    } while(0)

// Performance benchmarking utilities
namespace nlm::benchmark {

    /**
     * Benchmark different optimization levels
     */
    class BenchmarkSuite {
    public:
        struct BenchmarkResults {
            std::string name;
            double executionTimeSeconds;
            uint64_t operationsCount;
            double throughput;  // Operations per second
            double efficiency;   // Performance relative to baseline
        };
        
        BenchmarkSuite() = default;
        
        /**
         * Run benchmark with specified parameters
         */
        BenchmarkResults runBenchmark(const std::string& name,
                                      std::function<void()> benchmarkFunction,
                                      uint64_t iterations = 100) {
            PerformanceCounter counter;
            
            for (uint64_t i = 0; i < iterations; ++i) {
                benchmarkFunction();
            }
            
            double elapsedSeconds = counter.elapsedSeconds();
            uint64_t operations = iterations * /* operations per benchmark */;
            
            BenchmarkResults results;
            results.name = name;
            results.executionTimeSeconds = elapsedSeconds;
            results.operationsCount = operations;
            results.throughput = (operations > 0) ? (operations / elapsedSeconds) : 0.0;
            results.efficiency = 0.0; // Calculate relative to baseline if available
            
            return results;
        }
        
        /**
         * Compare performance between different implementations
         */
        std::vector<BenchmarkResults> compareImplementations(
            const std::vector<std::function<void()>>& implementations,
            const std::vector<std::string>& names,
            uint64_t iterations = 100) {
            
            std::vector<BenchmarkResults> results;
            double baselineTime = std::numeric_limits<double>::max();
            
            // First run all implementations to get baseline
            std::vector<double> times;
            for (const auto& impl : implementations) {
                PerformanceCounter counter;
                impl();
                double elapsed = counter.elapsedSeconds();
                times.push_back(elapsed);
                if (elapsed < baselineTime) baselineTime = elapsed;
            }
            
            // Calculate results
            for (size_t i = 0; i < implementations.size(); ++i) {
                BenchmarkResults result;
                result.name = names[i];
                result.executionTimeSeconds = times[i];
                result.throughput = (times[i] > 0) ? (100.0 / times[i]) : 0.0; // 100 operations per benchmark
                result.efficiency = (baselineTime > 0) ? (baselineTime / times[i]) : 1.0;
                results.push_back(result);
            }
            
            return results;
        }
        
        /**
         * Format benchmark results
         */
        std::string formatResults(const std::vector<BenchmarkResults>& results) const {
            std::stringstream ss;
            
            ss << "Benchmark Results:" << std::endl;
            ss << std::setw(20) << std::left << "Implementation"
               << std::setw(15) << std::left << "Time (s)"
               << std::setw(15) << std::left << "Throughput"
               << std::setw(12) << std::left << "Efficiency" << std::endl;
            
            for (const auto& result : results) {
                ss << std::setw(20) << std::left << result.name
                   << std::setw(15) << std::left << std::fixed << std::setprecision(6)
                   << result.executionTimeSeconds
                   << std::setw(15) << std::left << std::fixed << std::setprecision(2)
                   << result.throughput
                   << std::setw(12) << std::left << std::fixed << std::setprecision(3)
                   << result.efficiency << std::endl;
            }
            
            return ss.str();
        }
    };
    
} // namespace nlm::benchmark

// Resource-aware optimization selection
namespace nlm::optimization {

    /**
     * Determine optimal optimization level based on system resources
     */
    class ResourceAwareOptimizer {
    public:
        enum class OptimizationLevel {
            SCALAR_ONLY,      // Pure scalar, maximum compatibility
            SIMD_ONLY,       // SIMD acceleration only
            MEMORY_POOLS,    // Memory pools only
            FULL             // All optimizations enabled
        };
        
        struct SystemResources {
            uint64_t availableMemory;        // Available memory in bytes
            uint32_t cpuCoreCount;          // Number of CPU cores
            double memorySpeedGBPS;         // Memory bandwidth
            bool hasAVX512;                // AVX-512 support
            bool hasAVX2;                  // AVX2 support
            bool hasSSE;                   // SSE support
            uint64_t cacheLineSize;        // Cache line size in bytes
        };
        
        struct OptimizationRecommendation {
            OptimizationLevel level;
            double expectedSpeedup;          // Expected performance improvement
            double memoryOverhead;           // Memory overhead
            std::string rationale;          // Reason for recommendation
        };
        
        ResourceAwareOptimizer() = default;
        
        /**
         * Detect system resources
         */
        SystemResources detectSystemResources() const {
            SystemResources resources;
            
            // Detect CPU features (simplified - would use CPU detection in real implementation)
            resources.hasAVX512 = SIMDUtils::isSIMDAvailable();
            resources.hasAVX2 = SIMDUtils::isSIMDAvailable();
            resources.hasSSE = SIMDUtils::isSIMDAvailable();
            resources.cpuCoreCount = std::thread::hardware_concurrency();
            resources.cacheLineSize = 64; // Typical cache line size
            
            // Estimate memory (simplified)
            resources.availableMemory = 2LL * 1024 * 1024 * 1024; // 2GB estimate
            resources.memorySpeedGBPS = 25.0; // 25 GB/s estimate
            
            return resources;
        }
        
        /**
         * Recommend optimal optimization level
         */
        OptimizationRecommendation recommendOptimizationLevel(
            const SystemResources& resources,
            const PerformanceMonitor::ComprehensiveMetrics& currentMetrics) const {
            
            // Calculate scores for each optimization level
            double scalarScore = calculateScalarScore(resources, currentMetrics);
            double simdScore = calculateSIMDScore(resources, currentMetrics);
            double poolScore = calculatePoolScore(resources, currentMetrics);
            double fullScore = simdScore + poolScore;
            
            // Select best option
            OptimizationLevel bestLevel;
            double bestScore = std::max({scalarScore, simdScore, poolScore, fullScore});
            
            if (bestScore == scalarScore) {
                bestLevel = OptimizationLevel::SCALAR_ONLY;
            } else if (bestScore == simdScore) {
                bestLevel = OptimizationLevel::SIMD_ONLY;
            } else if (bestScore == poolScore) {
                bestLevel = OptimizationLevel::MEMORY_POOLS;
            } else {
                bestLevel = OptimizationLevel::FULL;
            }
            
            // Calculate expected speedup
            double expectedSpeedup = 1.0; // Base speed (no optimization)
            switch (bestLevel) {
                case OptimizationLevel::SCALAR_ONLY:
                    expectedSpeedup = 1.0;
                    break;
                case OptimizationLevel::SIMD_ONLY:
                    expectedSpeedup = 2.5; // 2-4x speedup for SIMD
                    break;
                case OptimizationLevel::MEMORY_POOLS:
                    expectedSpeedup = 1.8; // 50-90% allocation overhead reduction
                    break;
                case OptimizationLevel::FULL:
                    expectedSpeedup = 2.0; // Overall simulation improvement
                    break;
            }
            
            // Calculate memory overhead
            double memoryOverhead = 0.0;
            switch (bestLevel) {
                case OptimizationLevel::SCALAR_ONLY:
                    memoryOverhead = 0.0;
                    break;
                case OptimizationLevel::SIMD_ONLY:
                    memoryOverhead = 0.05; // 5% overhead for SIMD alignment
                    break;
                case OptimizationLevel::MEMORY_POOLS:
                    memoryOverhead = 0.15; // 15% overhead for pools
                    break;
                case OptimizationLevel::FULL:
                    memoryOverhead = 0.25; // 25% overhead for all optimizations
                    break;
            }
            
            std::string rationale;
            switch (bestLevel) {
                case OptimizationLevel::SCALAR_ONLY:
                    rationale = "Limited memory or CPU resources. Scalar implementation provides maximum compatibility.";
                    break;
                case OptimizationLevel::SIMD_ONLY:
                    rationale = "Good SIMD support and sufficient memory. SIMD acceleration provides best performance.";
                    break;
                case OptimizationLevel::MEMORY_POOLS:
                    rationale = "Memory bandwidth limited. Memory pools reduce allocation overhead without SIMD penalties.";
                    break;
                case OptimizationLevel::FULL:
                    rationale = "High-end system with ample resources. Full optimization suite provides maximum performance.";
                    break;
            }
            
            return OptimizationRecommendation{
                bestLevel,
                expectedSpeedup,
                memoryOverhead,
                rationale
            };
        }
        
    private:
        double calculateScalarScore(const SystemResources& resources,
                                    const PerformanceMonitor::ComprehensiveMetrics& metrics) const {
            return 1.0; // Base score
        }
        
        double calculateSIMDScore(const SystemResources& resources,
                                  const PerformanceMonitor::ComprehensiveMetrics& metrics) const {
            double score = 0.0;
            
            if (resources.hasAVX512 || resources.hasAVX2 || resources.hasSSE) {
                score += 1.0;
            }
            
            double vectorizationEfficiency = metrics.simdMetrics.vectorizationEfficiency;
            score += vectorizationEfficiency;
            
            return score;
        }
        
        double calculatePoolScore(const SystemResources& resources,
                                  const PerformanceMonitor::ComprehensiveMetrics& metrics) const {
            double score = 0.0;
            
            // Check memory availability
            if (resources.availableMemory >= 1024 * 1024 * 1024) { // >= 1GB
                score += 0.5;
            }
            
            // Check memory efficiency
            double memoryEfficiency = metrics.memoryEfficiencyPercent / 100.0;
            score += memoryEfficiency * 0.5;
            
            return score;
        }
    };
    
} // namespace nlm::optimization

// Backward compatibility layer
namespace nlm {

    /**
     * Backward compatibility wrapper for original scalar implementations
     */
    class BackwardCompatibilityLayer {
    public:
        BackwardCompatibilityLayer()
            : originalPerformanceMonitor_(),
              simdAvailable_(SIMDUtils::isSIMDAvailable()),
              currentOptimizationLevel_(1) {} // 1 = scalar (original)
        
        /**
         * Execute with full optimization (new SIMD + memory pools)
         */
        void executeWithOptimization(std::function<void()> optimizedFunction) {
            if (currentOptimizationLevel_ > 1) {
                // Use optimized implementation
                optimizedFunction();
            } else {
                // Fall back to original scalar implementation
                executeWithScalarFallback(optimizedFunction);
            }
        }
        
        /**
         * Execute with scalar fallback
         */
        void executeWithScalarFallback(std::function<void()> scalarFunction) {
            scalarFunction();
        }
        
        /**
         * Enable or disable SIMD optimizations
         */
        void enableSIMD(bool enable) {
            simdEnabled_ = enable;
            updateOptimizationLevel();
        }
        
        /**
         * Enable or disable memory pools
         */
        void enableMemoryPools(bool enable) {
            memoryPoolsEnabled_ = enable;
            updateOptimizationLevel();
        }
        
        /**
         * Set optimization level
         */
        void setOptimizationLevel(int level) {
            currentOptimizationLevel_ = std::max(1, std::min(level, 3));
        }
        
        /**
         * Get current optimization level
         */
        int getOptimizationLevel() const { return currentOptimizationLevel_; }
        
        /**
         * Check if SIMD is available
         */
        bool isSIMDAvailable() const { return simdAvailable_; }
        
        /**
         * Get performance metrics
         */
        PerformanceMonitor::ComprehensiveMetrics getMetrics() const {
            return originalPerformanceMonitor_.getMetrics();
        }
        
        /**
         * Log performance metrics
         */
        bool logMetrics(const std::string& filename) {
            return originalPerformanceMonitor_.logMetrics(filename);
        }
        
        /**
         * Reset metrics
         */
        void reset() {
            originalPerformanceMonitor_.reset();
        }
        
    private:
        void updateOptimizationLevel() {
            int level = 1; // Start with scalar
            if (simdEnabled_ && simdAvailable_) {
                level = 2; // Add SIMD
            }
            if (memoryPoolsEnabled_) {
                level = 3; // Add memory pools
            }
            currentOptimizationLevel_ = level;
        }
        
        PerformanceMonitor originalPerformanceMonitor_;
        bool simdAvailable_;
        bool simdEnabled_;
        bool memoryPoolsEnabled_;
        int currentOptimizationLevel_;
    };
    
    // Global backward compatibility instance
    inline BackwardCompatibilityLayer& getBackwardCompatibilityLayer() {
        static BackwardCompatibilityLayer layer;
        return layer;
    }
    
    // Convenience functions for backward compatibility
    inline void enableSIMD(bool enable) {
        getBackwardCompatibilityLayer().enableSIMD(enable);
    }
    
    inline void enableMemoryPools(bool enable) {
        getBackwardCompatibilityLayer().enableMemoryPools(enable);
    }
    
    inline void setOptimizationLevel(int level) {
        getBackwardCompatibilityLayer().setOptimizationLevel(level);
    }
    
    inline int getOptimizationLevel() {
        return getBackwardCompatibilityLayer().getOptimizationLevel();
    }
    
    inline PerformanceMonitor::ComprehensiveMetrics getPerformanceMetrics() {
        return getBackwardCompatibilityLayer().getMetrics();
    }
    
    inline bool logPerformanceMetrics(const std::string& filename) {
        return getBackwardCompatibilityLayer().logMetrics(filename);
    }
    
} // namespace nlm

// Integration macros for seamless backward compatibility
#define NLM_PERF_INIT() \
    static nlm::BackwardCompatibilityLayer perfLayer;

#define NLM_PERF_OPTIMIZE(function) \
    perfLayer.executeWithOptimization(function)

#define NLM_PERF_SCALAR_ONLY(function) \
    perfLayer.executeWithScalarFallback(function)

#define NLM_PERF_SET_OPTIMIZATION_LEVEL(level) \
    perfLayer.setOptimizationLevel(level)

#define NLM_PERF_LOG_METRICS(filename) \
    perfLayer.logMetrics(filename)

#endif // NLM_PERFORMANCE_MONITORING_H