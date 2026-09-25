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
 * Enhanced Phase 5.1+ adds:
 * 6. Performance Monitoring & Profiling
 *    - Real-time metrics collection
 *    - Performance regression testing
 *    - System-wide resource monitoring
 * 
 * 7. Advanced Memory Management
 *    - Memory pools with leak detection
 *    - Object lifetime tracking
 *    - Memory fragmentation analysis
 * 
 * 8. Enhanced Thread Safety
 *    - Lock-free algorithms with reduced contention
 *    - Adaptive spin-waits
 *    - Thread-local caching
 * 
 * 9. SIMD Integration
 *    - Better SIMD integration with performance tuning
 *    - Auto-detection and fallback mechanisms
 *    - Profile-guided optimization support
 * 
 * 10. Configuration Integration
 *    - Performance settings via Config system
 *    - Runtime optimization tuning
 *    - Per-component configuration
 * 
 * 11. Benchmarking
 *    - Performance regression testing
 *    - Baseline measurement
 *    - Automated performance validation
 * 
 * 12. Debug Support
 *    - Profile visualization and analysis
 *    - Performance profiling tools
 *    - Memory leak detection
 * 
 * This infrastructure is designed to scale from 10k to 100M+ neurons.
 */

#ifndef NLM_PERFORMANCE_HPP
#define NLM_PERFORMANCE_HPP

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <atomic>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <array>
#include <optional>
#include <functional>
#include <algorithm>
#include <numeric>
#include <limits>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <iomanip>
#include <sstream>

// Configure performance features based on build settings
#ifdef NLM_PERFORMANCE_ENABLED
    #define NLM_USE_MEMORY_POOL 1
    #define NLM_USE_SOA 1
    #define NLM_USE_EVENT_QUEUE 1
    #define NLM_USE_SPARE_CONNECTIVITY 1
    #define NLM_USE_MULTITHREADING 1
    #define NLM_USE_SIMD 1
    #define NLM_USE_PERFORMANCE_MONITORING 1
    #define NLM_USE_BENCHMARKING 1
    #define NLM_USE_METRICS_COLLECTOR 1
#else
    #define NLM_USE_MEMORY_POOL 0
    #define NLM_USE_SOA 0
    #define NLM_USE_EVENT_QUEUE 0
    #define NLM_USE_SPARE_CONNECTIVITY 0
    #define NLM_USE_MULTITHREADING 0
    #define NLM_USE_SIMD 0
    #define NLM_USE_PERFORMANCE_MONITORING 0
    #define NLM_USE_BENCHMARKING 0
    #define NLM_USE_METRICS_COLLECTOR 0
#endif

// SIMD configuration
#ifdef NLM_SIMD_AVX2
    #define NLM_SIMD_WIDTH 8  // AVX2: 8 float32
#elif defined(NLM_SIMD_AVX512)
    #define NLM_SIMD_WIDTH 16 // AVX512: 16 float32
#elif defined(NLM_SIMD_SSE)
    #define NLM_SIMD_WIDTH 4  // SSE: 4 float32
#else
    #define NLM_SIMD_WIDTH 1  // Scalar
#endif

// Multithreading configuration
#ifdef NLM_THREAD_COUNT
    #define NLM_NUM_THREADS NLM_THREAD_COUNT
#else
    #include <thread>
    static constexpr size_t NLM_NUM_THREADS = std::thread::hardware_concurrency();
#endif

// Memory pool configuration
#define NLM_MEMORY_POOL_PAGE_SIZE 4096
#define NLM_MAX_POOL_SIZE (1024 * 1024)  // Max objects per pool

// Performance monitoring configuration
#define NLM_PERF_METRICS_SAMPLE_INTERVAL 0.1  // seconds
#define NLM_PERF_MAX_HISTORY_POINTS 10000
#define NLM_PERF_PROFILE_BUFFER_SIZE 4096

