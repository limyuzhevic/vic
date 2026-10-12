#pragma once

/**
 * Performance Infrastructure
 * 
 * Phase 5 introduces comprehensive performance optimization for large-scale neural simulation:
 * 
 * 1. Memory Pool Allocators
 *    - Pre-allocated neuron and synapse pools
 *    - Reduced allocation overhead
 *    - Better cache locality
 * 
 * 2. Event Queue Optimization
 *    - Lock-free ring buffers for spike events
 *    - Time-bucketed delayed spikes
 *    - Priority queues for time-sensitive events
 * 
 * 3. Structure-of-Arrays (SoA) Data Layout
 *    - Contiguous memory for neuron states
 *    - Better SIMD vectorization
 *    - Improved cache utilization
 * 
 * 4. Sparse Connectivity
 *    - Adjacency lists instead of dense matrices
 *    - Efficient synapse lookup
 *    - Memory-efficient storage for large networks
 * 
 * 5. Multithreading Support
 *    - Parallel neural population updates
 *    - Thread-local neuron processing
 *    - Synchronized spike processing
 * 
 * This infrastructure is designed to scale from 10k to 100M+ neurons.
 */

#ifndef NLM_PERFORMANCE_HPP
#define NLM_PERFORMANCE_HPP

#include <cstdint>
#include <chrono>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

namespace nlm {

/**
 * Runtime performance configuration
 * 
 * Allows changing performance optimization settings at runtime,
 * overriding compile-time flags for adaptive behavior.
 */
struct RuntimePerformanceConfig {
    // Core performance optimization flags
    bool useMemoryPool;
    bool useSOA;
    bool useEventQueue;
    bool useSparseConnectivity;
    bool useMultithreading;
    bool useSIMD;
    
    // Runtime configuration parameters
    size_t threadCount;
    size_t eventQueueCapacity;
    size_t memoryPoolBlockSize;
    size_t memoryPoolInitialBlocks;
    size_t maxDelayedSpikeBuckets;
    uint64_t maxDelaySteps;
    
    // SIMD runtime configuration
    bool preferAVX512;
    bool preferAVX2;
    bool preferSSE;
    
    // Performance budgeting
    double timeBudgetPerStep;        // Maximum time allowed per simulation step (seconds)
    double memoryBudgetMB;           // Maximum memory usage (MB)
    size_t eventBudgetPerStep;       // Maximum events per simulation step
    
    // Adaptive optimization parameters
    double workloadThreshold;        // CPU usage threshold for optimization
    double memoryPressureThreshold;  // Memory usage threshold for optimization
    size_t minThreadsToEnableMT;     // Minimum threads required for multithreading
    
    // Logging and profiling
    bool enableDetailedProfiling;
    bool enablePerformanceLogging;
    bool enableEventBatching;
    bool enableEventCompression;
    
    RuntimePerformanceConfig()
        : useMemoryPool(true)
        , useSOA(true)
        , useEventQueue(true)
        , useSparseConnectivity(true)
        , useMultithreading(true)
        , useSIMD(true)
        , threadCount(std::max<size_t>(1, std::thread::hardware_concurrency()))
        , eventQueueCapacity(16384)
        , memoryPoolBlockSize(1024)
        , memoryPoolInitialBlocks(4)
        , maxDelayedSpikeBuckets(1024)
        , maxDelaySteps(100)
        , preferAVX512(true)
        , preferAVX2(true)
        , preferSSE(true)
        , timeBudgetPerStep(1.0)        // 1 second per step
        , memoryBudgetMB(1024.0)       // 1 GB
        , eventBudgetPerStep(10000)    // 10k events per step
        , workloadThreshold(0.7)       // Optimize when CPU usage > 70%
        , memoryPressureThreshold(0.8) // Optimize when memory > 80%
        , minThreadsToEnableMT(2)     // Need at least 2 threads
        , enableDetailedProfiling(true)
        , enablePerformanceLogging(false)
        , enableEventBatching(true)
        , enableEventCompression(true) {}
};

/**
 * Performance metrics collection
 * 
 * Real-time performance monitoring and statistics.
 */
struct PerformanceMetrics {
    // Timing metrics
    double stepTimeMs;              // Time per simulation step
    double simulationTimeMs;        // Total simulation time
    double cpuUsagePercent;         // CPU utilization percentage
    
