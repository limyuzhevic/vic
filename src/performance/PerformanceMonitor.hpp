#pragma once

/**
 * Comprehensive Performance Monitoring and Profiling System for NLM
 * 
 * This module provides a complete suite of performance monitoring and profiling utilities
 * for neural network simulations, enabling real-time monitoring, detailed profiling,
 * performance analysis, resource tracking, visualization, and optimization.
 * 
 * Categories:
 * 1. Performance Monitoring Tools
 * 2. Profiling Tools
 * 3. Performance Analysis Tools
 * 4. Resource Monitoring Tools
 * 5. Performance Visualization Tools
 * 6. Advanced Performance Tools
 * 7. Performance Optimization Tools
 * 8. Integration with Performance Systems
 * 9. Real-Time Performance Monitoring
 * 10. Performance Benchmarking Tools
 * 
 * All utilities are designed to be lightweight, thread-safe, and non-intrusive
 * to the simulation runtime. Performance counters use efficient atomic operations
 * and memory usage is minimized.
 */

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <deque>
#include <optional>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>

namespace nlm {

/**
 * Performance monitoring and profiling system for NLM neural network simulations.
 * 
 * This class provides a comprehensive framework for monitoring, profiling, analyzing,
 * and optimizing neural network simulation performance. It supports multiple
 * monitoring modalities, real-time data collection, and automated optimization.
 */
class PerformanceMonitor {
public:
    /**
     * Performance metric type enumeration.
     */
    enum class MetricType {
        CPU_USAGE,           ///< CPU utilization percentage
        MEMORY_USAGE,        ///< Memory consumption in bytes
        NETWORK_USAGE,       ///< Network bandwidth usage
        DISK_USAGE,          ///< Disk I/O operations
        THREAD_COUNT,        ///< Number of active threads
        CONTEXT_SWITCHES,    ///< CPU context switches
        PAGE_FAULTS,         ///< Memory page faults
        SYSTEM_LOAD,         ///< System load average
        PROCESS_CPU,         ///< Process-specific CPU usage
        PROCESS_MEMORY,      ///< Process-specific memory usage
        NEURAL_COMPUTATION,  ///< Neural computation performance
        PLASTICITY_RULES,    ///< Plasticity rule execution
        MEMORY_OPERATIONS,   ///< Memory system operations
        SIMULATION_STEPS,    ///< Simulation step performance
        CONFIG_LOADING,      ///< Configuration loading
        LOGGING_PERFORMANCE, ///< Logging system performance
        NETWORK_COMM,        ///< Network communication
        FILE_OPERATIONS,     ///< File I/O operations
        HARDWARE_PERF,       ///< Hardware-specific metrics
        SOFTWARE_PERF,       ///< Software-specific metrics
        NETWORK_PERF,        ///< Network performance metrics
        DB_PERF,             ///< Database performance (if applicable)
        DISTRIBUTED_PERF,    ///< Distributed system performance
        CLOUD_PERF,          ///< Cloud performance metrics
        EDGE_PERF,           ///< Edge computing performance
        TIMESTAMP            ///< Timestamp for time-series data
    };

    /**
     * Performance data point structure.
     */
    struct PerformanceDataPoint {
        MetricType metricType;
        double value;
        std::chrono::system_clock::time_point timestamp;
        std::string componentName;
        std::string metadata;  ///< Additional metadata as string
        std::map<std::string, std::string> tags;  ///< Key-value tags for filtering
        
        PerformanceDataPoint(MetricType type, double val, 
                           const std::string& component = "",
                           const std::string& meta = "")
            : metricType(type), value(val), componentName(component), metadata(meta) {
            timestamp = std::chrono::system_clock::now();
        }
    };

    /**
     * Performance trend analysis result.
     */
    struct TrendAnalysisResult {
        std::string metricName;
        double trendValue;              ///< Slope of the trend line
        double trendCoefficient;        ///< R² coefficient of determination
        std::string trendDirection;    ///< "increasing", "decreasing", "stable"
        double confidence;              ///< Statistical confidence in trend
        size_t dataPointsAnalyzed;      ///< Number of data points used
        std::chrono::system_clock::duration analysisPeriod;
        std::string anomalyCount;       ///< Number of anomalies detected
    };

    /**
     * Performance anomaly detection result.
     */
    struct AnomalyDetectionResult {
        std::string anomalyId;
        MetricType metricType;
        double value;
        double threshold;
        double severity;  ///< 0.0 (low) to 1.0 (high)
        std::string anomalyType;  ///< "spike", "dip", "outlier", "drift"
        std::chrono::system_clock::time_point detectionTime;
        std::string componentName;
        std::string description;
    };

    /**
     * Performance bottleneck analysis result.
     */
    struct BottleneckAnalysisResult {
        std::string bottleneckId;
        std::string componentName;
        MetricType metricType;
        double currentValue;
        double optimalValue;
        double degradationFactor;
        std::string bottleneckType;  ///< "CPU", "MEMORY", "IO", "NETWORK", "ALGORITHM"
        double impactScore;           ///< 0.0 (low) to 1.0 (high)
        std::string suggestedFix;
    };

