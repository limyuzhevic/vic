#pragma once

/**
 * Performance Monitoring System for NLM
 * 
 * Real-time performance monitoring and profiling for the Neural Learning Machine
 * Provides timing, memory usage, and activity metrics during brain simulation
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <atomic>
#include <algorithm>

namespace nlm {

/**
 * Performance metrics collected during simulation
 */
struct PerformanceMetrics {
    // Timing metrics
    double totalWallClockTime;      // Total execution time in seconds
    double simulationTime;           // Actual simulation time
    double overheadTime;             // Processing overhead time
    double timestepAverage;          // Average timestep processing time
    
    // Performance metrics
    double stepsPerSecond;          // Simulation steps per second
    double neuronStepsPerSecond;    // Neuron processing speed
    double memoryAccessRate;        // Memory access per second
    size_t instructionsPerStep;     // Approximate instructions per step
    
    // Activity metrics
    size_t totalSpikesRecorded;      // Total spike events
    size_t memoryOperations;         // Memory operations performed
    size_t plasticityOperations;     // Plasticity updates applied
    
    // Resource usage
    size_t peakMemoryUsage;          // Peak memory usage in bytes
    size_t currentMemoryUsage;       // Current memory usage
    size_t allocationCount;          // Number of memory allocations
    size_t deallocationCount;        // Number of memory deallocations
    
    // System metrics
    double cpuUsage;                 // CPU usage percentage
    double memoryUsagePercent;       // Memory usage percentage
    int activeThreads;               // Number of active threads
    
    // Neural dynamics
    double averageFiringRate;       // Average neural firing rate
    double averageExcitatoryInhibition; // Average E/I ratio
    
    PerformanceMetrics()
        : totalWallClockTime(0.0)
        , simulationTime(0.0)
        , overheadTime(0.0)
        , timestepAverage(0.0)
        , stepsPerSecond(0.0)
        , neuronStepsPerSecond(0.0)
        , memoryAccessRate(0.0)
        , instructionsPerStep(0)
        , totalSpikesRecorded(0)
        , memoryOperations(0)
        , plasticityOperations(0)
        , peakMemoryUsage(0)
        , currentMemoryUsage(0)
        , allocationCount(0)
        , deallocationCount(0)
        , cpuUsage(0.0)
        , memoryUsagePercent(0.0)
        , activeThreads(1)
        , averageFiringRate(0.0)
        , averageExcitatoryInhibition(1.0) {}
};

/**
 * Performance sample collected at specific intervals
 */
struct PerformanceSample {
    Timestamp timestamp;             // Simulation time when sample was taken
    SimulationStep step;            // Current simulation step
    double cpuUsage;                 // CPU usage at this sample
    size_t memoryUsage;              // Memory usage at this sample
    size_t allocatedObjects;         // Objects allocated at this sample
    size_t spikeCount;               // Spikes recorded since last sample
    size_t plasticityUpdates;        // Plasticity operations since last sample
    double processingTime;           // Time taken for this sample collection
};

/**
 * Real-time performance monitor for NLM brain simulation
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Disable copying, enable moving
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    PerformanceMonitor(PerformanceMonitor&&) noexcept = default;
    PerformanceMonitor& operator=(PerformanceMonitor&&) noexcept = default;
    
    /**
     * Initialize performance monitoring
     */
    void initialize();
    
    /**
     * Start timing a specific operation
     */
    void startTiming(const std::string& operation);
    
    /**
     * Stop timing and record the duration
     */
    void stopTiming(const std::string& operation);
    
    /**
     * Record a performance sample at current simulation time
     */
    void recordSample(SimulationStep step, Timestamp timestamp);
    
    /**
     * Update current metrics with latest values
     */
    void updateMetrics();
    
    /**
     * Get current performance metrics
     */
    PerformanceMetrics getMetrics() const;
    
    /**
     * Get recent performance samples
     */
    std::vector<PerformanceSample> getRecentSamples(size_t count = 100) const;
    
    /**
     * Get performance statistics for specific time window
     */
    PerformanceMetrics getStatistics(double timeWindowSeconds) const;
    
    /**
     * Get performance summary as string
     */
    std::string getSummary() const;
    
    /**
     * Get detailed performance report
     */
    std::string getDetailedReport() const;
    
    /**
     * Check if monitoring is active
     */
    bool isMonitoring() const { return monitoringEnabled_; }
    
    /**
     * Enable or disable monitoring
     */
    void setMonitoringEnabled(bool enabled) { monitoringEnabled_ = enabled; }
    
    /**
     * Set sampling interval (in simulation steps)
     */
    void setSampleInterval(size_t interval) { sampleInterval_ = interval; }
    
    /**
     * Set memory tracking enabled
     */
    void setMemoryTrackingEnabled(bool enabled) { trackMemory_ = enabled; }
    
    /**
     * Set CPU monitoring enabled
     */
    void setCpuMonitoringEnabled(bool enabled) { trackCpu_ = enabled; }
    
    /**
     * Reset all collected metrics
     */
    void reset();
    
    /**
     * Export metrics to JSON format
     */
    bool exportToJSON(const std::string& filename) const;
    
    /**
     * Export metrics to CSV format
     */
    bool exportToCSV(const std::string& filename) const;
    
    /**
     * Start neural activity tracking
     */
    void startNeuralTracking(size_t neuronCount);
    
    /**
     * Record neural activity metrics
     */
    void recordNeuralActivity(double firingRate, double eiRatio);
    
    /**
     * Start plasticity tracking
     */
    void startPlasticityTracking();
    
    /**
     * Record plasticity operation
     */
    void recordPlasticityOperation();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool monitoringEnabled_;
    size_t sampleInterval_;
    size_t stepCounter_;
    bool trackMemory_;
    bool trackCpu_;
    
    mutable std::mutex metricsMutex_;
};