    // Event processing metrics
    uint64_t spikesProcessed;        // Number of spikes processed
    uint64_t spikesGenerated;        // Number of spikes generated
    uint64_t delayedSpikesScheduled; // Number of delayed spikes
    uint64_t delayedSpikesProcessed; // Number of delayed spikes processed
    uint64_t eventQueueSize;         // Current event queue size
    uint64_t eventQueueCapacity;     // Event queue capacity
    
    // Memory metrics
    size_t memoryUsageBytes;        // Current memory usage
    size_t memoryPoolUsage;          // Memory pool allocated objects
    size_t synapticMemoryBytes;      // Synaptic memory usage
    
    // Computation metrics
    uint64_t neuronsProcessed;       // Number of neurons processed
    uint64_t neuronsFired;           // Number of neurons that fired
    uint64_t synapsesUpdated;        // Number of synapses updated
    uint64_t plasticityOperations;   // Number of plasticity operations
    
    // SIMD metrics
    bool simdAvailable;             // SIMD availability
    const char* simdType;            // SIMD instruction set type
    uint64_t simdOperations;         // Number of SIMD operations
    
    // Threading metrics
    size_t activeThreads;            // Number of active threads
    size_t totalThreads;             // Total threads used
    uint64_t workSteals;             // Number of work steals
    uint64_t loadImbalance;          // Load imbalance metric
    
    // Quality metrics
    double firingRateHz;             // Average firing rate
    double synapticEfficiency;       // Synaptic update efficiency
    double cacheEfficiency;          // Cache access efficiency
    
    PerformanceMetrics()
        : stepTimeMs(0.0)
        , simulationTimeMs(0.0)
        , cpuUsagePercent(0.0)
        , spikesProcessed(0)
        , spikesGenerated(0)
        , delayedSpikesScheduled(0)
        , delayedSpikesProcessed(0)
        , eventQueueSize(0)
        , eventQueueCapacity(0)
        , memoryUsageBytes(0)
        , memoryPoolUsage(0)
        , synapticMemoryBytes(0)
        , neuronsProcessed(0)
        , neuronsFired(0)
        , synapsesUpdated(0)
        , plasticityOperations(0)
        , simdAvailable(false)
        , simdType("Scalar")
        , simdOperations(0)
        , activeThreads(0)
        , totalThreads(0)
        , workSteals(0)
        , loadImbalance(0)
        , firingRateHz(0.0)
        , synapticEfficiency(0.0)
        , cacheEfficiency(0.0) {}
};

/**
 * Performance budget management
 * 
 * Tracks and enforces performance budgets for resource management.
 */
class PerformanceBudget {
public:
    PerformanceBudget(double timeBudgetPerStep = 1.0, 
                     double memoryBudgetMB = 1024.0,
                     size_t eventBudgetPerStep = 10000);
    
    ~PerformanceBudget() = default;
    
    // Disable copying
    PerformanceBudget(const PerformanceBudget&) = delete;
    PerformanceBudget& operator=(const PerformanceBudget&) = delete;
    
    // Check if current step is within budget
    bool isStepWithinBudget(double stepTimeMs, size_t eventsProcessed, size_t memoryBytes) const;
    
    // Update budgets based on actual usage
    void updateBudget(double stepTimeMs, size_t eventsProcessed, size_t memoryBytes);
    
    // Get current budget status
    double getRemainingTimeBudget() const;
    size_t getRemainingEventBudget() const;
    size_t getRemainingMemoryBudget() const;
    
    // Adjust budgets dynamically
    void setTimeBudgetPerStep(double budget);
    void setMemoryBudgetMB(double budget);
    void setEventBudgetPerStep(size_t budget);
    