    /**
     * Performance prediction result.
     */
    struct PredictionResult {
        MetricType metricType;
        std::vector<double> predictedValues;
        std::vector<std::chrono::system_clock::time_point> predictionTimes;
        double confidence;  ///< Prediction confidence
        std::string modelType;  ///< "linear", "exponential", "moving_average", "ARIMA"
        std::chrono::system_clock::duration predictionHorizon;
    };

    /**
     * Performance configuration structure.
     */
    struct PerformanceConfig {
        bool enableRealTimeMonitoring = true;
        bool enableHistoricalAnalysis = true;
        size_t dataRetentionHours = 24;
        size_t maxDataPoints = 1000000;
        double anomalyThreshold = 3.0;  ///< Standard deviations from mean
        size_t trendAnalysisWindow = 1000;
        bool enableAutomatedOptimization = false;
        double optimizationThreshold = 0.95;
        bool enableVisualization = true;
        std::string outputDirectory = "./performance_data";
        bool enableBenchmarking = true;
        size_t benchmarkSamples = 5;
    };

public:
    /**
     * Initialize the performance monitoring system.
     * 
     * @param config Configuration parameters for the performance monitor
     */
    explicit PerformanceMonitor(const PerformanceConfig& config = PerformanceConfig());
    
    /**
     * Clean up the performance monitor.
     */
    ~PerformanceMonitor();
    
    /**
     * Disable copying.
     */
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    /**
     * Get the singleton instance of the performance monitor.
     * 
     * @return Reference to the global performance monitor instance
     */
    static PerformanceMonitor& getInstance(const PerformanceConfig& config = PerformanceConfig());
    
    /**
     * Start the performance monitoring system.
     */
    void start();
    
    /**
     * Stop the performance monitoring system.
     */
    void stop();
    
    /**
     * Check if the performance monitor is running.
     * 
     * @return true if monitoring is active, false otherwise
     */
    bool isRunning() const { return running_.load(); }

public:
    // Performance Monitoring Tools
    /**
     * Real-time performance monitoring.
     * 
     * @param interval Monitoring interval in milliseconds
     * @return true if monitoring started successfully
     */
    bool performanceMonitor(double interval = 1000.0);
    
    /**
     * Create performance profiler.
     * 
     * @param name Name of the profiler
     * @param interval Profiling interval in milliseconds
     * @return true if profiler created successfully
     */
    bool createPerformanceProfiler(const std::string& name, double interval = 1000.0);
    
    /**
     * Start performance profiling.
     * 
     * @param profilerName Name of the profiler to start
     * @return true if profiling started successfully
     */
    bool startPerformanceProfile(const std::string& profilerName);
    
    /**
     * Stop performance profiling.
     * 
     * @param profilerName Name of the profiler to stop
     * @return true if profiling stopped successfully
     */
    bool stopPerformanceProfile(const std::string& profilerName);
    
    /**
     * Profile memory usage patterns.
     * 
     * @param componentName Name of the component to profile
     * @return Memory usage data in bytes
     */
    uint64_t profileMemoryUsage(const std::string& componentName = "");
    
    /**
     * Profile CPU usage statistics.
     * 
     * @param componentName Name of the component to profile
     * @return CPU usage percentage
     */
    double profileCPUUsage(const std::string& componentName = "");
    
    /**
     * Profile network usage statistics.
     * 
     * @param componentName Name of the component to profile
     * @return Network usage in bytes per second
     */
    uint64_t profileNetworkUsage(const std::string& componentName = "");
    
    /**
     * Profile disk I/O patterns.
     * 
     * @param componentName Name of the component to profile
     * @return Disk I/O statistics
     */
    struct DiskIOStats {
        uint64_t readBytes;      ///< Bytes read
        uint64_t writeBytes;     ///< Bytes written
        uint64_t readOperations; ///< Read operations
        uint64_t writeOperations;///< Write operations
        double readSpeed;        ///< Bytes per second read
        double writeSpeed;       ///< Bytes per second written
    };
    DiskIOStats profileDiskUsage(const std::string& componentName = "");

public:
    // Profiling Tools
    /**
     * Profile neural computation performance.
     * 
     * @param componentName Name of the neural component
     * @param iterations Number of computation iterations
     * @return Neural computation metrics
     */
    struct NeuralComputationMetrics {
        double computationTime;     ///< Total computation time in seconds
        uint64_t neuronsProcessed;  ///< Number of neurons processed
        uint64_t spikesGenerated;   ///< Number of spikes generated
        double spikesPerSecond;    ///< Average spikes per second
        double avgComputationTime; ///< Average computation time per neuron
        uint64_t totalConnections; ///< Total connections processed
    };
    NeuralComputationMetrics profileNeuralComputation(const std::string& componentName = "", size_t iterations = 1000);
    
    /**
     * Profile plasticity rule execution.
     * 
     * @param componentName Name of the plasticity component
     * @param iterations Number of plasticity iterations
     * @return Plasticity performance metrics
     */
    struct PlasticityMetrics {
        double executionTime;        ///< Total execution time in seconds
        uint64_t rulesApplied;       ///< Number of rules applied
        double rulesPerSecond;      ///< Average rules per second
        double avgRuleTime;          ///< Average time per rule
        uint64_t weightUpdates;      ///< Total weight updates
        double weightChangeSum;      ///< Total weight change
    };
    PlasticityMetrics profilePlasticityRules(const std::string& componentName = "", size_t iterations = 1000);
    
