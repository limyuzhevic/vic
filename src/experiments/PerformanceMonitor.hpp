// PerformanceMonitor.hpp - Comprehensive Performance Monitoring System
// Monitors CPU, memory, thread, and I/O performance for NLM

#pragma once

/**
 * PerformanceMonitor - Comprehensive performance monitoring for NLM
 * 
 * This module provides advanced performance monitoring capabilities for the
 * Neural Learning Machine, including:
 * 
 * 1. CPU Performance Counters
 *    - Real-time CPU usage monitoring
 *    - Instruction and cycle counting
 *    - Cache hit/miss ratios
 *    - Branch prediction statistics
 * 
 * 2. Memory Usage Tracking
 *    - Memory allocation/deallocation tracking
 *    - Memory pool usage monitoring
 *    - Memory bandwidth and latency
 *    - Heap vs. pool memory tracking
 * 
 * 3. Timestep Timing and Bottlenecks
 *    - Fine-grained timestep profiling
 *    - Bottleneck identification
 *    - Performance degradation tracking
 *    - Critical path analysis
 * 
 * 4. Thread Performance Metrics
 *    - Thread creation/destruction tracking
 *    - Lock contention monitoring
 *    - Thread-local storage usage
 *    - Work stealing statistics
 * 
 * 5. Network and I/O Performance
 *    - File I/O monitoring
 *    - Network communication tracking
 *    - Event queue performance
 *    - Data transfer rates
 * 
 * Features:
 * - Real-time monitoring with configurable sampling
 * - Non-intrusive measurement (zero overhead when disabled)
 * - Thread-safe operation
 * - Integration with NLM profiling system
 * - Customizable monitoring per module/component
 */

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <atomic>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <thread>
#include <future>
#include <map>
#include <variant>
#include <optional>
#include <numeric>
#include <cmath>
#include <type_traits>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <cstring>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif

namespace nlm {

/**
 * CPU performance counter types
 */
enum class CPUCounterType {
    // Basic counters
    CYCLES = 0,
    INSTRUCTIONS = 1,
    CACHE_REFERENCES = 2,
    CACHE_MISSES = 3,
    BRANCHES = 4,
    BRANCH_MISSES = 5,
    
    // Advanced counters (x86-specific)
    INSTRUMENTED_INSTRUCTIONS = 6,
    L1D_CACHE_LOADS = 7,
    L1D_CACHE_LOAD_MISSES = 8,
    L1D_CACHE_STORES = 9,
    L1D_CACHE_STORE_MISSES = 10,
    L1I_CACHE_LOADS = 11,
    L1I_CACHE_LOAD_MISSES = 12,
    L2_CACHE_LOADS = 13,
    L2_CACHE_LOAD_MISSES = 14,
    L2_CACHE_STORES = 15,
    L2_CACHE_STORE_MISSES = 16,
    L3_CACHE_LOADS = 17,
    L3_CACHE_LOAD_MISSES = 18,
    L3_CACHE_STORES = 19,
    L3_CACHE_STORE_MISSES = 20,
    
    // Memory counters
    DTLB_LOADS = 21,
    DTLB_LOAD_MISSES = 22,
    DTLB_STORES = 23,
    DTLB_STORE_MISSES = 24,
    ITLB_LOADS = 25,
    ITLB_LOAD_MISSES = 26,
    
    // Performance counters
    EXECUTION_FREQUENCY = 27,
    TEMPERATURE_CELSIUS = 28,
    POWER_WATTS = 29,
    VOLTAGE_VOLTS = 30,
    
    // Custom counters
    CUSTOM_START = 100
};

/**
 * Memory allocation tracking information
 */
struct MemoryAllocationInfo {
    uint64_t timestamp;  // Nanoseconds since epoch
    size_t size;         // Allocation size in bytes
    const char* file;    // Source file
    size_t line;         // Source line
    const char* function; // Function name
    std::string threadId; // Thread that allocated
    bool isPool;         // True if from memory pool
    uint32_t poolId;     // Pool identifier (if pool allocation)
    
