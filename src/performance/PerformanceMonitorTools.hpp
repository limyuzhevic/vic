#pragma once

/**
 * Performance Monitoring Tools for NLM
 * 
 * This module provides real-time performance monitoring capabilities for neural network simulations.
 * It includes tools for monitoring CPU usage, memory usage, network activity, disk I/O, and more.
 */

#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <memory>

namespace nlm {

/**
 * Real-time performance monitoring tool.
 * 
 * This class provides real-time performance monitoring capabilities for neural network simulations.
 * It can monitor CPU usage, memory usage, network activity, disk I/O, and other system metrics.
 */
class PerformanceMonitorTools {
public:
    /**
     * Performance monitoring configuration.
     */
    struct MonitorConfig {
        double monitoringInterval;      ///< Interval between monitoring checks in milliseconds
        bool monitorCPU;                ///< Whether to monitor CPU usage
        bool monitorMemory;             ///< Whether to monitor memory usage
        bool monitorNetwork;            ///< Whether to monitor network usage
        bool monitorDisk;               ///< Whether to monitor disk I/O
        bool monitorThreads;            ///< Whether to monitor thread count
        bool monitorSystem;             ///< Whether to monitor overall system
        std::string outputFormat;       ///< Output format for monitoring data
        std::string outputFile;         ///< File path for output
        bool enableAlerts;              ///< Whether to enable performance alerts
        double alertThreshold;          ///< Threshold for performance alerts
        std::function<void(double)> alertCallback; ///< Callback for alert notifications
    };

    /**
     * Performance monitoring data structure.
     */
    struct MonitorData {
        std::chrono::system_clock::time_point timestamp; ///< Data collection timestamp
        double cpuUsage;                                 ///< CPU usage percentage
        uint64_t memoryUsed;                             ///< Memory used in bytes
        uint64_t memoryAvailable;                        ///< Available memory in bytes
        uint64_t networkSent;                            ///< Network bytes sent
        uint64_t networkReceived;                        ///< Network bytes received
        uint64_t diskReads;                              ///< Disk read operations
        uint64_t diskWrites;                             ///< Disk write operations
        uint64_t diskReadBytes;                          ///< Bytes read from disk
        uint64_t diskWriteBytes;                         ///< Bytes written to disk
        uint64_t threadCount;                           ///< Number of active threads
        double systemLoad;                              ///< System load average
        std::string componentName;                      ///< Component name
    };

    /**
     * Performance alert structure.
     */
    struct PerformanceAlert {
        std::chrono::system_clock::time_point timestamp; ///< Alert timestamp
        std::string metricName;                         ///< Name of the metric that triggered the alert
        double value;                                   ///< Current value of the metric
        double threshold;                               ///< Threshold that was exceeded
        std::string severity;                           ///< Alert severity (LOW, MEDIUM, HIGH)
        std::string message;                            ///< Alert message
        std::string componentName;                      ///< Component name
    };

public:
    /**
     * Initialize the performance monitoring tools.
     * 
     * @param config Configuration for the performance monitoring tools
     */
    explicit PerformanceMonitorTools(const MonitorConfig& config = MonitorConfig());
    
    /**
     * Clean up the performance monitoring tools.
     */
    ~PerformanceMonitorTools();
    
    /**
     * Disable copying.
     */
    PerformanceMonitorTools(const PerformanceMonitorTools&) = delete;
    PerformanceMonitorTools& operator=(const PerformanceMonitorTools&) = delete;
    
    /**
     * Start performance monitoring.
     */
    void start();
    
    /**
     * Stop performance monitoring.
     */
    void stop();
    
    /**
     * Check if performance monitoring is active.
     * 
     * @return true if monitoring is active, false otherwise
     */
    bool isMonitoring() const { return monitoring_.load(); }
    
    /**
     * Get the latest performance monitoring data.
     * 
     * @return Latest monitoring data
     */
    MonitorData getLatestData() const;
    
    /**
     * Get all collected performance monitoring data.
     * 
     * @return Vector of all monitoring data
     */
    std::vector<MonitorData> getAllData() const;
    
    /**
     * Get performance alerts.
     * 
     * @return Vector of performance alerts
     */
    std::vector<PerformanceAlert> getAlerts() const;
    
    /**
     * Set monitoring configuration.
     * 
     * @param config New configuration
     */
    void setConfig(const MonitorConfig& config);
    
    /**
     * Get current monitoring configuration.
     * 
     * @return Current configuration
     */
    MonitorConfig getConfig() const { return config_; }
    
    /**
     * Clear all collected data.
     */
    void clearData();

private:
    /**
     * Performance monitoring loop.
     */
    void monitoringLoop();
    
    /**
     * Collect performance data.
     */
    void collectData();
    
    /**
     * Check for performance alerts.
     */
    void checkAlerts();
    
    /**
     * Process collected data.
     */
    void processData(const MonitorData& data);
    
    /**
     * Write data to output.
     */
    void writeData(const MonitorData& data);
    