    /**
     * Profile memory system performance.
     * 
     * @param componentName Name of the memory component
     * @param iterations Number of memory operations
     * @return Memory system performance metrics
     */
    struct MemoryMetrics {
        double accessTime;          ///< Average memory access time in microseconds
        uint64_t totalOperations;   ///< Total memory operations
        uint64_t successfulOps;     ///< Successful operations
        uint64_t failedOps;         ///< Failed operations
        double operationsPerSecond; ///< Operations per second
        uint64_t cacheHitRate;      ///< Cache hit rate percentage
        uint64_t memoryAllocated;   ///< Total memory allocated
        uint64_t memoryFreed;       ///< Total memory freed
    };
    MemoryMetrics profileMemoryOperations(const std::string& componentName = "", size_t iterations = 100000);
    
    /**
     * Profile simulation step performance.
     * 
     * @param componentName Name of the simulation component
     * @param iterations Number of simulation steps
     * @return Simulation step performance metrics
     */
    struct SimulationStepMetrics {
        double stepTime;            ///< Average step time in seconds
        double stepTimeMin;         ///< Minimum step time
        double stepTimeMax;         ///< Maximum step time
        double stepTimeStdDev;      ///< Standard deviation of step time
        uint64_t totalSteps;        ///< Total simulation steps
        uint64_t completedSteps;    ///< Completed steps
        uint64_t failedSteps;       ///< Failed steps
        double stepsPerSecond;      ///< Steps per second
        double simulationTime;      ///< Total simulation time
        uint64_t neuronUpdates;     ///< Number of neuron state updates
        uint64_t connectionUpdates; ///< Number of connection updates
    };
    SimulationStepMetrics profileSimulationSteps(const std::string& componentName = "", size_t iterations = 1000);
    
    /**
     * Profile configuration loading performance.
     * 
     * @param configFilePath Path to the configuration file
     * @return Configuration loading performance metrics
     */
    struct ConfigLoadingMetrics {
        double loadTime;           ///< Time to load configuration in seconds
        uint64_t fileSize;         ///< Size of configuration file in bytes
        size_t parametersLoaded;   ///< Number of parameters loaded
        double loadSpeed;          ///< Parameters per second
        bool loadSuccess;          ///< Whether loading was successful
    };
    ConfigLoadingMetrics profileConfigurationLoad(const std::string& configFilePath = "");
    
    /**
     * Profile logging system performance.
     * 
     * @param componentName Name of the logging component
     * @param iterations Number of log operations
     * @return Logging performance metrics
     */
    struct LoggingMetrics {
        double logTime;            ///< Average log operation time in microseconds
        uint64_t totalLogs;        ///< Total log operations
        uint64_t logSuccess;       ///< Successful log operations
        uint64_t logFailed;        ///< Failed log operations
        double logsPerSecond;      ///< Logs per second
        uint64_t totalBytesLogged; ///< Total bytes logged
        double avgLogSize;         ///< Average log size
    };
    LoggingMetrics profileLoggingPerformance(const std::string& componentName = "", size_t iterations = 10000);
    
    /**
     * Profile network communication performance.
     * 
     * @param componentName Name of the network component
     * @param iterations Number of network operations
     * @return Network communication performance metrics
     */
    struct NetworkMetrics {
        double commTime;           ///< Average communication time in milliseconds
        uint64_t totalMessages;    ///< Total messages sent/received
        uint64_t successfulComm;   ///< Successful communications
        uint64_t failedComm;       ///< Failed communications
        double messagesPerSecond; ///< Messages per second
        uint64_t totalBytesSent;   ///< Total bytes sent
        uint64_t totalBytesReceived;///< Total bytes received
        double avgMessageSize;     ///< Average message size
    };
    NetworkMetrics profileNetworkCommunication(const std::string& componentName = "", size_t iterations = 1000);
    
    /**
     * Profile file I/O performance.
     * 
     * @param filePath Path to the file to profile
     * @param operations Number of file operations
     * @return File I/O performance metrics
     */
    struct FileIOMetrics {
        double opTime;             ///< Average file operation time in milliseconds
        uint64_t totalOperations;  ///< Total file operations
        uint64_t successfulOps;   ///< Successful operations
        uint64_t failedOps;        ///< Failed operations
        double opsPerSecond;       ///< Operations per second
        uint64_t totalBytesRead;   ///< Total bytes read
        uint64_t totalBytesWritten;///< Total bytes written
        double avgFileSize;        ///< Average file size
    };
    FileIOMetrics profileFileOperations(const std::string& filePath = "", size_t operations = 100);

public:
    // Performance Analysis Tools
    /**
     * Analyze performance trends over time.
     * 
     * @param metricType Type of metric to analyze
     * @param componentName Component to analyze
     * @param duration Analysis duration in hours
     * @return Trend analysis results
     */
    std::vector<TrendAnalysisResult> analyzePerformanceTrends(
        MetricType metricType, 
        const std::string& componentName = "",
        double duration = 24.0);
    
