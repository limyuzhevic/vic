/* Performance Profiling Framework for NLM Brain Emulation
 * 
 * Comprehensive real-time performance monitoring and optimization platform.
 * Integrates with existing Phase 5 optimizations to provide detailed profiling.
 */

#pragma once

#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <map>
#include <string>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {

/**
 * Performance metrics for tracking system behavior
 */
struct PerformanceMetrics {
    // Timing metrics
    double realTime;              // Wall clock time
    double cpuTime;              // CPU time
    double stepTime;             // Last step duration
    double stepsPerSecond;       // Processing rate
    double spikeRate;            // Spikes per second
    double synapticRate;         // Synaptic events per second
    double memoryBandwidth;      // Memory bandwidth usage
    
    // Resource metrics
    size_t activeNeurons;        // Currently active neurons
    size_t totalNeurons;         // Total neurons in simulation
    size_t firingNeurons;        // Neurons firing in current step
    size_t totalFiringNeurons;   // Total firing events
    
    // Performance ratios
    double realTimeFactor;       // Simulated time / real time
    size_t synapsesPerNeuron;    // Connectivity density
    double cacheHitRate;         // Memory cache efficiency
    
    // Thread metrics
    size_t activeThreads;        // Threads currently working
    double threadUtilization;    // CPU utilization by threads
    size_t queueLength;          // Event queue length
    
    // Memory metrics
    size_t allocatedMemory;      // Total allocated bytes
    size_t peakMemory;           // Peak memory usage
    size_t memoryFragments;      // Memory fragmentation
    double allocationTime;       // Time spent in allocation
    
    // System metrics
    double cpuUtilization;       // Overall CPU utilization
    double memoryUtilization;    // Overall memory utilization
    size_t diskIOPS;             // Disk I/O operations per second
    size_t networkIOPS;          // Network I/O operations per second
    
    // Quality metrics
    double numericalAccuracy;    // Accuracy of numerical computations
    double convergenceRate;      // Learning convergence rate
    double stabilityMetric;      // System stability indicator
    
    uint64_t timestamp;          // Measurement timestamp
    uint64_t stepId;             // Current step identifier
    
    PerformanceMetrics()
        : realTime(0.0), cpuTime(0.0), stepTime(0.0)
        , stepsPerSecond(0.0), spikeRate(0.0), synapticRate(0.0)
        , realTimeFactor(1.0), cacheHitRate(0.0), threadUtilization(0.0)
        , memoryUtilization(0.0), cpuUtilization(0.0)
        , numericalAccuracy(1.0), convergenceRate(0.0), stabilityMetric(1.0)
        , timestamp(0), stepId(0) {}
};

/**
 * Performance event for change tracking
 */
struct PerformanceEvent {
    enum class EventType {
        PERFORMANCE_METRIC,
        CPU_PROFILE_ENTRY,
        MEMORY_ALLOCATION,
        MEMORY_DEALLOCATION,
        THREAD_START,
        THREAD_END,
        SPIKE_EVENT,
        SYNAPTIC_EVENT,
        ERROR_OCCURRED,
        WARNING,
        INFO
    };
    
    EventType type;
    uint64_t timestamp;
    uint64_t stepId;
    std::string source;        // File and line where event occurred
    std::string message;       // Event description
    double value;              // Numeric value if applicable
    std::unordered_map<std::string, std::string> metadata;
    
    PerformanceEvent(EventType t, uint64_t ts, uint64_t sid, 
                     const std::string& src, const std::string& msg,
                     double val = 0.0)
        : type(t), timestamp(ts), stepId(sid), source(src), message(msg), value(val) {}
};

/**
 * Call stack frame for CPU profiling
 */
struct ProfileFrame {
    std::string functionName;
    std::string filePath;
    uint32_t lineNumber;
    uint64_t address;          // Instruction pointer
    double samples;            // Number of samples at this frame
    double percentage;         // Percentage of total samples
    
    ProfileFrame(const std::string& func, const std::string& file, 
                 uint32_t line, uint64_t addr = 0)
        : functionName(func), filePath(file), lineNumber(line), 
          address(addr), samples(0.0), percentage(0.0) {}
};

/**
 * CPU profile snapshot
 */