    MemoryAllocationInfo(uint64_t ts, size_t sz, const char* f, size_t l, 
                        const char* func, const std::string& thread, bool pool, uint32_t pid)
        : timestamp(ts), size(sz), file(f), line(l), function(func), 
          threadId(thread), isPool(pool), poolId(pid) {}
};

/**
 * Thread performance metrics
 */
struct ThreadPerformanceMetrics {
    std::string threadId;
    uint64_t threadIdNumeric;  // Numeric thread ID for sorting
    double cpuTimePercent;     // Percentage of total CPU time
    double userTimePercent;    // User space CPU time percentage
    double systemTimePercent;  // System space CPU time percentage
    size_t minStackSize;       // Minimum stack size used
    size_t maxStackSize;       // Maximum stack size used
    size_t currentStackSize;   // Current stack size
    uint64_t contextSwitches;  // Number of context switches
    uint64_t voluntaryContextSwitches; // Voluntary context switches
    uint64_t involuntaryContextSwitches; // Involuntary context switches
    double cpuUtilization;     // CPU utilization (0-1)
    double memoryUsage;        // Memory usage in MB
    uint64_t allocations;      // Number of memory allocations
    uint64_t deallocations;    // Number of memory deallocations
    double lockWaitTimeNs;     // Total lock wait time in nanoseconds
    uint64_t lockContentionEvents; // Number of lock contention events
    double activeTimePercent;  // Percentage of time thread was active
    
    ThreadPerformanceMetrics(const std::string& id, uint64_t tidNumeric)
        : threadId(id), threadIdNumeric(tidNumeric), cpuTimePercent(0.0), 
          userTimePercent(0.0), systemTimePercent(0.0), minStackSize(0), 
          maxStackSize(0), currentStackSize(0), contextSwitches(0),
          voluntaryContextSwitches(0), involuntaryContextSwitches(0),
          cpuUtilization(0.0), memoryUsage(0.0), allocations(0), deallocations(0),
          lockWaitTimeNs(0.0), lockContentionEvents(0), activeTimePercent(0.0) {}
};

/**
 * I/O performance metrics
 */
struct IOMetrics {
    uint64_t readBytes;        // Total bytes read
    uint64_t writeBytes;       // Total bytes written
    uint64_t readOperations;   // Number of read operations
    uint64_t writeOperations;  // Number of write operations
    double readThroughputMBps; // Read throughput in MB/s
    double writeThroughputMBps; // Write throughput in MB/s
    double readLatencyNs;     // Average read latency in nanoseconds
    double writeLatencyNs;    // Average write latency in nanoseconds
    double diskUtilization;    // Disk utilization (0-1)
    std::unordered_map<std::string, double> deviceMetrics; // Per-device metrics
    
    IOMetrics() : readBytes(0), writeBytes(0), readOperations(0), writeOperations(0),
                  readThroughputMBps(0.0), writeThroughputMBps(0.0),
                  readLatencyNs(0.0), writeLatencyNs(0.0), diskUtilization(0.0) {}
};

/**
 * Timestep performance analysis
 */
struct TimestepPerformance {
    uint64_t timestep;        // Timestep number
    double wallTimeNs;        // Wall clock time for the timestep
    double cpuTimeNs;         // CPU time for the timestep
    double memoryDeltaBytes;  // Memory delta (positive = allocated)
    size_t spikesGenerated;   // Number of spikes generated
    size_t spikesPropagated;  // Number of spikes propagated
    size_t activeNeurons;     // Number of active neurons
    size_t neuronUpdates;     // Number of neuron updates
    size_t synapseUpdates;    // Number of synapse updates
    double bottleneckRatio;  // Ratio indicating how close to bottleneck (0-1)
    
    // Component timing breakdown
    struct ComponentTiming {
        double neuronUpdateNs;    // Neuron update time
        double synapseUpdateNs;   // Synapse update time
        double spikePropagationNs; // Spike propagation time
        double memoryManagementNs; // Memory management time
        double lockContentionNs;  // Lock contention time
        double otherOverheadNs;   // Other overhead time
        
        ComponentTiming() : neuronUpdateNs(0.0), synapseUpdateNs(0.0),
                           spikePropagationNs(0.0), memoryManagementNs(0.0),
                           lockContentionNs(0.0), otherOverheadNs(0.0) {}
    } componentTiming;
    
    // Bottleneck analysis
    std::string bottleneckComponent; // Component that is the bottleneck
    double bottleneckSeverity;        // Severity (0.0 - 1.0)
    