    /**
     * Detect performance anomalies.
     * 
     * @param metricType Type of metric to analyze
     * @param componentName Component to analyze
     * @param threshold Anomaly threshold (standard deviations)
     * @return Detected anomalies
     */
    std::vector<AnomalyDetectionResult> detectPerformanceAnomalies(
        MetricType metricType,
        const std::string& componentName = "",
        double threshold = 3.0);
    
    /**
     * Identify performance bottlenecks.
     * 
     * @param componentName Component to analyze
     * @param duration Analysis duration in hours
     * @return Identified bottlenecks
     */
    std::vector<BottleneckAnalysisResult> identifyPerformanceBottlenecks(
        const std::string& componentName = "",
        double duration = 24.0);
    
    /**
     * Predict future performance based on patterns.
     * 
     * @param metricType Type of metric to predict
     * @param componentName Component to analyze
     * @param horizon Prediction horizon in hours
     * @param modelType Prediction model type
     * @return Prediction results
     */
    PredictionResult predictPerformance(
        MetricType metricType,
        const std::string& componentName = "",
        double horizon = 24.0,
        const std::string& modelType = "moving_average");
    
    /**
     * Compare performance across configurations.
     * 
     * @param configNames List of configuration names to compare
     * @param metricType Type of metric to compare
     * @return Comparison results
     */
    struct ComparisonResult {
        std::string configName;
        double metricValue;
        double rank;  ///< Rank in comparison (1 = best)
        double improvementFactor;  ///< Improvement over baseline
    };
    std::vector<ComparisonResult> comparePerformance(
        const std::vector<std::string>& configNames,
        MetricType metricType);
    
    /**
     * Benchmark performance across different runs.
     * 
     * @param benchmarkName Name of the benchmark
     * @param iterations Number of benchmark iterations
     * @param description Description of the benchmark
     * @return Benchmark results
     */
    struct BenchmarkResult {
        std::string benchmarkName;
        double totalTime;            ///< Total benchmark time
        double avgTime;             ///< Average time per iteration
        double minTime;             ///< Minimum time
        double maxTime;             ///< Maximum time
        double stdDev;              ///< Standard deviation
        uint64_t totalOperations;   ///< Total operations performed
        double opsPerSecond;        ///< Operations per second
        bool success;               ///< Whether benchmark completed successfully
    };
    BenchmarkResult benchmarkPerformance(
        const std::string& benchmarkName,
        size_t iterations = 100,
        const std::string& description = "");
    
    /**
     * Automatically optimize performance.
     * 
     * @param targetMetric Target metric to optimize
     * @param componentName Component to optimize
     * @param threshold Optimization threshold
     * @return Optimization results
     */
    struct OptimizationResult {
        bool optimized;             ///< Whether optimization was performed
        double improvement;         ///< Performance improvement percentage
        std::string optimizationType;  ///< Type of optimization applied
        std::string componentName;   ///< Component that was optimized
        double oldValue;            ///< Original metric value
        double newValue;            ///< Optimized metric value
        std::string recommendation; ///< Recommendation for further optimization
    };
    OptimizationResult optimizePerformance(
        MetricType targetMetric,
        const std::string& componentName = "",
        double threshold = 0.95);

public:
    // Resource Monitoring Tools
    /**
     * Monitor system resource usage.
     * 
     * @param componentName Component to monitor
     * @return System resource usage metrics
     */
    struct SystemResourceMetrics {
        double cpuUsage;           ///< CPU usage percentage
        uint64_t memoryUsed;       ///< Memory used in bytes
        uint64_t memoryAvailable;  ///< Available memory in bytes
        uint64_t memoryTotal;      ///< Total memory in bytes
        double memoryUsage;        ///< Memory usage percentage
        uint64_t diskUsed;         ///< Disk used in bytes
        uint64_t diskTotal;        ///< Total disk space in bytes
        double diskUsage;          ///< Disk usage percentage
        uint64_t networkSent;      ///< Network bytes sent
        uint64_t networkReceived;  ///< Network bytes received
        uint64_t openFiles;        ///< Number of open file handles
        uint64_t threadCount;      ///< Number of active threads
        uint64_t contextSwitches; ///< Number of context switches
        uint64_t pageFaults;       ///< Number of page faults
        double systemLoad;         ///< System load average
    };
    SystemResourceMetrics monitorSystemResources(const std::string& componentName = "");
    
    /**
     * Monitor memory allocation patterns.
     * 
     * @param componentName Component to monitor
     * @return Memory allocation metrics
     */
    struct MemoryAllocationMetrics {
        uint64_t totalAllocated;   ///< Total memory allocated
        uint64_t totalFreed;       ///< Total memory freed
        uint64_t currentUsage;     ///< Current memory usage
        uint64_t allocationCount;  ///< Number of allocation operations
        uint64_t deallocationCount;///< Number of deallocation operations
        double allocationTime;     ///< Average allocation time in microseconds
        double deallocationTime;   ///< Average deallocation time in microseconds
        uint64_t fragmentation;    ///< Memory fragmentation percentage
        uint64_t poolHits;         ///< Memory pool hit rate
    };
    MemoryAllocationMetrics monitorMemoryAllocation(const std::string& componentName = "");
    
