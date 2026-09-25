#pragma once

/**
 * Performance Monitoring Infrastructure
 * 
 * Comprehensive performance monitoring and benchmarking system for NLM.
 * Integrates with existing performance infrastructure (MemoryPool, EventQueue,
 * SIMDVectorization, ParallelProcessing) to provide detailed performance
 * metrics, benchmarking tools, and optimization recommendations.
 * 
 * Features:
 * - Runtime performance metrics collection
 * - Time profiling for critical operations
 * - Memory usage tracking and analysis
 * - Event queue performance metrics
 * - SIMD utilization monitoring
 * - Multithreading performance metrics
 * - Automatic performance tuning recommendations
 * - Bottleneck detection and analysis
 * - Real-time performance visualization
 * - Python bindings for performance monitoring
 */

#include <cstddef>
#include <cstdint>
#include <chrono>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <atomic>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <optional>

// Performance monitoring configuration
#define NLM_PERF_ENABLED 1

// Forward declarations for integration
namespace nlm {
    class Brain;
    class NeuronPool;
    class SynapsePool;
    class EventQueue;
    class ParallelProcessing;
    class SIMDVectorization;
}

// Performance metrics structures
struct BenchmarkResult {
    std::string benchmarkName;
    double duration;           // Wall-clock time in seconds
    double cpuTime;           // CPU time in seconds
    uint64_t operations;      // Number of operations
    double opsPerSecond;      // Operations per second
    size_t memoryUsed;        // Memory used in bytes
    double memoryBandwidth;   // Memory bandwidth in GB/s
    std::string simdLevel;   // SIMD instruction set used (SSE, AVX2, AVX512)
    size_t numThreads;        // Number of threads used
    double efficiency;        // Parallel efficiency (0.0 to 1.0)
    double instructionsPerCycle; // Instructions per cycle
    std::vector<double> timeline; // Per-operation timing
};

struct PerformanceSnapshot {
    double timestamp;         // Simulation time
    uint64_t step;           // Current simulation step
    uint64_t totalSpikes;    // Total spikes so far
    size_t activeNeurons;    // Number of active neurons
    size_t memoryUsage;       // Current memory usage (bytes)
    double cpuUsage;         // CPU usage percentage
    double eventQueueSize;   // Event queue size
    double eventQueueFill;   // Event queue fill rate (0.0 to 1.0)
    size_t memoryPoolAllocations;  // Memory pool allocations
    size_t memoryPoolDeallocations; // Memory pool deallocations
    double simdUtilization;  // SIMD utilization percentage
    size_t parallelThreads;  // Number of active threads
    std::map<std::string, double> customMetrics; // Custom metric values
};

struct TimelineEvent {
    double timestamp;         // Event timestamp
    std::string eventType;    // Event type (e.g., "neuron_update", "spike_detection")
    uint64_t threadId;       // Thread that generated the event
    double duration;          // Duration in seconds
    std::map<std::string, std::string> metadata; // Additional metadata
};

struct BottleneckAnalysis {
    std::string bottleneckType;     // Type of bottleneck (memory, cpu, event_queue, etc.)
    double severity;                // Severity score (0.0 to 1.0)
    size_t affectedOperations;      // Number of affected operations
    std::string recommendation;      // Recommended action
    double potentialGain;           // Expected performance gain (%)
    bool autoFixable;               // Whether auto-fix is possible
};