namespace nlm {

// Forward declarations
class MemoryPoolBase;
class MetricsCollector;
class Benchmark;

/**
 * Performance metrics types for monitoring
 */
struct PerfMetrics {
    // Core performance counters
    uint64_t allocations{0};
    uint64_t deallocations{0};
    uint64_t allocationsFailed{0};
    
    // Timing metrics
    double totalAllocationTime{0.0};
    double totalDeallocationTime{0.0};
    double maxAllocationTime{0.0};
    double maxDeallocationTime{0.0};
    
    // Memory usage
    size_t totalMemoryUsed{0};
    size_t peakMemoryUsage{0};
    size_t fragmentationRatio{0};
    
    // Thread metrics
    size_t activeThreads{0};
    uint64_t lockContentionCount{0};
    uint64_t cacheMisses{0};
    
    // SIMD metrics
    uint64_t simdOperations{0};
    uint64_t scalarFallbacks{0};
    
    // Throughput metrics
    double operationsPerSecond{0.0};
    double memoryBandwidthMBps{0.0};
    
    // Health metrics
    bool healthy{true};
    std::string lastError{};
    uint64_t errorCount{0};
    
    PerfMetrics() = default;
    
    void reset() {
        allocations = deallocations = allocationsFailed = 0;
        totalAllocationTime = totalDeallocationTime = 0.0;
        maxAllocationTime = maxDeallocationTime = 0.0;
        totalMemoryUsed = peakMemoryUsage = 0;
        activeThreads = 0;
        lockContentionCount = cacheMisses = 0;
        simdOperations = scalarFallbacks = 0;
        operationsPerSecond = memoryBandwidthMBps = 0.0;
        healthy = true;
        lastError.clear();
        errorCount = 0;
    }
    
    double allocationFailureRate() const {
        uint64_t total = allocations + allocationsFailed;
        return total > 0 ? (double)allocationsFailed / total * 100.0 : 0.0;
    }
    
    double simdEfficiency() const {
        uint64_t total = simdOperations + scalarFallbacks;
        return total > 0 ? (double)simdOperations / total * 100.0 : 0.0;
    }
};

/**
 * Performance sample containing metrics at a point in time
 */
struct PerfSample {
    std::chrono::high_resolution_clock::time_point timestamp;
    size_t sampleId;
    PerfMetrics metrics;
    std::string threadName;
    size_t threadId;
    
    PerfSample() : sampleId(0), threadId(0) {}
    PerfSample(size_t id, size_t tid) : sampleId(id), threadId(tid) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
    
    double getTimeSince(const PerfSample& other) const {
        auto duration = timestamp - other.timestamp;
        return std::chrono::duration<double>(duration).count();
    }
};

/**
 * Performance event types for profiling
 */
enum class PerfEventType {
    Allocation,
    Deallocation,
    LockAcquire,
    LockRelease,
    ThreadStart,
    ThreadEnd,
    SIMDOperation,
    MemoryAccess,
    CacheMiss,
    Custom
};

/**
 * Performance event for detailed profiling
 */
struct PerfEvent {
    std::chrono::high_resolution_clock::time_point timestamp;
    PerfEventType type;
    std::string name;
    size_t threadId;
    uint64_t threadAffinity;
    
    // Event-specific data
    union {
        struct {
            size_t size;
            void* address;
        } allocation;
        
        struct {
            void* address;
            size_t size;
        } deallocation;
        
        struct {
            std::string lockName;
            uint64_t waitTimeNs;
        } lockOp;
        
        struct {
            size_t cacheLine;
            bool wasCacheHit;
        } memoryAccess;
        
        struct {
            std::string simdInstruction;
            size_t width;
        } simdOp;
        
        struct {
            std::string customData;
        } custom;
    } data;
    
    PerfEvent() : type(PerfEventType::Custom), threadId(0), threadAffinity(0) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
    
    PerfEvent(PerfEventType t, const std::string& n) 
        : type(t), name(n), threadId(0), threadAffinity(0) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
};

/**
 * Performance monitoring and profiling system
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    /**
     * Get singleton instance
     */
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    /**
     * Initialize performance monitoring
     */
    bool initialize();
    