    TimestepPerformance(uint64_t ts) : timestep(ts), wallTimeNs(0.0), cpuTimeNs(0.0),
                                       memoryDeltaBytes(0), spikesGenerated(0),
                                       spikesPropagated(0), activeNeurons(0),
                                       neuronUpdates(0), synapseUpdates(0),
                                       bottleneckRatio(0.0) {}
};

/**
 * Performance monitoring configuration
 */
struct PerformanceMonitorConfig {
    std::string name;
    bool enabled = true;
    double samplingIntervalMs = 1.0;  // Sampling interval in milliseconds
    size_t historySize = 10000;      // Maximum history size
    bool monitorCPU = true;
    bool monitorMemory = true;
    bool monitorThreads = true;
    bool monitorIO = false;           // I/O monitoring (more expensive)
    bool monitorSimulation = true;   // Monitor simulation-specific events
    bool enableBottleneckDetection = true;
    double bottleneckThreshold = 0.8; // Threshold for bottleneck detection (0-1)
    bool trackMemoryAllocations = true;
    bool trackLockContention = true;
    bool trackContextSwitches = true;
    std::string outputFormat = "json";
    std::string outputPath = ".";
    
    // CPU-specific settings
    bool enableCPUCounters = false;   // Enable detailed CPU counters
    std::vector<CPUCounterType> cpuCounters; // Specific counters to monitor
    
    // Memory-specific settings
    bool trackMemoryPools = true;     // Track memory pool usage
    bool trackHeapAllocations = true; // Track heap allocations
    size_t memorySampleInterval = 100; // Sample interval for memory
    
    // Thread-specific settings
    bool trackThreadCreation = true;   // Track thread lifecycle
    bool trackLockUsage = true;       // Track lock usage
    size_t threadSampleInterval = 1000; // Sample interval for threads
    
    // I/O-specific settings
    std::vector<std::string> monitoredFiles; // Specific files to monitor
    std::vector<std::string> monitoredDevices; // Specific devices to monitor
    
    PerformanceMonitorConfig() = default;
};

/**
 * Performance monitor for NLM
 */
class PerformanceMonitor {
private:
    std::string name_;
    PerformanceMonitorConfig config_;
    std::atomic<bool> running_{false};
    std::atomic<bool> paused_{false};
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point lastSampleTime_;
    
    // Monitoring data
    std::vector<TimestepPerformance> timestepHistory_;
    std::vector<MemoryAllocationInfo> memoryAllocations_;
    std::vector<ThreadPerformanceMetrics> threadMetricsHistory_;
    std::vector<IOMetrics> ioMetricsHistory_;
    
    // Current state
    std::atomic<uint64_t> currentTimestep_{0};
    std::atomic<size_t> activeNeurons_{0};
    std::atomic<size_t> totalSpikesGenerated_{0};
    std::atomic<size_t> totalSpikesPropagated_{0};
    std::atomic<double> cumulativeWallTimeNs_{0};
    std::atomic<double> cumulativeCPUTimeNs_{0};
    
    // CPU monitoring
    struct CPUMonitorState {
        std::atomic<uint64_t> lastCycles{0};
        std::atomic<uint64_t> lastInstructions{0};
        std::atomic<uint64_t> lastCacheRefs{0};
        std::atomic<uint64_t> lastCacheMisses{0};
        std::chrono::high_resolution_clock::time_point lastCpuTimeStart_;
        double lastCpuTimeDurationNs{0.0};
        
        CPUMonitorState() {
            lastCpuTimeStart_ = std::chrono::high_resolution_clock::now();
        }
    } cpuState_;
    
    // Memory monitoring
    struct MemoryMonitorState {
        std::atomic<size_t> peakMemoryBytes{0};
        std::atomic<size_t> currentMemoryBytes{0};
        std::atomic<uint64_t> totalAllocations{0};
        std::atomic<uint64_t> totalDeallocations{0};
        std::chrono::high_resolution_clock::time_point lastMemoryCheck_;
        
        MemoryMonitorState() {
            lastMemoryCheck_ = std::chrono::high_resolution_clock::now();
        }
    } memoryState_;
    
    // Thread monitoring
    struct ThreadMonitorState {
        std::atomic<size_t> activeThreads{0};
        std::atomic<size_t> totalThreads{0};
        std::chrono::high_resolution_clock::time_point lastThreadUpdate_;
        std::unordered_map<std::string, ThreadPerformanceMetrics> currentThreadMetrics_;
        
        ThreadMonitorState() {
            lastThreadUpdate_ = std::chrono::high_resolution_clock::now();
        }
    } threadState_;
    