class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Singleton pattern
    static PerformanceMonitor& getInstance();
    
    // Configuration
    void enable(bool enable = true);
    bool isEnabled() const { return enabled_; }
    
    void setMonitoringInterval(double interval = 0.1); // Seconds
    void setSnapshotInterval(double interval = 1.0);   // Seconds
    void setTimelineEnabled(bool enable = true);
    
    // Integration with NLM systems
    void integrateWithBrain(nlm::Brain* brain);
    void integrateWithMemoryPool(nlm::NeuronPool* neuronPool);
    void integrateWithMemoryPool(nlm::SynapsePool* synapsePool);
    void integrateWithEventQueue(nlm::EventQueue* eventQueue);
    void integrateWithParallelProcessing(nlm::ParallelProcessing* parallelProc);
    void integrateWithSIMD(nlm::SIMDVectorization* simd);
    
    // Performance metrics collection
    void startBenchmark(const std::string& name);
    BenchmarkResult stopBenchmark(const std::string& name);
    void startTimer(const std::string& name);
    void stopTimer(const std::string& name);
    void incrementCounter(const std::string& name, uint64_t count = 1);
    void updateMetric(const std::string& name, double value);
    
    // Memory tracking
    void trackMemoryAllocation(size_t bytes);
    void trackMemoryDeallocation(size_t bytes);
    void trackMemoryPoolUsage(size_t allocated, size_t available);
    
    // Event queue metrics
    void trackEventQueueStats(size_t size, double fillRate);
    void trackEventProcessingTime(double duration, uint64_t eventCount);
    
    // SIMD metrics
    void trackSIMDU utilization(double utilization, const std::string& simdLevel);
    void trackSIMDVectorization(size_t vectorizedOps, size_t totalOps);
    
    // Parallel processing metrics
    void trackThreadStats(size_t activeThreads, double efficiency);
    void trackParallelEfficiency(double efficiency, size_t threads, size_t work);
    
    // Performance snapshots
    PerformanceSnapshot takeSnapshot();
    std::vector<PerformanceSnapshot> getSnapshots() const;
    
    // Timeline and event tracking
    void recordTimelineEvent(const TimelineEvent& event);
    std::vector<TimelineEvent> getTimelineEvents(const std::string& eventType = "") const;
    
    // Analysis and reporting
    std::vector<BottleneckAnalysis> analyzeBottlenecks();
    std::vector<BenchmarkResult> getBenchmarkResults() const;
    std::string generateReport() const;
    void saveReport(const std::string& filename) const;
    
    // Performance optimization recommendations
    std::vector<std::string> getOptimizationRecommendations() const;
    bool applyAutoOptimizations();
    
    // Integration hooks
    void startStep(uint64_t step, double time);
    void endStep(uint64_t step, double time);
    void recordSpikeProcessingTime(double duration, size_t neuronCount);
    void recordNeuronUpdateTime(double duration, size_t neuronCount);
    void recordPlasticityUpdateTime(double duration, size_t synapseCount);
    
    // Flush and cleanup
    void flush();
    void reset();

private:
    struct BenchmarkInfo {
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point cpuStart;
        bool active;
    };
    
    struct TimerInfo {
        std::chrono::high_resolution_clock::time_point startTime;
        double totalDuration;
        uint64_t count;
    };
    
    // Core monitoring state
    bool enabled_;
    double monitoringInterval_;
    double snapshotInterval_;
    bool timelineEnabled_;
    
    // Integration pointers
    nlm::Brain* brain_;
    nlm::NeuronPool* neuronPool_;
    nlm::SynapsePool* synapsePool_;
    nlm::EventQueue* eventQueue_;
    nlm::ParallelProcessing* parallelProc_;
    nlm::SIMDVectorization* simd_;
    
    // Timing utilities
    std::chrono::high_resolution_clock::time_point globalStartTime_;
    uint64_t currentStep_;
    double currentTime_;
    
    // Benchmark tracking
    std::map<std::string, BenchmarkInfo> benchmarks_;
    std::vector<BenchmarkResult> benchmarkHistory_;
    
    // Performance metrics
    std::map<std::string, TimerInfo> timers_;
    std::map<std::string, uint64_t> counters_;
    std::map<std::string, double> metrics_;
    
    // Memory tracking
    std::atomic<size_t> totalMemoryAllocated_;
    std::atomic<size_t> totalMemoryDeallocated_;
    std::atomic<size_t> neuronPoolAllocated_;
    std::atomic<size_t> neuronPoolAvailable_;
    std::atomic<size_t> synapsePoolAllocated_;
    std::atomic<size_t> synapsePoolAvailable_;
    
    // Event queue metrics
    std::atomic<size_t> eventQueueSize_;
    std::atomic<double> eventQueueFillRate_;
    std::atomic<double> totalEventProcessingTime_;
    std::atomic<uint64_t> totalEventsProcessed_;
    
    // SIMD metrics
    std::atomic<size_t> simdVectorizedOps_;
    std::atomic<size_t> simdTotalOps_;
    std::atomic<double> simdUtilization_;
    
    // Parallel processing metrics
    std::atomic<size_t> activeThreads_;
    std::atomic<double> parallelEfficiency_;
    
    // Snapshots and timeline
    std::vector<PerformanceSnapshot> snapshots_;
    std::vector<TimelineEvent> timelineEvents_;
    
    // Thread safety
    mutable std::mutex dataMutex_;
    std::condition_variable flushCondition_;
    
    // Utility functions
    double getWallClockTime() const;
    double getCPUTime() const;
    size_t getCurrentMemoryUsage() const;
    std::string getSIMDLevel() const;
    
    // Snapshot generation
    PerformanceSnapshot createSnapshot() const;
    
    // Bottleneck analysis
    std::vector<BottleneckAnalysis> analyzeMemoryBottlenecks() const;
    std::vector<BottleneckAnalysis> analyzeCPUbottlenecks() const;
    std::vector<BottleneckAnalysis> analyzeEventQueueBottlenecks() const;
    std::vector<BottleneckAnalysis> analyzeParallelBottlenecks() const;
    
    // Report generation
    std::string formatBenchmarkResult(const BenchmarkResult& result) const;
    std::string formatBottleneckAnalysis(const BottleneckAnalysis& bottleneck) const;
    std::string formatPerformanceSnapshot(const PerformanceSnapshot& snapshot) const;
};