    /**
     * Shutdown performance monitoring
     */
    void shutdown();
    
    /**
     * Start recording performance events
     */
    void startRecording();
    
    /**
     * Stop recording performance events
     */
    void stopRecording();
    
    /**
     * Is performance monitoring active?
     */
    bool isRecording() const { return isRecording_; }
    
    /**
     * Take a performance snapshot
     */
    PerfSample takeSnapshot();
    
    /**
     * Record a performance event
     */
    void recordEvent(const PerfEvent& event);
    
    /**
     * Get performance metrics
     */
    PerfMetrics getMetrics() const { return currentMetrics_; }
    
    /**
     * Get performance history
     */
    std::vector<PerfSample> getHistory(size_t maxSamples = NLM_PERF_MAX_HISTORY_POINTS) const;
    
    /**
     * Get performance events
     */
    std::vector<PerfEvent> getEvents(const std::string& nameFilter = "", 
                                    PerfEventType typeFilter = PerfEventType::Custom,
                                    size_t maxEvents = 1000) const;
    
    /**
     * Clear performance history
     */
    void clearHistory();
    
    /**
     * Export performance data to JSON
     */
    std::string exportToJSON() const;
    
    /**
     * Export performance data to CSV
     */
    std::string exportToCSV() const;
    
    /**
     * Generate performance report
     */
    std::string generateReport() const;
    
    /**
     * Calculate performance statistics
     */
    struct PerformanceStats {
        double totalTime{0.0};
        double cpuUsage{0.0};
        double memoryUsageMB{0.0};
        double allocationRate{0.0};
        double deallocationRate{0.0};
        double throughput{0.0};
        double cacheHitRate{0.0};
        double simdEfficiency{0.0};
        size_t peakActiveThreads{0};
        size_t totalEvents{0};
    };
    
    PerformanceStats calculateStats(const std::vector<PerfSample>& samples) const;
    
    /**
     * Compare performance between two time periods
     */
    struct PerformanceComparison {
        double performanceChange{0.0};
        double throughputChange{0.0};
        double memoryChange{0.0};
        std::string trend; // "improving", "degrading", "stable"
    };
    
    PerformanceComparison comparePerformance(size_t window1Samples, size_t window2Samples) const;
    
    /**
     * Profile specific code regions
     */
    class ScopedProfiler {
    public:
        ScopedProfiler(PerformanceMonitor& monitor, const std::string& regionName);
        ~ScopedProfiler();
        
        void stop();
        double getDuration() const { return duration_; }
        
    private:
        PerformanceMonitor& monitor_;
        std::string regionName_;
        std::chrono::high_resolution_clock::time_point startTime_;
        double duration_;
        bool stopped_;
    };
    
    ScopedProfiler createProfiler(const std::string& regionName) {
        return ScopedProfiler(*this, regionName);
    }
    
    /**
     * Register performance metrics updater
     */
    using MetricsUpdater = std::function<void(PerfMetrics&)>;
    
    void registerMetricsUpdater(const std::string& name, MetricsUpdater updater);
    
    /**
     * Update metrics from registered updaters
     */
    void updateMetrics();
    
    /**
     * Set performance monitoring enabled/disabled
     */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    bool isEnabled() const { return enabled_; }
    
    /**
     * Set sampling interval
     */
    void setSampleInterval(double interval) { sampleInterval_ = interval; }
    
    double getSampleInterval() const { return sampleInterval_; }
    
private:
    bool enabled_;
    bool isRecording_;
    double sampleInterval_;
    
    mutable std::mutex metricsMutex_;
    mutable std::mutex eventsMutex_;
    
    PerfMetrics currentMetrics_;
    std::vector<PerfSample> history_;
    std::vector<PerfEvent> events_;
    
    size_t nextSampleId_;
    size_t nextEventId_;
    
    std::vector<MetricsUpdater> metricsUpdaters_;
    