    /**
     * Monitor CPU utilization.
     * 
     * @param componentName Component to monitor
     * @return CPU utilization metrics
     */
    struct CPUUtilizationMetrics {
        double utilization;        ///< Current CPU utilization percentage
        double utilizationMin;    ///< Minimum CPU utilization
        double utilizationMax;    ///< Maximum CPU utilization
        double utilizationStdDev; ///< Standard deviation of utilization
        uint64_t totalInstructions;///< Total instructions executed
        uint64_t totalCycles;      ///< Total CPU cycles
        double instructionsPerCycle;///< Instructions per cycle
        std::vector<double> coreUtilization;  ///< Per-core utilization
    };
    CPUUtilizationMetrics monitorCPUUtilization(const std::string& componentName = "");
    
    /**
     * Monitor network bandwidth usage.
     * 
     * @param componentName Component to monitor
     * @return Network bandwidth metrics
     */
    struct NetworkBandwidthMetrics {
        double uploadSpeed;        ///< Upload speed in bytes per second
        double downloadSpeed;      ///< Download speed in bytes per second
        uint64_t uploadTotal;      ///< Total bytes uploaded
        uint64_t downloadTotal;    ///< Total bytes downloaded
        double packetLoss;         ///< Packet loss rate
        double latency;            ///< Network latency in milliseconds
        uint64_t connectionCount;  ///< Number of active connections
        double bandwidthUsage;     ///< Bandwidth utilization percentage
    };
    NetworkBandwidthMetrics monitorNetworkBandwidth(const std::string& componentName = "");
    
    /**
     * Monitor disk usage and I/O.
     * 
     * @param componentName Component to monitor
     * @return Disk usage metrics
     */
    struct DiskUsageMetrics {
        uint64_t totalSpace;       ///< Total disk space
        uint64_t usedSpace;        ///< Used disk space
        uint64_t freeSpace;        ///< Free disk space
        double usagePercentage;    ///< Disk usage percentage
        uint64_t readOperations;   ///< Number of read operations
        uint64_t writeOperations;  ///< Number of write operations
        uint64_t readBytes;        ///< Bytes read
        uint64_t writeBytes;       ///< Bytes written
        double readSpeed;          ///< Read speed in bytes/second
        double writeSpeed;         ///< Write speed in bytes/second
        double ioWaitTime;         ///< I/O wait time percentage
    };
    DiskUsageMetrics monitorDiskUsage(const std::string& componentName = "");
    
    /**
     * Monitor file handle usage.
     * 
     * @param componentName Component to monitor
     * @return File handle metrics
     */
    struct FileHandleMetrics {
        uint64_t openFiles;        ///< Number of open files
        uint64_t maxOpenFiles;     ///< Maximum number of open files
        uint64_t fileDescriptors; ///< Number of file descriptors
        uint64_t directoryHandles; ///< Number of open directory handles
        double fileOpenTime;      ///< Average file open time in milliseconds
        double fileCloseTime;     ///< Average file close time in milliseconds
        uint64_t readHandles;      ///< Number of read handles
        uint64_t writeHandles;     ///< Number of write handles
        uint64_t sharedHandles;    ///< Number of shared handles
    };
    FileHandleMetrics monitorFileHandles(const std::string& componentName = "");
    
    /**
     * Monitor process-specific resources.
     * 
     * @param componentName Component to monitor
     * @return Process resource metrics
     */
    struct ProcessResourceMetrics {
        pid_t processId;           ///< Process ID
        std::string processName;  ///< Process name
        double cpuTime;           ///< CPU time used by process
        uint64_t memoryUsage;      ///< Memory usage by process
        uint64_t virtualMemory;    ///< Virtual memory usage
        uint64_t sharedMemory;     ///< Shared memory usage
        uint64_t textMemory;       ///< Code segment memory
        uint64_t dataMemory;       ///< Data segment memory
        uint64_t stackMemory;      ///< Stack segment memory
        uint64_t pageFaults;       ///< Page faults
        uint64_t signalsReceived; ///< Signals received
        uint64_t voluntaryContextSwitches; ///< Voluntary context switches
        uint64_t involuntaryContextSwitches;///< Involuntary context switches
    };
    ProcessResourceMetrics monitorProcessResources(const std::string& componentName = "");

public:
    // Performance Visualization Tools
    /**
     * Create performance visualization charts.
     * 
     * @param metricType Type of metric to visualize
     * @param componentName Component to visualize
     * @param duration Chart duration in hours
     * @param chartType Chart type ("line", "bar", "heatmap")
     * @return Chart data
     */
    struct ChartData {
        std::string chartType;
        std::string title;
        std::vector<std::chrono::system_clock::time_point> timestamps;
        std::vector<double> values;
        std::vector<std::string> labels;
        std::string xAxisLabel;
        std::string yAxisLabel;
        std::string outputFilePath;
        bool generated;  ///< Whether chart was generated successfully
    };
    ChartData createPerformanceCharts(
        MetricType metricType,
        const std::string& componentName = "",
        double duration = 24.0,
        const std::string& chartType = "line");
    
