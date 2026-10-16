#pragma once

/**
 * Performance Profiling Tools for NLM Brain System
 * 
 * Comprehensive performance monitoring and analysis infrastructure for neural computation
 * Identifies bottlenecks, tracks memory usage, and collects performance metrics
 * Maintains high-performance characteristics with minimal overhead
 * 
 * Features:
 * - Real-time performance monitoring during brain operation
 * - Memory allocation tracking and leak detection
 * - Performance metrics collection for optimization analysis
 * - Performance profiling tools for development and testing
 * - Integration with existing brain systems with minimal overhead
 */

#include "src/core/Types/Types.hpp"
#include "src/performance/MemoryPool.hpp"
#include <chrono>
#include <unordered_map>
#include <vector>
#include <array>
#include <atomic>
#include <string>
#include <memory>
#include <unordered_set>
#include <thread>
#include <mutex>
#include <deque>
#include <functional>

namespace nlm {

/**
 * High-resolution performance timer for measuring operation durations
 */
class PerformanceTimer {
public:
    PerformanceTimer() : startTime_(std::chrono::high_resolution_clock::now()) {}
    
    // Get elapsed time in microseconds
    uint64_t elapsedMicroseconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        return duration.count();
    }
    
    // Get elapsed time in nanoseconds
    uint64_t elapsedNanoseconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime_);
        return duration.count();
    }
    
    // Get elapsed time in milliseconds
    double elapsedMilliseconds() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
        return duration.count();
    }
    
    // Reset timer
    void reset() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
};

/**
 * Operation profiler for measuring individual neural computation performance
 */
class OperationProfiler {
public:
    enum class OperationType {
        NEURON_UPDATE,
        SYNAPSE_UPDATE,
        PLASTICITY_APPLY,
        SPIKE_PROCESSING,
        MEMORY_ACCESS,
        PREDICTION_COMPUTATION,
        COGNITIVE_PROCESSING,
        NEUROMODULATION,
        SENSOR_INPUT,
        MOTOR_OUTPUT,
        MEMORY_ALLOCATION,
        MEMORY_DEALLOCATION,
        UNKNOWN
    };
    
    struct ProfilingData {
        OperationType operationType;
        std::string operationName;
        uint64_t callCount;
        uint64_t totalTimeMicroseconds;
        uint64_t minTimeMicroseconds;
        uint64_t maxTimeMicroseconds;
        std::atomic<double> avgTimeMicroseconds{0.0};
        double cpuPercentage{0.0};
        
        ProfilingData(OperationType type, const std::string& name) 
            : operationType(type), operationName(name), callCount(0), 
              totalTimeMicroseconds(0), minTimeMicroseconds(UINT64_MAX),
              maxTimeMicroseconds(0) {}
    };
    
    OperationProfiler(size_t maxHistoryEntries = 1000);
    
    // Start profiling an operation
    void startProfiling(OperationType operationType, const std::string& operationName);
    
    // Stop profiling current operation
    void stopProfiling();
    
    // Record a profiling sample
    void recordSample(OperationType operationType, const std::string& operationName, uint64_t timeMicroseconds);
    
    // Get current profiling data for an operation
    const ProfilingData* getProfilingData(OperationType operationType) const;
    
    // Get profiling data for all operations
    std::vector<const ProfilingData*> getAllProfilingData() const;
    
    // Get hot operations (sorted by average time)
    std::vector<const ProfilingData*> getHotOperations(size_t count = 10) const;
    
    // Get operations by call count
    std::vector<const ProfilingData*> getFrequentOperations(size_t count = 10) const;
    
    // Clear profiling data
    void clearProfilingData();
    
    // Generate profiling report
    std::string generateReport() const;
    
    // Check if profiler is active
    bool isActive() const { return active_; }
    
    void setActive(bool active) { active_ = active; }
    
private:
    std::vector<std::unique_ptr<ProfilingData>> profilingData_;
    std::unordered_map<OperationType, ProfilingData*> dataMap_;
    std::deque<std::pair<OperationType, uint64_t>> recentOperations_;
    bool active_;
    size_t maxHistoryEntries_;
    mutable std::mutex mutex_;
};

/**
 * Memory tracker for monitoring allocation/deallocation patterns
 */