    // I/O monitoring
    struct IOMonitorState {
        std::chrono::high_resolution_clock::time_point lastIOUpdate_;
        IOMetrics currentIOMetrics_;
        
        IOMonitorState() {
            lastIOUpdate_ = std::chrono::high_resolution_clock::now();
        }
    } ioState_;
    
    mutable std::mutex dataMutex_;
    std::vector<std::function<void(const PerformanceMetrics&)>> metricCallbacks_;
    
public:
    PerformanceMonitor(const std::string& name, const PerformanceMonitorConfig& config);
    ~PerformanceMonitor();
    
    void start();
    void stop();
    void pause();
    void resume();
    
    bool isRunning() const { return running_.load(); }
    bool isPaused() const { return paused_.load(); }
    
    // Monitoring control
    void update(double timestepTimeNs, size_t spikesGenerated, size_t spikesPropagated,
                size_t neuronUpdates, size_t synapseUpdates, double memoryDeltaBytes);
    
    void recordMemoryAllocation(size_t size, const char* file, size_t line, 
                               const char* function, bool isPool, uint32_t poolId);
    
    void recordMemoryDeallocation(size_t size, const char* file, size_t line,
                                 const char* function, bool isPool, uint32_t poolId);
    
    void updateThreadMetrics(const ThreadPerformanceMetrics& metrics);
    void updateIOMetrics(const IOMetrics& metrics);
    
    // Configuration
    void setConfig(const PerformanceMonitorConfig& config) { config_ = config; }
    const PerformanceMonitorConfig& getConfig() const { return config_; }
    
    // Data access
    std::vector<TimestepPerformance> getTimestepHistory(size_t maxCount = 0) const;
    std::vector<MemoryAllocationInfo> getMemoryAllocations(size_t maxCount = 0) const;
    std::vector<ThreadPerformanceMetrics> getThreadMetricsHistory(size_t maxCount = 0) const;
    std::vector<IOMetrics> getIOMetricsHistory(size_t maxCount = 0) const;
    
    // Analysis
    PerformanceMetrics calculateCurrentMetrics() const;
    std::string analyzeBottlenecks() const;
    double getAverageTimestepTimeMs() const;
    double getMinTimestepTimeMs() const;
    double getMaxTimestepTimeMs() const;
    double getTimestepTimeStdDev() const;
    size_t getTimestepCount() const { return timestepHistory_.size(); }
    
    // Callback system
    void addMetricCallback(std::function<void(const PerformanceMetrics&)> callback);
    
    // Export
    std::string exportToJSON() const;
    std::string exportToCSV() const;
    std::string exportToYAML() const;
    std::string exportToXML() const;
    
    // Integration with profiler
    void integrateWithProfiler(class NLPerformanceProfiler& profiler);
    
private:
    void sampleCPUMetrics();
    void sampleMemoryMetrics();
    void sampleThreadMetrics();
    void sampleIOMetrics();
    void detectBottlenecks();
    double calculateBottleneckRatio(const TimestepPerformance& ts) const;
    
    static std::string getCurrentThreadId();
    static size_t getCurrentThreadStackSize();
    static uint64_t getThreadNumericId();
};

/**
 * Performance monitor factory
 */
class PerformanceMonitorFactory {
public:
    static std::shared_ptr<PerformanceMonitor> createMonitor(const std::string& name, 
                                                             const PerformanceMonitorConfig& config);
    
    static std::vector<PerformanceMonitorConfig> getAvailableMonitorConfigs();
};

/**
 * Performance statistics calculator
 */
class PerformanceStatistics {
public:
    static double calculateMean(const std::vector<double>& values);
    static double calculateStdDev(const std::vector<double>& values, double mean);
    static double calculateMedian(const std::vector<double>& values);
    static double calculatePercentile(const std::vector<double>& values, double percentile);
    static size_t calculateMode(const std::vector<size_t>& values);
    static std::string analyzeDistribution(const std::vector<double>& values);
    static std::string identifyTrends(const std::vector<double>& values);
    static std::pair<double, double> linearRegression(const std::vector<double>& x, const std::vector<double>& y);
    static double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    static std::string formatTimeNs(double timeNs);
    static std::string formatMemoryBytes(size_t bytes);
    static std::string formatThroughput(double bytes, double timeNs);
};

} // namespace nlm