    // Internal helper functions
    void updateCurrentMetrics();
    PerfSample createSampleFromCurrent() const;
    void addSampleToHistory(const PerfSample& sample);
    double calculateSampleInterval() const;
};

/**
 * Memory tracking for leak detection
 */
class MemoryTracker {
public:
    MemoryTracker();
    ~MemoryTracker();
    
    // Disable copying
    MemoryTracker(const MemoryTracker&) = delete;
    MemoryTracker& operator=(const MemoryTracker&) = delete;
    
    /**
     * Track memory allocation
     */
    void* trackAllocation(size_t size, const std::string& sourceFile, 
                         size_t sourceLine, const std::string& allocationType);
    
    /**
     * Track memory deallocation
     */
    void trackDeallocation(void* address, const std::string& sourceFile,
                          size_t sourceLine, const std::string& allocationType);
    
    /**
     * Check for memory leaks
     */
    struct LeakReport {
        std::vector<void*> leakedPointers;
        size_t totalLeakedBytes;
        std::vector<std::string> leakTypes;
        bool hasLeaks() const { return !leakedPointers.empty(); }
    };
    
    LeakReport checkLeaks() const;
    
    /**
     * Get allocation statistics
     */
    struct AllocationStats {
        size_t totalAllocations{0};
        size_t totalDeallocations{0};
        size_t currentAllocated{0};
        size_t peakAllocated{0};
        size_t totalAllocatedBytes{0};
        std::unordered_map<std::string, size_t> allocationsByType;
        double averageAllocationSize{0.0};
    };
    
    AllocationStats getAllocationStats() const;
    
    /**
     * Enable/disable leak detection
     */
    void setLeakDetectionEnabled(bool enabled) { leakDetectionEnabled_ = enabled; }
    
    bool isLeakDetectionEnabled() const { return leakDetectionEnabled_; }
    
    /**
     * Clear all tracking data
     */
    void clear();
    
private:
    struct AllocationInfo {
        void* address;
        size_t size;
        std::string sourceFile;
        size_t sourceLine;
        std::string allocationType;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    bool leakDetectionEnabled_;
    mutable std::mutex mutex_;
    std::vector<AllocationInfo> allocations_;
    std::unordered_set<void*> allocatedAddresses_;
};

/**
 * Thread-local performance data
 */
class ThreadLocalPerfData {
public:
    ThreadLocalPerfData();
    ~ThreadLocalPerfData();
    
    // Disable copying
    ThreadLocalPerfData(const ThreadLocalPerfData&) = delete;
    ThreadLocalPerfData& operator=(const ThreadLocalPerfData&) = delete;
    
    /**
     * Increment allocation counter
     */
    void incrementAllocations(size_t count = 1) {
        allocations_.fetch_add(count, std::memory_order_relaxed);
    }
    
    /**
     * Increment deallocation counter
     */
    void incrementDeallocations(size_t count = 1) {
        deallocations_.fetch_add(count, std::memory_order_relaxed);
    }
    
    /**
     * Add to allocation time
     */
    void addAllocationTime(double time) {
        totalAllocationTime_.fetch_add(static_cast<uint64_t>(time * 1e9), std::memory_order_relaxed);
    }
    
    /**
     * Add to deallocation time
     */
    void addDeallocationTime(double time) {
        totalDeallocationTime_.fetch_add(static_cast<uint64_t>(time * 1e9), std::memory_order_relaxed);
    }
    
    /**
     * Update memory usage
     */
    void updateMemoryUsage(size_t usage) {
        memoryUsage_.store(usage, std::memory_order_relaxed);
        if (usage > peakMemoryUsage_.load(std::memory_order_relaxed)) {
            peakMemoryUsage_.store(usage, std::memory_order_relaxed);
        }
    }
    
    /**
     * Increment lock contention
     */
    void incrementLockContention() {
        lockContention_.fetch_add(1, std::memory_order_relaxed);
    }
    