    // Get configuration
    double getTimeBudgetPerStep() const { return timeBudgetPerStep_; }
    double getMemoryBudgetMB() const { return memoryBudgetMB_; }
    size_t getEventBudgetPerStep() const { return eventBudgetPerStep_; }
    
    // Budget status
    bool isTimeBudgetExceeded() const { return timeBudgetExceeded_; }
    bool isMemoryBudgetExceeded() const { return memoryBudgetExceeded_; }
    bool isEventBudgetExceeded() const { return eventBudgetExceeded_; }
    
private:
    double timeBudgetPerStep_;        // Time budget per simulation step
    double memoryBudgetMB_;           // Memory budget in MB
    size_t eventBudgetPerStep_;       // Event budget per simulation step
    
    // Current usage tracking
    double stepTimeUsed_;             // Time used in current step
    size_t eventsUsed_;               // Events used in current step
    size_t memoryUsed_;               // Memory used (bytes)
    
    // Budget status
    bool timeBudgetExceeded_;         // Time budget exceeded flag
    bool memoryBudgetExceeded_;       // Memory budget exceeded flag
    bool eventBudgetExceeded_;        // Event budget exceeded flag
    
    std::chrono::steady_clock::time_point stepStartTime_;
};

/**
 * Performance profiler
 * 
 * Detailed performance measurement and analysis for optimization.
 */
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler() = default;
    
    // Disable copying
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
    // Start/stop profiling
    void startProfiling(const std::string& operationName);
    void stopProfiling(const std::string& operationName);
    
    // Profile a function call
    template<typename Func, typename... Args>
    auto profile(const std::string& operationName, Func&& func, Args&&... args);
    
    // Get performance statistics
    double getAverageTime(const std::string& operationName) const;
    double getMinTime(const std::string& operationName) const;
    double getMaxTime(const std::string& operationName) const;
    size_t getCallCount(const std::string& operationName) const;
    
    // Get overall statistics
    const PerformanceMetrics& getOverallMetrics() const { return overallMetrics_; }
    
    // Get all metrics
    const std::unordered_map<std::string, PerformanceMetrics>& getDetailedMetrics() const { return detailedMetrics_; }
    
    // Reset profiler state
    void reset();
    
    // Enable/disable profiling
    void enable(bool enable) { enabled_ = enable; }
    bool isEnabled() const { return enabled_; }
    
    // Get singleton instance
    static PerformanceProfiler& getInstance() {
        static PerformanceProfiler instance;
        return instance;
    }
    
private:
    struct OperationMetrics {
        double totalTime;
        double minTime;
        double maxTime;
        size_t callCount;
        std::chrono::steady_clock::time_point startTime;
        
        OperationMetrics() : totalTime(0.0), minTime(std::numeric_limits<double>::max()),
                            maxTime(0.0), callCount(0) {}
    };
    
    bool enabled_;
    PerformanceMetrics overallMetrics_;
    std::unordered_map<std::string, PerformanceMetrics> detailedMetrics_;
    std::unordered_map<std::string, OperationMetrics> operationMetrics_;
    bool isProfiling_;
    std::string currentOperationName_;
};

/**
 * Runtime SIMD engine
 * 
 * Detects available SIMD instruction sets at runtime and dispatches
 * to the most appropriate optimized implementation.
 */
class SIMDEngine {
public:
    SIMDEngine();
    ~SIMDEngine() = default;
    
    // Disable copying
    SIMDEngine(const SIMDEngine&) = delete;
    SIMDEngine& operator=(const SIMDEngine&) = delete;
    
    // Detect available SIMD instruction sets
    void detectSIMDCapabilities();
    
    // Check if specific SIMD is available
    bool hasAVX512() const { return hasAVX512_; }
    bool hasAVX2() const { return hasAVX2_; }
    bool hasSSE() const { return hasSSE_; }
    
    // Get best available SIMD level
    const char* getBestSIMD() const;
    