    /**
     * Send alert notification.
     */
    void sendAlert(const PerformanceAlert& alert);

private:
    // Configuration
    MonitorConfig config_;
    
    // State
    std::atomic<bool> monitoring_{false};
    
    // Data
    std::vector<MonitorData> monitoringData_;
    std::vector<PerformanceAlert> alerts_;
    mutable std::mutex dataMutex_;
    
    // Performance data
    double currentCPUUsage_;
    uint64_t currentMemoryUsed_;
    uint64_t currentNetworkSent_;
    uint64_t currentNetworkReceived_;
    uint64_t currentDiskReads_;
    uint64_t currentDiskWrites_;
    uint64_t currentDiskReadBytes_;
    uint64_t currentDiskWriteBytes_;
    uint64_t currentThreadCount_;
    double currentSystemLoad_;
    mutable std::mutex performanceDataMutex_;
    
    // Monitoring thread
    std::thread monitoringThread_;
};

/**
 * Performance profiler for creating and managing performance profilers.
 * 
 * This class provides functionality to create, start, and stop performance profilers
 * for different components of neural network simulations.
 */
class PerformanceProfiler {
public:
    /**
     * Profiler configuration.
     */
    struct ProfilerConfig {
        std::string name;                ///< Name of the profiler
        double samplingInterval;         ///< Sampling interval in milliseconds
        bool enableCPUProfiling;         ///< Whether to enable CPU profiling
        bool enableMemoryProfiling;      ///< Whether to enable memory profiling
        bool enableFileIOProfiling;      ///< Whether to enable file I/O profiling
        bool enableNetworkProfiling;     ///< Whether to enable network profiling
        bool enableThreadProfiling;      ///< Whether to enable thread profiling
        std::string outputFormat;        ///< Output format for profiling data
        std::string outputFile;          ///< File path for output
        size_t maxDataPoints;            ///< Maximum number of data points to collect
        bool continuousProfiling;        ///< Whether to perform continuous profiling
    };

    /**
     * Profiler data point.
     */
    struct ProfilerDataPoint {
        std::chrono::system_clock::time_point timestamp; ///< Timestamp of the data point
        double cpuUsage;                                 ///< CPU usage at this time point
        uint64_t memoryUsage;                            ///< Memory usage in bytes
        uint64_t fileReads;                             ///< File read operations
        uint64_t fileWrites;                            ///< File write operations
        uint64_t fileReadBytes;                         ///< Bytes read from files
        uint64_t fileWriteBytes;                        ///< Bytes written to files
        uint64_t networkSent;                           ///< Network bytes sent
        uint64_t networkReceived;                       ///< Network bytes received
        uint64_t threadCount;                          ///< Thread count
        double systemLoad;                             ///< System load
        std::string componentName;                     ///< Component name
    };

public:
    /**
     * Initialize the performance profiler.
     * 
     * @param config Configuration for the profiler
     */
    explicit PerformanceProfiler(const ProfilerConfig& config = ProfilerConfig());
    
    /**
     * Clean up the performance profiler.
     */
    ~PerformanceProfiler();
    
    /**
     * Disable copying.
     */
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    
    /**
     * Start profiling.
     */
    void start();
    
    /**
     * Stop profiling.
     */
    void stop();
    
    /**
     * Check if profiling is active.
     * 
     * @return true if profiling is active, false otherwise
     */
    bool isProfiling() const { return profiling_.load(); }
    
    /**
     * Get profiler configuration.
     * 
     * @return Profiler configuration
     */
    ProfilerConfig getConfig() const { return config_; }
    
    /**
     * Set profiler configuration.
     * 
     * @param config New configuration
     */
    void setConfig(const ProfilerConfig& config);
    
    /**
     * Get profiler data.
     * 
     * @return Vector of profiler data points
     */
    std::vector<ProfilerDataPoint> getData() const;
    
    /**
     * Clear profiler data.
     */
    void clearData();
    
    /**
     * Get profiler statistics.
     * 
     * @return Profiler statistics
     */
    struct ProfilerStatistics {
        double avgCPUUsage;              ///< Average CPU usage
        double maxCPUUsage;              ///< Maximum CPU usage
        double minCPUUsage;              ///< Minimum CPU usage
        double cpuUsageStdDev;           ///< Standard deviation of CPU usage
        uint64_t totalMemoryUsage;       ///< Total memory usage
        uint64_t avgMemoryUsage;         ///< Average memory usage
        uint64_t maxMemoryUsage;         ///< Maximum memory usage
        uint64_t totalFileReads;         ///< Total file read operations
        uint64_t totalFileWrites;        ///< Total file write operations
        uint64_t totalFileReadBytes;     ///< Total bytes read from files
        uint64_t totalFileWriteBytes;    ///< Total bytes written to files
        uint64_t totalNetworkSent;       ///< Total network bytes sent
        uint64_t totalNetworkReceived;   ///< Total network bytes received
        uint64_t totalThreadCount;       ///< Total thread count
        double totalSystemLoad;          ///< Total system load
        size_t dataPointsCollected;      ///< Number of data points collected
        double profilingDuration;        ///< Total profiling duration in seconds
    };
    