    /**
     * Increment cache misses
     */
    void incrementCacheMisses() {
        cacheMisses_.fetch_add(1, std::memory_order_relaxed);
    }
    
    /**
     * Increment SIMD operations
     */
    void incrementSIMDOperations() {
        simdOperations_.fetch_add(1, std::memory_order_relaxed);
    }
    
    /**
     * Increment scalar fallbacks
     */
    void incrementScalarFallbacks() {
        scalarFallbacks_.fetch_add(1, std::memory_order_relaxed);
    }
    
    /**
     * Get thread-local metrics
     */
    PerfMetrics getMetrics() const {
        PerfMetrics metrics;
        metrics.allocations = allocations_.load(std::memory_order_relaxed);
        metrics.deallocations = deallocations_.load(std::memory_order_relaxed);
        metrics.totalAllocationTime = totalAllocationTime_.load(std::memory_order_relaxed) / 1e9;
        metrics.totalDeallocationTime = totalDeallocationTime_.load(std::memory_order_relaxed) / 1e9;
        metrics.totalMemoryUsed = memoryUsage_.load(std::memory_order_relaxed);
        metrics.peakMemoryUsage = peakMemoryUsage_.load(std::memory_order_relaxed);
        metrics.lockContentionCount = lockContention_.load(std::memory_order_relaxed);
        metrics.cacheMisses = cacheMisses_.load(std::memory_order_relaxed);
        metrics.simdOperations = simdOperations_.load(std::memory_order_relaxed);
        metrics.scalarFallbacks = scalarFallbacks_.load(std::memory_order_relaxed);
        return metrics;
    }
    
    /**
     * Reset thread-local counters
     */
    void reset() {
        allocations_.store(0, std::memory_order_relaxed);
        deallocations_.store(0, std::memory_order_relaxed);
        totalAllocationTime_.store(0, std::memory_order_relaxed);
        totalDeallocationTime_.store(0, std::memory_order_relaxed);
        memoryUsage_.store(0, std::memory_order_relaxed);
        peakMemoryUsage_.store(0, std::memory_order_relaxed);
        lockContention_.store(0, std::memory_order_relaxed);
        cacheMisses_.store(0, std::memory_order_relaxed);
        simdOperations_.store(0, std::memory_order_relaxed);
        scalarFallbacks_.store(0, std::memory_order_relaxed);
    }
    
    static ThreadLocalPerfData& getInstance() {
        thread_local ThreadLocalPerfData instance;
        return instance;
    }
    
private:
    std::atomic<size_t> allocations_;
    std::atomic<size_t> deallocations_;
    std::atomic<uint64_t> totalAllocationTime_;
    std::atomic<uint64_t> totalDeallocationTime_;
    std::atomic<size_t> memoryUsage_;
    std::atomic<size_t> peakMemoryUsage_;
    std::atomic<uint64_t> lockContention_;
    std::atomic<uint64_t> cacheMisses_;
    std::atomic<uint64_t> simdOperations_;
    std::atomic<uint64_t> scalarFallbacks_;
};

// Global performance monitor instance
inline PerformanceMonitor& gPerformanceMonitor = PerformanceMonitor::getInstance();

// Global memory tracker instance
inline MemoryTracker& gMemoryTracker = MemoryTracker::getInstance();

// Convenience macros for performance tracking
#define NLM_PERF_SCOPE(monitor, name) auto _perf_scope_##__LINE__ = monitor.createProfiler(name)

#define NLM_PERF_RECORD_EVENT(type, name) monitor.recordEvent(PerfEvent(type, name))

#define NLM_PERF_ALLOC_TRACK(size, source_file, source_line, alloc_type) gMemoryTracker.trackAllocation(size, source_file, source_line, alloc_type)

#define NLM_PERF_DEALLOC_TRACK(address, source_file, source_line, alloc_type) gMemoryTracker.trackDeallocation(address, source_file, source_line, alloc_type)

} // namespace nlm

#endif // NLM_PERFORMANCE_HPP