    /**
     * Generate comprehensive performance reports.
     * 
     * @param reportType Type of report ("summary", "detailed", "anomaly", "trend")
     * @param componentName Component to report on
     * @param duration Report duration in hours
     * @return Report data
     */
    struct ReportData {
        std::string reportType;
        std::string title;
        std::chrono::system_clock::time_point generationTime;
        std::string content;  ///< Report content (text, HTML, etc.)
        std::string outputFilePath;
        std::map<std::string, double> summaryStats;
        std::vector<std::string> keyFindings;
        std::vector<std::string> recommendations;
        bool generated;  ///< Whether report was generated successfully
    };
    ReportData generatePerformanceReports(
        const std::string& reportType = "summary",
        const std::string& componentName = "",
        double duration = 24.0);
    
    /**
     * Create performance graphs and charts.
     * 
     * @param graphType Type of graph ("line", "scatter", "histogram", "heatmap")
     * @param metricType Metric to graph
     * @param componentName Component to graph
     * @param duration Graph duration in hours
     * @return Graph data
     */
    struct GraphData {
        std::string graphType;
        std::string title;
        std::vector<double> xData;
        std::vector<double> yData;
        std::vector<std::string> seriesLabels;
        std::string xAxisLabel;
        std::string yAxisLabel;
        std::map<std::string, std::string> styling;
        std::string outputFilePath;
        bool generated;  ///< Whether graph was generated successfully
    };
    GraphData createPerformanceGraphs(
        const std::string& graphType = "line",
        MetricType metricType = MetricType::CPU_USAGE,
        const std::string& componentName = "",
        double duration = 24.0);
    
    /**
     * Visualize performance as heatmaps.
     * 
     * @param metricType Metric to visualize
     * @param componentName Component to visualize
     * @param duration Heatmap duration in hours
     * @param bins Number of bins for the heatmap
     * @return Heatmap data
     */
    struct HeatmapData {
        std::vector<std::vector<double>> heatmapMatrix;
        std::vector<std::string> xLabels;
        std::vector<std::string> yLabels;
        double minValue;
        double maxValue;
        std::string colorScheme;  ///< "viridis", "plasma", "hot", "cool"
        std::string outputFilePath;
        bool generated;  ///< Whether heatmap was generated successfully
    };
    HeatmapData visualizePerformanceHeatmap(
        MetricType metricType,
        const std::string& componentName = "",
        double duration = 24.0,
        size_t bins = 100);
    
    /**
     * Create performance timelines.
     * 
     * @param componentName Component to timeline
     * @param duration Timeline duration in hours
     * @param events Timeline events
     * @return Timeline data
     */
    struct TimelineData {
        std::string title;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::vector<std::tuple<std::chrono::system_clock::time_point, std::string, std::string>> events;
        std::string outputFilePath;
        bool generated;  ///< Whether timeline was generated successfully
    };
    TimelineData createPerformanceTimeline(
        const std::string& componentName = "",
        double duration = 24.0,
        const std::vector<std::tuple<std::string, std::string>>& events = {});
    
    /**
     * Generate performance dashboards.
     * 
     * @param dashboardType Type of dashboard ("system", "component", "detailed")
     * @param components List of components to include
     * @param updateInterval Update interval in seconds
     * @return Dashboard data
     */
    struct DashboardData {
        std::string dashboardType;
        std::vector<std::string> components;
        double updateInterval;  ///< Update interval in seconds
        std::chrono::system_clock::time_point lastUpdate;
        std::string outputFilePath;
        std::string dashboardHtml;  ///< Dashboard HTML content
        bool generated;  ///< Whether dashboard was generated successfully
    };
    DashboardData generatePerformanceDashboards(
        const std::string& dashboardType = "system",
        const std::vector<std::string>& components = {},
        double updateInterval = 60.0);
    
    /**
     * Export performance data for analysis.
     * 
     * @param outputFormat Output format ("csv", "json", "parquet")
     * @param componentName Component to export
     * @param metricTypes Metrics to export
     * @param duration Export duration in hours
     * @return Export results
     */
    struct ExportResult {
        std::string outputFormat;
        std::string outputFilePath;
        size_t recordCount;  ///< Number of records exported
        double fileSize;    ///< Size of exported file in bytes
        bool success;       ///< Whether export was successful
        std::string errorMessage;  ///< Error message if export failed
    };
    ExportResult exportPerformanceData(
        const std::string& outputFormat = "csv",
        const std::string& componentName = "",
        const std::vector<MetricType>& metricTypes = {},
        double duration = 24.0);

public:
    // Advanced Performance Tools
    /**
     * Profile hardware-specific performance.
     * 
     * @param hardwareType Hardware type ("cpu", "gpu", "memory", "storage")
     * @param componentName Component to profile
     * @return Hardware performance metrics
     */
    struct HardwarePerformanceMetrics {
        std::string hardwareType;
        std::string hardwareModel;
        std::string hardwareVendor;
        double utilization;      ///< Hardware utilization percentage
        double temperature;      ///< Temperature in Celsius
        double powerConsumption; ///< Power consumption in watts
        double frequency;        ///< Operating frequency
        uint64_t operationsPerSecond; ///< Operations per second
        double efficiency;        ///< Performance efficiency
        std::map<std::string, double> capabilities;  ///< Hardware capabilities
    };
    HardwarePerformanceMetrics profileHardwarePerformance(
        const std::string& hardwareType = "cpu",
        const std::string& componentName = "");
    