class MemoryTracker {
public:
    enum class AllocationType {
        NEW,
        NEW_ARRAY,
        MALLOC,
        MEMORY_POOL,
        ALLOCATE_UNIQUE,
        ALLOCATE_SHARED,
        UNKNOWN
    };
    
    struct AllocationInfo {
        AllocationType allocType;
        void* pointer;
        size_t size;
        std::string allocationSite;
        uint64_t allocationTime;
        int callStackDepth;
        std::vector<std::string> callStack;
        size_t threadId;
        bool isFreed;
        
        AllocationInfo(AllocationType type, void* ptr, size_t size, 
                      const std::string& site, uint64_t time,
                      const std::vector<std::string>& stack)
            : allocType(type), pointer(ptr), size(size), allocationSite(site),
              allocationTime(time), callStackDepth(static_cast<int>(stack.size())),
              callStack(stack), threadId(std::hash<std::thread::id>{}(std::this_thread::get_id())),
              isFreed(false) {}
    };
    
    MemoryTracker(size_t maxTrackedAllocations = 100000);
    
    // Track memory allocation
    void trackAllocation(AllocationType type, void* pointer, size_t size, 
                        const std::string& allocationSite);
    
    // Track memory deallocation
    void trackDeallocation(AllocationType type, void* pointer);
    
    // Get allocations by pointer
    const AllocationInfo* getAllocationInfo(void* pointer) const;
    
    // Get current allocations (not freed)
    std::vector<const AllocationInfo*> getCurrentAllocations() const;
    
    // Get allocations by thread
    std::vector<const AllocationInfo*> getAllocationsByThread(size_t threadId) const;
    
    // Get allocations by site
    std::vector<const AllocationInfo*> getAllocationsBySite(const std::string& site) const;
    
    // Get total memory usage
    size_t getTotalMemoryUsage() const;
    
    // Get allocation count
    size_t getAllocationCount() const;
    
    // Get deallocation count
    size_t getDeallocationCount() const;
    
    // Get memory leaks
    std::vector<const AllocationInfo*> getMemoryLeaks() const;
    
    // Clear tracking data
    void clearTracking();
    
    // Generate memory report
    std::string generateMemoryReport() const;
    
    // Get allocation statistics
    struct MemoryStats {
        size_t totalAllocations;
        size_t totalDeallocations;
        size_t currentAllocations;
        size_t totalMemoryBytes;
        size_t peakMemoryBytes;
        double fragmentationRatio;
    };
    
    MemoryStats getMemoryStats() const;
    
    // Set allocation tracking enabled
    void setAllocationTrackingEnabled(bool enabled) { allocationTrackingEnabled_ = enabled; }
    
    bool isAllocationTrackingEnabled() const { return allocationTrackingEnabled_; }
    
    // Set stack tracking depth
    void setStackDepth(size_t depth) { stackDepth_ = depth; }
    
private:
    std::unordered_map<void*, std::unique_ptr<AllocationInfo>> allocationMap_;
    std::unordered_map<size_t, std::vector<const AllocationInfo*>> allocationsByThread_;
    std::unordered_map<std::string, std::vector<const AllocationInfo*>> allocationsBySite_;
    bool allocationTrackingEnabled_;
    size_t maxTrackedAllocations_;
    size_t stackDepth_;
    mutable std::mutex mutex_;
    std::atomic<size_t> peakMemoryUsage_;
};

/**
 * Performance metrics collector for comprehensive brain performance analysis
 */
class PerformanceMetricsCollector {
public:
    struct BrainMetrics {
        uint64_t simulationStep;
        double simulationTime;  // seconds
        double stepTime;        // seconds
        
        // Neural computation metrics
        size_t neuronCount;
        size_t activeNeurons;
        size_t firingNeurons;
        size_t pendingSpikes;
        size_t totalSpikes;
        double averageFiringRate;
        
        // Performance metrics
        double neuronsPerSecond;
        double spikesPerSecond;
        double synapticEventsPerSecond;
        double realTimeFactor;  // simulated time / real time
        
        // Memory metrics
        size_t memoryUsageBytes;
        size_t neuronPoolUsage;
        size_t synapsePoolUsage;
        
        // CPU metrics
        double cpuUsagePercentage;
        double memoryBandwidthUsage;
        
        // System metrics
        size_t threadCount;
        size_t activeThreads;
        