struct CPUSnapshot {
    uint64_t timestamp;
    uint64_t stepId;
    std::vector<ProfileFrame> callStack;
    double totalCpuTime;
    double instructionCount;
    double cacheMisses;
    std::string currentThreadName;
    
    CPUSnapshot() : timestamp(0), stepId(0), totalCpuTime(0.0), 
                    instructionCount(0.0), cacheMisses(0.0) {}
};

/**
 * Memory allocation record
 */
struct MemoryRecord {
    uint64_t timestamp;
    uint64_t stepId;
    void* address;
    size_t size;
    const char* file;          // Allocation site
    size_t line;
    bool isAllocation;
    std::string allocationType;
    double currentMemoryUsage;
    
    MemoryRecord(uint64_t ts, uint64_t sid, void* addr, size_t sz,
                 const char* file, size_t line, bool alloc,
                 const std::string& type)
        : timestamp(ts), stepId(sid), address(addr), size(sz), file(file), 
          line(line), isAllocation(alloc), allocationType(type),
          currentMemoryUsage(0.0) {}
};

/**
 * Thread activity record
 */
struct ThreadRecord {
    uint64_t threadId;
    std::string threadName;
    uint64_t timestamp;
    uint64_t stepId;
    double activeTime;
    size_t instructionsRetired;
    double cpuTime;
    double waitTime;
    size_t lockContention;
    std::string state;         // Running, Sleeping, Waiting, etc.
    
    ThreadRecord(uint64_t tid, const std::string& name, uint64_t ts,
                 uint64_t sid, double active, double cpu, const std::string& state)
        : threadId(tid), threadName(name), timestamp(ts), stepId(sid),
          activeTime(active), instructionsRetired(0), cpuTime(cpu),
          waitTime(0.0), lockContention(0), state(state) {}
};

/**
 * Statistical analyzer for performance data
 */
class PerformanceStatistics {
public:
    struct Trend {
        std::string metricName;
        std::vector<double> values;
        std::vector<uint64_t> timestamps;
        double trend;           // Linear regression slope
        double r2;              // Coefficient of determination
        double mean;            // Mean value
        double stddev;          // Standard deviation
        double min;             // Minimum value
        double max;             // Maximum value
        
        Trend(const std::string& name) : metricName(name), trend(0.0), 
                                        r2(0.0), mean(0.0), stddev(0.0),
                                        min(std::numeric_limits<double>::max()),
                                        max(std::numeric_limits<double>::lowest()) {}
    };
    
    struct Anomaly {
        std::string metricName;
        uint64_t timestamp;
        double value;
        double expectedValue;
        double deviation;        // Standard deviations from mean
        std::string severity;    // Warning, Error, Critical
        std::string description;
    };
    
    void addMetric(const std::string& name, double value, uint64_t timestamp);
    void addCPUProfile(const CPUSnapshot& snapshot);
    void addMemoryRecord(const MemoryRecord& record);
    void addThreadRecord(const ThreadRecord& record);
    
    const std::vector<Trend>& getTrends() const { return trends_; }
    const std::vector<Anomaly>& getAnomalies() const { return anomalies_; }
    
    std::vector<double> getMetricHistory(const std::string& name, 
                                         size_t lastN = 100) const;
    double calculateTrend(const std::string& name) const;
    void detectAnomalies(double threshold = 3.0);
    
    std::string generateReport() const;
    
private:
    std::unordered_map<std::string, std::vector<std::pair<uint64_t, double>>> metricHistory_;
    std::vector<Trend> trends_;
    std::vector<Anomaly> anomalies_;
    std::mutex mutex_;
    
    double calculateLinearRegression(const std::vector<double>& x, 
                                     const std::vector<double>& y,
                                     double& trend, double& r2) const;
};