    ProfilerStatistics getStatistics() const;

private:
    /**
     * Profiling loop.
     */
    void profilingLoop();
    
    /**
     * Collect profiler data.
     */
    void collectData();
    
    /**
     * Process collected data.
     */
    void processData(const ProfilerDataPoint& dataPoint);
    
    /**
     * Write data to output.
     */
    void writeData(const ProfilerDataPoint& dataPoint);

private:
    // Configuration
    ProfilerConfig config_;
    
    // State
    std::atomic<bool> profiling_{false};
    
    // Data
    std::vector<ProfilerDataPoint> profilerData_;
    mutable std::mutex dataMutex_;
    
    // Performance data
    double currentCPUUsage_;
    uint64_t currentMemoryUsage_;
    uint64_t currentFileReads_;
    uint64_t currentFileWrites_;
    uint64_t currentFileReadBytes_;
    uint64_t currentFileWriteBytes_;
    uint64_t currentNetworkSent_;
    uint64_t currentNetworkReceived_;
    uint64_t currentThreadCount_;
    double currentSystemLoad_;
    mutable std::mutex performanceDataMutex_;
    
    // Profiling thread
    std::thread profilingThread_;
    std::chrono::system_clock::time_point profilingStartTime_;
};

/**
 * Performance metrics calculator for calculating performance metrics.
 * 
 * This class provides functionality to calculate various performance metrics
 * from collected performance data, including averages, standard deviations,
 * percentiles, and other statistical measures.
 */
class PerformanceMetricsCalculator {
public:
    /**
     * Performance metric result.
     */
    struct MetricResult {
        std::string metricName;     ///< Name of the metric
        double value;              ///< Value of the metric
        std::string unit;          ///< Unit of the metric
        std::string description;   ///< Description of the metric
    };

public:
    /**
     * Calculate performance metrics from data.
     * 
     * @param data Vector of data points to analyze
     * @return Vector of metric results
     */
    std::vector<MetricResult> calculateMetrics(const std::vector<MonitorData>& data) const;
    
    /**
     * Calculate performance metrics from profiler data.
     * 
     * @param data Vector of profiler data points to analyze
     * @return Vector of metric results
     */
    std::vector<MetricResult> calculateMetrics(const std::vector<ProfilerDataPoint>& data) const;
    
    /**
     * Calculate performance metrics for a specific component.
     * 
     * @param data Vector of data points to analyze
     * @param componentName Component name
     * @return Vector of metric results
     */
    std::vector<MetricResult> calculateMetricsForComponent(
        const std::vector<MonitorData>& data,
        const std::string& componentName) const;
    
    /**
     * Calculate performance metrics for a specific time period.
     * 
     * @param data Vector of data points to analyze
     * @param startTime Start time
     * @param endTime End time
     * @return Vector of metric results
     */
    std::vector<MetricResult> calculateMetricsForTimePeriod(
        const std::vector<MonitorData>& data,
        const std::chrono::system_clock::time_point& startTime,
        const std::chrono::system_clock::time_point& endTime) const;
    
    /**
     * Calculate percentile of a metric.
     * 
     * @param values Vector of values
     * @param percentile Percentile (0-100)
     * @return Percentile value
     */
    static double calculatePercentile(const std::vector<double>& values, double percentile);
    
    /**
     * Calculate mean of values.
     * 
     * @param values Vector of values
     * @return Mean value
     */
    static double calculateMean(const std::vector<double>& values);
    
    /**
     * Calculate standard deviation of values.
     * 
     * @param values Vector of values
     * @return Standard deviation
     */
    static double calculateStandardDeviation(const std::vector<double>& values);
    
    /**
     * Calculate median of values.
     * 
     * @param values Vector of values
     * @return Median value
     */
    static double calculateMedian(const std::vector<double>& values);
    
    /**
     * Calculate variance of values.
     * 
     * @param values Vector of values
     * @return Variance
     */
    static double calculateVariance(const std::vector<double>& values);
    
    /**
     * Calculate coefficient of variation of values.
     * 
     * @param values Vector of values
     * @return Coefficient of variation (as percentage)
     */
    static double calculateCoefficientOfVariation(const std::vector<double>& values);
    
    /**
     * Calculate trend of values.
     * 
     * @param values Vector of values
     * @return Trend value (positive for increasing, negative for decreasing)
     */
    static double calculateTrend(const std::vector<double>& values);
    
    /**
     * Calculate rate of change of values.
     * 
     * @param values Vector of values
     * @return Rate of change (per unit time)
     */
    static double calculateRateOfChange(const std::vector<double>& values);
    
    /**
     * Calculate time since first value.
     * 
     * @param data Vector of data points
     * @return Time since first data point
     */
    static std::chrono::duration<double> calculateTimeSinceFirst(
        const std::vector<MonitorData>& data);
    
    /**
     * Calculate data point rate.
     * 
     * @param data Vector of data points
     * @return Data points per second
     */
    static double calculateDataPointRate(const std::vector<MonitorData>& data);
};

} // namespace nlm