        BrainMetrics() : simulationStep(0), simulationTime(0.0), stepTime(0.0),
                        neuronCount(0), activeNeurons(0), firingNeurons(0),
                        pendingSpikes(0), totalSpikes(0), averageFiringRate(0.0),
                        neuronsPerSecond(0.0), spikesPerSecond(0.0),
                        synapticEventsPerSecond(0.0), realTimeFactor(1.0),
                        memoryUsageBytes(0), neuronPoolUsage(0), synapsePoolUsage(0),
                        cpuUsagePercentage(0.0), memoryBandwidthUsage(0.0),
                        threadCount(0), activeThreads(0) {}
    };
    
    PerformanceMetricsCollector(size_t metricsHistorySize = 1000);
    
    // Record metrics snapshot
    void recordMetrics(const BrainMetrics& metrics);
    
    // Get recent metrics
    std::vector<BrainMetrics> getRecentMetrics(size_t count = 100) const;
    
    // Get metrics by simulation step range
    std::vector<BrainMetrics> getMetricsInRange(uint64_t startStep, uint64_t endStep) const;
    
    // Get performance summary
    struct PerformanceSummary {
        double averageStepTime;
        double minStepTime;
        double maxStepTime;
        double averageNeuronsPerSecond;
        double averageSpikesPerSecond;
        double averageMemoryUsage;
        double peakMemoryUsage;
        size_t peakNeuronCount;
        size_t peakFiringNeurons;
    };
    
    PerformanceSummary getPerformanceSummary() const;
    
    // Get bottleneck analysis
    struct BottleneckAnalysis {
        std::string bottleneckType;  // "memory", "cpu", "thread", "i/O"
        double severity;  // 0.0 to 1.0
        std::string description;
        std::vector<std::string> recommendations;
    };
    
    BottleneckAnalysis analyzeBottlenecks() const;
    
    // Generate metrics report
    std::string generateMetricsReport() const;
    
    // Clear metrics data
    void clearMetrics();
    
    // Set metrics collection enabled
    void setMetricsCollectionEnabled(bool enabled) { metricsCollectionEnabled_ = enabled; }
    
    bool isMetricsCollectionEnabled() const { return metricsCollectionEnabled_; }
    
    // Get current metrics count
    size_t getMetricsCount() const { return metrics_.size(); }
    
private:
    std::deque<BrainMetrics> metrics_;
    bool metricsCollectionEnabled_;
    size_t maxHistorySize_;
    mutable std::mutex mutex_;
};

/**
 * Brain-specific performance profiler
 */
class BrainPerformanceProfiler {
public:
    BrainPerformanceProfiler(std::shared_ptr<Config> config);
    
    // Initialize profiler with brain reference
    void initialize(Brain* brain);
    
    // Start performance profiling
    void startProfiling();
    
    // Stop performance profiling
    void stopProfiling();
    
    // Record performance snapshot
    void recordPerformanceSnapshot();
    
    // Get operation profiler
    OperationProfiler& getOperationProfiler() { return operationProfiler_; }
    
    // Get memory tracker
    MemoryTracker& getMemoryTracker() { return memoryTracker_; }
    
    // Get performance metrics collector
    PerformanceMetricsCollector& getMetricsCollector() { return metricsCollector_; }
    
    // Get performance statistics
    struct PerformanceStatistics {
        uint64_t totalSteps;
        double totalSimulationTime;
        double averageStepTime;
        size_t peakNeuronCount;
        size_t peakFiringNeurons;
        size_t peakMemoryUsage;
        double totalCPUUsage;
        double totalMemoryBandwidthUsage;
        
        // Operation breakdown
        std::unordered_map<std::string, uint64_t> operationCounts;
        std::unordered_map<std::string, double> operationTimes;
        
        // Memory breakdown
        std::unordered_map<std::string, size_t> allocationCounts;
        std::unordered_map<std::string, size_t> deallocationCounts;
    };
    
    PerformanceStatistics getPerformanceStatistics() const;
    
    // Generate comprehensive performance report
    std::string generatePerformanceReport() const;
    
    // Check if profiling is active
    bool isProfiling() const { return profilingActive_; }
    