/**
 * Real-time performance monitoring system
 */
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start/stop monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const { return isMonitoring_; }
    
    // Data collection
    void recordMetrics(const PerformanceMetrics& metrics);
    void recordCPUSnapshot(const CPUSnapshot& snapshot);
    void recordMemoryRecord(const MemoryRecord& record);
    void recordThreadRecord(const ThreadRecord& record);
    
    // Event recording
    void recordEvent(const PerformanceEvent& event);
    void recordSystemEvent(const std::string& source, 
                          PerformanceEvent::EventType type,
                          const std::string& message,
                          double value = 0.0);
    
    // Configuration
    void setSamplingInterval(double interval) { samplingInterval_ = interval; }
    void setMetricsLevel(size_t level) { metricsLevel_ = level; }
    void enableCPUCProfiling(bool enable) { cpuProfilingEnabled_ = enable; }
    void enableMemoryProfiling(bool enable) { memoryProfilingEnabled_ = enable; }
    void enableThreadProfiling(bool enable) { threadProfilingEnabled_ = enable; }
    
    // Query results
    PerformanceMetrics getLatestMetrics() const;
    std::vector<PerformanceEvent> getRecentEvents(size_t count = 100) const;
    PerformanceStatistics getStatistics() const { return statistics_; }
    
    // Flame graph generation
    std::string generateFlameGraph(size_t maxSamples = 1000) const;
    
    // Performance alerting
    void addAlertThreshold(const std::string& metric, double threshold,
                           PerformanceEvent::EventType severity);
    void removeAlertThreshold(const std::string& metric);
    
private:
    bool isMonitoring_;
    double samplingInterval_;
    size_t metricsLevel_;
    bool cpuProfilingEnabled_;
    bool memoryProfilingEnabled_;
    bool threadProfilingEnabled_;
    
    mutable std::mutex mutex_;
    
    std::vector<PerformanceMetrics> metricsHistory_;
    std::vector<PerformanceEvent> eventHistory_;
    std::vector<CPUSnapshot> cpuHistory_;
    std::vector<MemoryRecord> memoryHistory_;
    std::vector<ThreadRecord> threadHistory_;
    
    PerformanceStatistics statistics_;
    std::unordered_map<std::string, double> alertThresholds_;
    
    // Internal monitoring
    void monitoringThread();
    void collectSystemMetrics();
    void collectCPUTopology();
    void collectMemoryUsage();
    void collectThreadUsage();
    
    // Helper functions
    std::string getCurrentThreadName() const;
    uint64_t getCurrentTimestamp() const;
    
    // Signal handling
    void setupSignalHandlers();
    static void signalHandler(int signal);
};

/**
 * High-level performance analyzer for neural simulation
 */
class NeuralPerformanceAnalyzer {
public:
    NeuralPerformanceAnalyzer();
    
    // Analysis methods
    double calculateEfficiency(const PerformanceMetrics& metrics) const;
    double calculateBottleneckScore(const PerformanceMetrics& metrics) const;
    std::vector<std::string> identifyPerformanceIssues(
        const PerformanceMetrics& metrics) const;
    std::map<std::string, double> getOptimizationRecommendations(
        const PerformanceMetrics& metrics) const;
    
    // Neural-specific analysis
    double analyzeMemoryEfficiency(const PerformanceMetrics& metrics) const;
    double analyzeComputationEfficiency(const PerformanceMetrics& metrics) const;
    double analyzeParallelEfficiency(const PerformanceMetrics& metrics) const;
    std::vector<std::string> getHardwareOptimizationHints(
        const PerformanceMetrics& metrics) const;
    
    // Trend analysis
    struct NeuralPerformanceTrend {
        double neuronScaling;           // How neurons scale with performance
        double spikeEfficiency;         // Efficiency of spike processing
        double plasticityEfficiency;    // Efficiency of plasticity updates
        double memoryEfficiency;        // Memory usage efficiency
        double simulationSpeed;         // Steps per second scaling
        double realTimeFactor;          // Simulation efficiency
    };
    
    NeuralPerformanceTrend analyzeTrends(const PerformanceMetrics& metrics,
                                         size_t historySize = 100) const;
    
    // Validation against benchmarks
    struct BenchmarkResult {
        std::string benchmarkName;
        size_t neurons;
        size_t synapses;
        size_t steps;
        double targetStepsPerSecond;
        double actualStepsPerSecond;
        bool passed;
        double performanceRatio;
        std::string details;
    };
    
    std::vector<BenchmarkResult> runBenchmarks(
        const PerformanceMetrics& metrics,
        const std::vector<std::tuple<std::string, size_t, size_t>>& benchmarks) const;
    
private:
    PerformanceMonitor& monitor_;
    PerformanceStatistics& statistics_;
};