    // SIMD detection callbacks
    void setAVX512Detector(std::function<bool()> detector);
    void setAVX2Detector(std::function<bool()> detector);
    void setSSEDetector(std::function<bool()> detector);
    
    // Runtime SIMD dispatch
    template<typename ScalarFunc, typename VectorFunc>
    auto dispatchSIMD(ScalarFunc scalarFunc, VectorFunc vectorFunc);
    
    // SIMD operation counters
    uint64_t getSIMDOperationsCount() const { return simdOperations_; }
    void incrementSIMDOperations() { ++simdOperations_; }
    
    // SIMD configuration
    void setPreferAVX512(bool prefer) { preferAVX512_ = prefer; }
    void setPreferAVX2(bool prefer) { preferAVX2_ = prefer; }
    void setPreferSSE(bool prefer) { preferSSE_ = prefer; }
    
    // Get SIMD type
    const char* getSIMDType() const;
    
    // Get singleton instance
    static SIMDEngine& getInstance() {
        static SIMDEngine instance;
        return instance;
    }
    
private:
    bool hasAVX512_;
    bool hasAVX2_;
    bool hasSSE_;
    bool preferAVX512_;
    bool preferAVX2_;
    bool preferSSE_;
    uint64_t simdOperations_;
    std::function<bool()> avx512Detector_;
    std::function<bool()> avx2Detector_;
    std::function<bool()> sseDetector_;
};

/**
 * Adaptive processor
 * 
 * Runtime optimization based on workload characteristics and performance metrics.
 */
class AdaptiveProcessor {
public:
    AdaptiveProcessor();
    ~AdaptiveProcessor() = default;
    
    // Disable copying
    AdaptiveProcessor(const AdaptiveProcessor&) = delete;
    AdaptiveProcessor& operator=(const AdaptiveProcessor&) = delete;
    
    // Update configuration based on current metrics
    void updateConfiguration(const PerformanceMetrics& metrics);
    
    // Get optimized configuration
    RuntimePerformanceConfig getOptimalConfiguration(const PerformanceMetrics& metrics) const;
    
    // Determine if optimizations should be enabled
    bool shouldEnableMultithreading(const PerformanceMetrics& metrics) const;
    bool shouldEnableSIMD(const PerformanceMetrics& metrics) const;
    bool shouldEnableEventDriven(const PerformanceMetrics& metrics) const;
    bool shouldEnableSparseConnectivity(const PerformanceMetrics& metrics) const;
    
    // Get adaptive thresholds
    double getWorkloadThreshold() const { return workloadThreshold_; }
    double getMemoryPressureThreshold() const { return memoryPressureThreshold_; }
    
    // Set thresholds
    void setWorkloadThreshold(double threshold) { workloadThreshold_ = threshold; }
    void setMemoryPressureThreshold(double threshold) { memoryPressureThreshold_ = threshold; }
    
    // Get singleton instance
    static AdaptiveProcessor& getInstance() {
        static AdaptiveProcessor instance;
        return instance;
    }
    
private:
    RuntimePerformanceConfig defaultConfig_;
    double workloadThreshold_;
    double memoryPressureThreshold_;
    size_t minThreadsToEnableMT_;
};

// Global configuration function
void setRuntimePerformanceConfig(const RuntimePerformanceConfig& config);
RuntimePerformanceConfig getRuntimePerformanceConfig();

// Global performance metrics access
PerformanceMetrics getCurrentPerformanceMetrics();

// Global configuration function
void setRuntimePerformanceConfig(const RuntimePerformanceConfig& config);
RuntimePerformanceConfig getRuntimePerformanceConfig();

// Set current performance metrics
define void setCurrentPerformanceMetrics(const PerformanceMetrics& metrics);

// Get current performance metrics
PerformanceMetrics getCurrentPerformanceMetrics();

// Performance monitoring functions
define void startPerformanceMonitoring();
define void stopPerformanceMonitoring();
define bool isPerformanceMonitoringActive();

} // namespace nlm

#endif // NLM_PERFORMANCE_HPP