    // Enable/disable specific profiling features
    void setOperationProfilingEnabled(bool enabled) { operationProfilingEnabled_ = enabled; }
    void setMemoryTrackingEnabled(bool enabled) { memoryTrackingEnabled_ = enabled; }
    void setMetricsCollectionEnabled(bool enabled) { metricsCollectionEnabled_ = enabled; }
    
    bool isOperationProfilingEnabled() const { return operationProfilingEnabled_; }
    bool isMemoryTrackingEnabled() const { return memoryTrackingEnabled_; }
    bool isMetricsCollectionEnabled() const { return metricsCollectionEnabled_; }
    
private:
    Brain* brain_;
    bool profilingActive_;
    PerformanceTimer stepTimer_;
    
    OperationProfiler operationProfiler_;
    MemoryTracker memoryTracker_;
    PerformanceMetricsCollector metricsCollector_;
    
    bool operationProfilingEnabled_;
    bool memoryTrackingEnabled_;
    bool metricsCollectionEnabled_;
    
    mutable std::mutex mutex_;
};

/**
 * Performance analysis tools for identifying optimization opportunities
 */
class OptimizationAnalyzer {
public:
    struct OptimizationOpportunity {
        std::string optimizationType;  // "memory_allocation", "cache_usage", "thread_contention", etc.
        double potentialImprovement;  // Expected performance gain (0.0 to 1.0)
        double currentCost;  // Current overhead
        std::string description;
        std::string implementationComplexity;  // "low", "medium", "high"
        std::vector<std::string> implementationSteps;
        std::vector<std::string> risks;
    };
    
    OptimizationAnalyzer() = default;
    
    // Analyze operation profiler data for optimization opportunities
    std::vector<OptimizationOpportunity> analyzeOperationProfiling(
        const OperationProfiler& profiler) const;
    
    // Analyze memory tracker data for optimization opportunities
    std::vector<OptimizationOpportunity> analyzeMemoryTracking(
        const MemoryTracker& tracker) const;
    
    // Analyze metrics for optimization opportunities
    std::vector<OptimizationOpportunity> analyzeMetrics(
        const PerformanceMetricsCollector& metrics) const;
    
    // Generate optimization report
    std::string generateOptimizationReport(
        const std::vector<OptimizationOpportunity>& opportunities) const;
    
    // Prioritize optimization opportunities
    std::vector<size_t> prioritizeOpportunities(
        const std::vector<OptimizationOpportunity>& opportunities) const;
    
    // Get optimization recommendations for bottleneck analysis
    std::vector<std::string> getRecommendations(const PerformanceMetricsCollector::BottleneckAnalysis& bottleneck) const;
};

/**
 * Memory pool tracker for monitoring pool allocation patterns
 */
class MemoryPoolTracker {
public:
    MemoryPoolTracker() = default;
    
    // Track memory pool allocation
    void trackMemoryPoolAllocation(const std::string& poolName, size_t objectSize, size_t count);
    
    // Track memory pool deallocation
    void trackMemoryPoolDeallocation(const std::string& poolName, size_t count);
    
    // Get pool statistics
    struct PoolStats {
        std::string poolName;
        size_t totalObjectsAllocated;
        size_t totalObjectsDeallocated;
        size_t currentObjects;
        size_t totalMemoryBytes;
        double allocationEfficiency;  // allocated / (allocated + deallocated)
    };
    
    std::vector<PoolStats> getPoolStats() const;
    
    // Get memory pool recommendations
    std::vector<std::string> getPoolRecommendations() const;
    
    // Generate pool tracking report
    std::string generatePoolReport() const;
    
private:
    std::unordered_map<std::string, PoolStats> poolStats_;
    mutable std::mutex mutex_;
};

/**
 * Global performance monitoring utilities
 */
namespace PerformanceMonitor {
    
    // Get global performance profiler instance
    BrainPerformanceProfiler& getGlobalProfiler();
    
    // Set global configuration
    void configure(const std::shared_ptr<Config>& config);
    
    // Start global performance monitoring
    void startMonitoring();
    
    // Stop global performance monitoring
    void stopMonitoring();
    
    // Check if global monitoring is active
    bool isMonitoring();
    
    // Generate global performance report
    std::string generateGlobalReport();
    
    // Reset global monitoring state
    void reset();
    
} // namespace PerformanceMonitor

} // namespace nlm

#endif // NLM_PERFORMANCE_PROFILING_HPP