    /**
     * Profile software-specific performance.
     * 
     * @param softwareType Software type ("algorithm", "library", "framework")
     * @param componentName Component to profile
     * @param iterations Number of iterations
     * @return Software performance metrics
     */
    struct SoftwarePerformanceMetrics {
        std::string softwareType;
        std::string softwareVersion;
        double executionTime;    ///< Total execution time
        uint64_t totalOperations; ///< Total operations performed
        double operationsPerSecond; ///< Operations per second
        double memoryUsage;      ///< Memory usage in MB
        double cpuUsage;         ///< CPU usage percentage
        double cacheHitRate;     ///< Cache hit rate
        std::map<std::string, double> optimizationMetrics; ///< Optimization metrics
    };
    SoftwarePerformanceMetrics profileSoftwarePerformance(
        const std::string& softwareType = "algorithm",
        const std::string& componentName = "",
        size_t iterations = 1000);
    
    /**
     * Profile network performance.
     * 
     * @param networkType Network type ("wired", "wireless", "distributed")
     * @param componentName Component to profile
     * @param iterations Number of network operations
     * @return Network performance metrics
     */
    struct NetworkPerformanceMetrics {
        std::string networkType;
        std::string networkTopology;
        double throughput;       ///< Throughput in Mbps
        double latency;          ///< Latency in milliseconds
        double packetLossRate;   ///< Packet loss rate
        uint64_t connectionCount; ///< Number of connections
        double bandwidthUtilization; ///< Bandwidth utilization percentage
        double reliabilityScore; ///< Network reliability score
        std::vector<double> hopLatencies; ///< Latency per hop
    };
    NetworkPerformanceMetrics profileNetworkPerformance(
        const std::string& networkType = "wired",
        const std::string& componentName = "",
        size_t iterations = 1000);
    
    /**
     * Profile database performance (if applicable).
     * 
     * @param databaseType Database type ("relational", "document", "key_value")
     * @param componentName Component to profile
     * @return Database performance metrics
     */
    struct DatabasePerformanceMetrics {
        std::string databaseType;
        std::string databaseVersion;
        double queryTime;        ///< Average query time in milliseconds
        uint64_t totalQueries;   ///< Total queries executed
        uint64_t successfulQueries; ///< Successful queries
        uint64_t failedQueries;  ///< Failed queries
        double queriesPerSecond; ///< Queries per second
        uint64_t dataSize;       ///< Data size in MB
        double cacheHitRate;     ///< Cache hit rate
        uint64_t connectionPoolSize; ///< Connection pool size
    };
    DatabasePerformanceMetrics profileDatabasePerformance(
        const std::string& databaseType = "relational",
        const std::string& componentName = "");
    
    /**
     * Profile distributed system performance.
     * 
     * @param componentName Component to profile
     * @param duration Profile duration in seconds
     * @return Distributed system performance metrics
     */
    struct DistributedPerformanceMetrics {
        uint64_t nodeCount;      ///< Number of nodes in the system
        double averageLatency;   ///< Average latency in milliseconds
        double throughput;      ///< System throughput
        double availability;     ///< System availability percentage
        double consistencyScore; ///< Consistency score
        uint64_t totalMessages;  ///< Total messages exchanged
        uint64_t messageLoss;    ///< Message loss count
        double faultTolerance;  ///< Fault tolerance score
        std::map<std::string, double> nodeMetrics;  ///< Per-node metrics
    };
    DistributedPerformanceMetrics profileDistributedPerformance(
        const std::string& componentName = "",
        double duration = 60.0);
    
    /**
     * Profile cloud-based performance.
     * 
     * @param cloudProvider Cloud provider ("aws", "gcp", "azure", "oci")
     * @param componentName Component to profile
     * @param duration Profile duration in seconds
     * @return Cloud performance metrics
     */
    struct CloudPerformanceMetrics {
        std::string cloudProvider;
        std::string region;
        double computeTime;      ///< Compute time in seconds
        uint64_t computeCost;    ///< Compute cost in USD
        double storageUsage;    ///< Storage usage in GB
        double networkEgress;   ///< Network egress in GB
        double apiLatency;       ///< API latency in milliseconds
        uint64_t activeConnections; ///< Active connections
        double autoScalingScore; ///< Auto-scaling efficiency
        double SLACompliance;    ///< SLA compliance percentage
    };
    CloudPerformanceMetrics profileCloudPerformance(
        const std::string& cloudProvider = "aws",
        const std::string& componentName = "",
        double duration = 60.0);
    