/**
 * Timing utilities for performance measurement
 */
class Timer {
public:
    Timer();
    ~Timer();
    
    void start();
    void stop();
    
    double elapsedSeconds() const;
    double elapsedMilliseconds() const;
    long long elapsedNanoseconds() const;
    
    bool isRunning() const { return running_; }
    
    void reset();
    
    std::string toString() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool running_;
};

/**
 * Memory profiler for tracking memory usage patterns
 */
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Disable copying, enable moving
    MemoryProfiler(const MemoryProfiler&) = delete;
    MemoryProfiler& operator=(const MemoryProfiler&) = delete;
    MemoryProfiler(MemoryProfiler&&) noexcept = default;
    MemoryProfiler& operator=(MemoryProfiler&&) noexcept = default;
    
    /**
     * Start tracking memory allocations
     */
    void startTracking();
    
    /**
     * Stop tracking and collect statistics
     */
    void stopTracking();
    
    /**
     * Record a memory allocation
     */
    void recordAllocation(size_t bytes, const std::string& sourceFile, int lineNumber);
    
    /**
     * Record a memory deallocation
     */
    void recordDeallocation(size_t bytes, const std::string& sourceFile, int lineNumber);
    
    /**
     * Get current memory usage
     */
    size_t getCurrentMemoryUsage() const;
    
    /**
     * Get peak memory usage
     */
    size_t getPeakMemoryUsage() const;
    
    /**
     * Get allocation count
     */
    size_t getAllocationCount() const;
    
    /**
     * Get deallocation count
     */
     */
    size_t getDeallocationCount() const;
    
    /**
     * Get average allocation size
     */
    double getAverageAllocationSize() const;
    
    /**
     * Get memory usage by source file
     */
    std::unordered_map<std::string, size_t> getMemoryUsageByFile() const;
    
    /**
     * Reset memory statistics
     */
    void reset();
    
    /**
     * Generate memory profile report
     */
    std::string generateReport() const;
    
    /**
     * Check if tracking is active
     */
    bool isTracking() const { return tracking_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool tracking_;
};

/**
 * Performance analyzer for neural computation patterns
 */
class NeuralPerformanceAnalyzer {
public:
    NeuralPerformanceAnalyzer();
    ~NeuralPerformanceAnalyzer();
    
    // Disable copying, enable moving
    NeuralPerformanceAnalyzer(const NeuralPerformanceAnalyzer&) = delete;
    NeuralPerformanceAnalyzer& operator=(const NeuralPerformanceAnalyzer&) = delete;
    NeuralPerformanceAnalyzer(NeuralPerformanceAnalyzer&&) noexcept = default;
    NeuralPerformanceAnalyzer& operator=(NeuralPerformanceAnalyzer&&) noexcept = default;
    
    /**
     * Record neuron processing statistics
     */
    void recordNeuronProcessing(size_t step, size_t neuronCount, double processingTime);
    
    /**
     * Record spike processing statistics
     */
    void recordSpikeProcessing(size_t step, size_t spikeCount, double processingTime);
    
    /**
     * Record synaptic operation statistics
     */
    void recordSynapticOperation(size_t step, size_t synapseCount, double processingTime);
    
    /**
     * Get average processing time per neuron
     */
    double getAverageNeuronProcessingTime() const;
    
    /**
     * Get average processing time per spike
     */
    double getAverageSpikeProcessingTime() const;
    
    /**
     * Get average processing time per synapse
     */
    double getAverageSynapticProcessingTime() const;
    
    /**
     * Get performance statistics for specific time window
     */
    std::string getPerformanceStatistics(double timeWindowSeconds) const;
    
    /**
     * Reset performance statistics
     */
    void reset();
    
    /**
     * Check if analyzer is collecting data
     */
    bool isActive() const { return active_; }
    
    /**
     * Enable or disable performance analysis
     */
    void setActive(bool active) { active_ = active; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool active_;
};

/**
 * Performance benchmark suite for NLM
 */
class PerformanceBenchmark {
public:
    PerformanceBenchmark();
    ~PerformanceBenchmark();
    
    /**
     * Run memory benchmark
     */
    bool runMemoryBenchmark();
    
    /**
     * Run neural processing benchmark
     */
    bool runNeuralProcessingBenchmark();
    
    /**
     * Run spike processing benchmark
     */
    bool runSpikeProcessingBenchmark();
    
    /**
     * Run plasticity benchmark
     */
    bool runPlasticityBenchmark();
    
    /**
     * Get benchmark results
     */
    std::string getResults() const;
    
    /**
     * Export benchmark results
     */
    bool exportResults(const std::string& filename) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm