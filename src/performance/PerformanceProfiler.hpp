#pragma once

/**
 * PerformanceProfiler - Comprehensive performance monitoring and profiling for NLM
 * 
 * This class provides detailed profiling capabilities for:
 * - CPU performance with function-level granularity
 * - Memory usage tracking and leak detection
 * - Network I/O profiling for experiment data
 * - Simulation performance monitoring
 * - GPU acceleration profiling
 * - I/O performance for large datasets
 * - Real-time performance monitoring and alerts
 * 
 * Designed to work with the existing NLM Phase 5 performance infrastructure
 * and integrate with standard profiling tools (Gprof, perf, Valgrind, Intel VTune)
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <atomic>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <thread>
#include <numeric>
#include <algorithm>

namespace nlm {

/**
 * Performance measurement units
 */
enum class TimeUnit {
    Nanoseconds,
    Microseconds,
    Milliseconds,
    Seconds
};

/**
 * Performance data point
 */
struct PerformanceData {
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    double cpuUsage;                    // CPU usage percentage (0-100)
    double memoryUsage;                 // Memory usage in MB
    double memoryPeak;                  // Peak memory usage in MB
    size_t activeThreads;               // Number of active threads
    double cpuTemperature;              // CPU temperature in Celsius (if available)
    double gpuUsage;                    // GPU usage percentage (if available)
    uint64_t networkBytesRead;          // Network bytes read
    uint64_t networkBytesWritten;       // Network bytes written
    uint64_t diskReadSpeed;             // Disk read speed in MB/s
    uint64_t diskWriteSpeed;            // Disk write speed in MB/s
    
    PerformanceData()
        : cpuUsage(0.0), memoryUsage(0.0), memoryPeak(0.0), activeThreads(0),
          cpuTemperature(0.0), gpuUsage(0.0), networkBytesRead(0), networkBytesWritten(0),
          diskReadSpeed(0), diskWriteSpeed(0) {}
};

/**
 * Function-level CPU profiling data
 */
struct FunctionProfile {
    std::string functionName;
    std::string functionSignature;
    uint64_t callCount;
    double totalTime;                   // Total time in seconds
    double averageTime;                 // Average time per call in seconds
    double minTime;                     // Minimum time per call in seconds
    double maxTime;                     // Maximum time per call in seconds
    double cpuPercentage;               // Percentage of total CPU time
    size_t memoryBytesAllocated;        // Total bytes allocated
    size_t memoryPeakBytes;             // Peak memory usage
    
    FunctionProfile() : callCount(0), totalTime(0.0), averageTime(0.0),
                       minTime(0.0), maxTime(0.0), cpuPercentage(0.0),
                       memoryBytesAllocated(0), memoryPeakBytes(0) {}
};

/**
 * Memory allocation tracking
 */
struct MemoryAllocation {
    void* address;
    size_t size;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    std::string stackTrace;
    std::string allocationContext;
    bool isFreed;
    
    MemoryAllocation() : address(nullptr), size(0), isFreed(false) {}
};

/**
 * Network I/O tracking
 */
struct NetworkIO {
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    std::string endpoint;               // Remote endpoint
    uint64_t bytesTransferred;          // Bytes transferred
    double transferSpeed;               // Transfer speed in MB/s
    double latency;                     // Latency in seconds
    std::string protocol;               // HTTP, TCP, UDP, etc.
    std::string operation;              // GET, POST, PUT, etc.
};

/**
 * Simulation performance metrics
 */
struct SimulationMetrics {
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    uint64_t stepsCompleted;            // Number of simulation steps
    uint64_t neuronsActive;             // Number of active neurons
    uint64_t totalSpikes;               // Total spikes generated
    uint64_t synapseActivations;        // Number of synapse activations
    double averageFiringRate;           // Average firing rate (spikes/step)
    double computationTimePerStep;     // Average computation time per step
    double memoryUsagePerStep;         // Average memory usage per step
    double cpuUsagePerStep;            // Average CPU usage per step
    std::vector<double> spikeCounts;   // Spike count per step
    std::vector<double> stepTimes;    // Computation time per step
    
    SimulationMetrics() : stepsCompleted(0), neuronsActive(0), totalSpikes(0),
                         synapseActivations(0), averageFiringRate(0.0),
                         computationTimePerStep(0.0), memoryUsagePerStep(0.0),
                         cpuUsagePerStep(0.0) {}
};

/**
 * Performance alert levels
 */
enum class AlertLevel {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

/**
 * Performance alert
 */
struct PerformanceAlert {
    AlertLevel level;
    std::string message;
    std::chrono::time_point<std::chrono::high_resolution_clock> timestamp;
    std::string source;                // Component that generated the alert
    double threshold;                  // Threshold value that was exceeded
    double currentValue;               // Current measured value
    std::function<void()> action;      // Action to take
    
    PerformanceAlert(AlertLevel lvl, const std::string& msg, const std::string& src)
        : level(lvl), message(msg), source(src), threshold(0.0), currentValue(0.0) {}
};

/**
 * Real-time performance monitor
 */
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Disable copying
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
    /**
     * Initialize the profiler
     */
    bool initialize();
    
    /**
     * Shutdown the profiler
     */
    void shutdown();
    
    /**
     * Start profiling a function
     */
    void startFunctionProfile(const std::string& functionName, 
                            const std::string& signature = "");
    
    /**
     * Stop profiling a function
     */
    void stopFunctionProfile(const std::string& functionName);
    
    /**
     * Record memory allocation
     */
    void recordMemoryAllocation(void* address, size_t size, 
                               const std::string& context = "");
    
    /**
     * Record memory deallocation
     */
    void recordMemoryDeallocation(void* address);
    
    /**
     * Start recording network I/O
     */
    void startNetworkIO(const std::string& endpoint, 
                       const std::string& protocol = "TCP",
                       const std::string& operation = "READ");
    
    /**
     * Stop recording network I/O
     */
    void stopNetworkIO();
    
    /**
     * Start simulation metrics collection
     */
    void startSimulationMetrics();
    
    /**
     * Update simulation metrics
     */
    void updateSimulationMetrics(uint64_t stepsCompleted, uint64_t neuronsActive,
                                uint64_t totalSpikes, uint64_t synapseActivations);
    
    /**
     * Stop simulation metrics collection
     */
    SimulationMetrics stopSimulationMetrics();
    
    /**
     * Add performance alert
     */
    void addAlert(AlertLevel level, const std::string& message, 
                  const std::string& source, double threshold = 0.0,
                  double currentValue = 0.0);
    
    /**
     * Set performance alert callback
     */
    void setAlertCallback(std::function<void(const PerformanceAlert&)> callback);
    
    /**
     * Get current performance data
     */
    PerformanceData getCurrentPerformance() const;
    
    /**
     * Get function profiles
     */
    std::unordered_map<std::string, FunctionProfile> getFunctionProfiles() const;
    
    /**
     * Get memory allocations (leak detection)
     */
    std::vector<MemoryAllocation> getMemoryAllocations() const;
    
    /**
     * Get network I/O statistics
     */
    std::vector<NetworkIO> getNetworkIOStats() const;
    
    /**
     * Get current simulation metrics
     */
    SimulationMetrics getCurrentSimulationMetrics() const;
    
    /**
     * Get performance alerts
     */
    std::vector<PerformanceAlert> getAlerts() const;
    
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
     * Reset all profiling data
     */
    void reset();
    
    /**
     * Check for memory leaks
     */
    std::vector<MemoryAllocation> detectMemoryLeaks() const;
    
    /**
     * Get system resource usage
     */
    static PerformanceData getSystemResourceUsage();
    
    /**
     * Convert time units for display
     */
    static std::string formatTime(double seconds, TimeUnit unit = TimeUnit::Seconds);
    
    /**
     * Format bytes for display
     */
    static std::string formatBytes(size_t bytes);
    
    /**
     * Get profiler instance (singleton)
     */
    static PerformanceProfiler& getInstance();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // NLM_PERFORMANCE_PROFILER_HPP