    /**
     * Profile edge computing performance.
     * 
     * @param edgeDeviceType Edge device type ("iot", "edge_server", "gateway")
     * @param componentName Component to profile
     * @param duration Profile duration in seconds
     * @return Edge computing performance metrics
     */
    struct EdgePerformanceMetrics {
        std::string edgeDeviceType;
        std::string deviceId;
        double computationLatency; ///< Computation latency in milliseconds
        double memoryUsage;       ///< Memory usage in MB
        double energyConsumption; ///< Energy consumption in watts
        double networkBandwidth;  ///< Network bandwidth in Mbps
        double inferenceTime;     ///< Inference time in milliseconds
        double modelSize;        ///< Model size in MB
        double accuracy;          ///< Model accuracy percentage
        double robustnessScore;   ///< Robustness score
    };
    EdgePerformanceMetrics profileEdgePerformance(
        const std::string& edgeDeviceType = "iot",
        const std::string& componentName = "",
        double duration = 60.0);

public:
    // Performance Optimization Tools
    /**
     * Optimize memory usage patterns.
     * 
     * @param componentName Component to optimize
     * @param targetUsage Target memory usage in bytes
     * @return Optimization results
     */
    OptimizationResult optimizeMemoryUsage(
        const std::string& componentName = "",
        uint64_t targetUsage = 0);
    
    /**
     * Optimize CPU usage.
     * 
     * @param componentName Component to optimize
     * @param targetUtilization Target CPU utilization percentage
     * @return Optimization results
     */
    OptimizationResult optimizeCPUUsage(
        const std::string& componentName = "",
        double targetUtilization = 80.0);
    
    /**
     * Optimize network usage.
     * 
     * @param componentName Component to optimize
     * @param targetBandwidth Target network bandwidth in Mbps
     * @return Optimization results
     */
    OptimizationResult optimizeNetworkUsage(
        const std::string& componentName = "",
        double targetBandwidth = 100.0);
    
    /**
     * Optimize file system usage.
     * 
     * @param componentName Component to optimize
     * @param targetFragmentation Target fragmentation percentage
     * @return Optimization results
     */
    OptimizationResult optimizeFileSystemUsage(
        const std::string& componentName = "",
        double targetFragmentation = 10.0);
    
    /**
     * Optimize configuration for performance.
     * 
     * @param componentName Component to optimize
     * @param config optimization configuration
     * @return Optimization results
     */
    OptimizationResult optimizeConfiguration(
        const std::string& componentName = "",
        const std::map<std::string, std::string>& config = {});
    
    /**
     * Optimize logging for performance.
     * 
     * @param componentName Component to optimize
     * @param logLevel Target log level
     * @param logRotationTarget Target log rotation frequency
     * @return Optimization results
     */
    OptimizationResult optimizeLogging(
        const std::string& componentName = "",
        const std::string& logLevel = "INFO",
        const std::string& logRotationTarget = "daily");
    
    /**
     * Optimize database queries (if applicable).
     * 
     * @param queryOptimizationRules Query optimization rules
     * @param componentName Component to optimize
     * @return Optimization results
     */
    OptimizationResult optimizeDatabaseQueries(
        const std::vector<std::string>& queryOptimizationRules = {},
        const std::string& componentName = "");

private:
    // Internal helper functions
    /**
     * Collect performance data for a specific metric.
     */
    double collectPerformanceData(MetricType metricType, const std::string& componentName = "");
    
    /**
     * Store performance data point.
     */
    void storePerformanceData(const PerformanceDataPoint& dataPoint);
    
    /**
     * Analyze performance trend using linear regression.
     */
    TrendAnalysisResult analyzeTrend(const std::vector<PerformanceDataPoint>& dataPoints);
    
    /**
     * Detect anomalies using statistical methods.
     */
    AnomalyDetectionResult detectAnomaly(const std::vector<PerformanceDataPoint>& dataPoints);
    
    /**
     * Identify bottlenecks using performance profiling.
     */
    BottleneckAnalysisResult identifyBottleneck(const std::vector<PerformanceDataPoint>& dataPoints);
    
    /**
     * Generate predictions using time series analysis.
     */
    PredictionResult predictPerformance(const std::vector<PerformanceDataPoint>& dataPoints);
    
    /**
     * Generate visualizations.
     */
    bool generateVisualization(const ChartData& chartData);
    
    /**
     * Export data to file.
     */
    bool exportData(const std::string& outputFilePath, const std::vector<PerformanceDataPoint>& dataPoints);
    
    /**
     * Clean up old data.
     */
    void cleanupOldData();

private:
    // Internal state and data
    std::atomic<bool> running_{false};
    PerformanceConfig config_;
    
    // Performance data storage
    std::deque<PerformanceDataPoint> performanceData_;
    std::map<std::string, std::deque<PerformanceDataPoint>> componentData_;
    std::mutex dataMutex_;
    
    // Active profilers
    std::map<std::string, std::chrono::system_clock::time_point> activeProfilers_;
    std::mutex profilersMutex_;
    
    // Performance monitoring thread
    std::thread monitoringThread_;
    
    // Real-time performance data
    std::map<MetricType, std::atomic<double>> realTimeMetrics_;
    std::map<std::string, std::atomic<uint64_t>> realTimeCounters_;
    std::mutex metricsMutex_;
    
    // Performance statistics
    struct Statistics {
        double mean = 0.0;
        double stdDev = 0.0;
        double min = 0.0;
        double max = 0.0;
        double median = 0.0;
        size_t count = 0;
    };
    std::map<MetricType, Statistics> metricStats_;
    std::mutex statsMutex_;
};