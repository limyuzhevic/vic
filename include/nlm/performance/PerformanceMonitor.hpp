#pragma once

/**
 * PerformanceMonitor - Real-time performance monitoring and analysis
 * 
 * Core monitoring class for NLM that provides comprehensive performance tracking,
 * including memory usage, CPU utilization, step timing, and throughput metrics.
 * 
 * Features:
 * - Thread-safe real-time monitoring
 * - Configurable threshold alerts
 * - Statistical analysis of metrics
 * - Integration with Python profiling tools
 * - Automatic resource optimization recommendations
 * 
 * Biological Motivation:
 * Neural systems maintain performance homeostasis through feedback loops
 * This monitor provides similar monitoring for large-scale neural simulations
 */

#include <chrono>
#include <memory>
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <map>
#include <unordered_map>
#include <array>
#include <span>
#include <numeric>
#include <cmath>
#include <optional>
#include <future>

namespace nlm {

/**
 * Metric value with timestamp and metadata
 */
struct MetricValue {
    std::chrono::high_resolution_clock::time_point timestamp;
    double value;
    std::string unit;
    std::unordered_map<std::string, std::string> metadata;
};

/**
 * Performance alert for threshold violations
 */
struct PerformanceAlert {
    enum class AlertLevel { INFO, WARNING, CRITICAL, ERROR };
    
    AlertLevel level;
    std::string name;
    std::string description;
    double threshold;
    double currentValue;
    std::chrono::high_resolution_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> context;
};

/**
 * Performance statistics for analysis
 */
struct PerformanceStats {
    double mean{0.0};
    double median{0.0};
    double stddev{0.0};
    double min{0.0};
    double max{0.0};
    double variance{0.0};
    std::chrono::nanoseconds duration{0};
    size_t count{0};
};

/**
 * Simulation throughput metrics
 */
struct ThroughputMetrics {
    size_t spikesPerSecond{0};
    size_t neuronsPerSecond{0};
    double realTimeFactor{1.0};
    size_t totalSteps{0};
    size_t totalSpikes{0};
    std::chrono::high_resolution_clock::time_point startTime;
};

/**
 * Resource allocation information
 */
struct ResourceAllocation {
    size_t memoryAllocated{0};
    size_t memoryUsed{0};
    size_t memoryPeak{0};
    size_t memoryReserved{0};
    size_t cpuCoresUsed{0};
    std::chrono::high_resolution_clock::duration cpuTime{0};
    double cpuUsagePercentage{0.0};
    std::vector<size_t> memoryPools;
    std::vector<size_t> threadPools;
};

/**
 * Core PerformanceMonitor class for real-time monitoring
 */
class PerformanceMonitor {
public:
    using MetricCallback = std::function<void(const MetricValue&)>
;    using AlertCallback = std::function<void(const PerformanceAlert&)>
;    using OptimizationCallback = std::function<void(const std::string&)>
;    
    /**
     * Create a performance monitor
     * @param maxHistorySize Maximum number of historical data points to retain
     * @param alertThresholdLow Low threshold for alerts (0.0 to disable)
     * @param alertThresholdHigh High threshold for alerts (0.0 to disable)
     */
    PerformanceMonitor(
        size_t maxHistorySize = 10000,
        double alertThresholdLow = 0.0,
        double alertThresholdHigh = 0.0);
    
    /**
     * Start monitoring (starts background thread)
     */
    void start();
    
    /**
     * Stop monitoring (stops background thread)
     */
    void stop();
    
    /**
     * Check if monitor is running
     */
    bool isRunning() const { return running_.load(std::memory_order_relaxed); }
    
    /**
     * Record memory usage metrics
     * @param memoryAllocated Total allocated memory (bytes)
     * @param memoryUsed Used memory (bytes)
     * @param peakMemory Peak memory usage (bytes)
     */
    void recordMemoryUsage(size_t memoryAllocated, size_t memoryUsed, size_t peakMemory);
    
    /**
     * Record CPU usage metrics
     * @param cpuUsagePercentage CPU usage percentage (0.0-100.0)
     * @param cpuCoresUsed Number of CPU cores in use
     * @param cpuTime Total CPU time consumed
     */
    void recordCPUUsage(double cpuUsagePercentage, size_t cpuCoresUsed,
                       std::chrono::high_resolution_clock::duration cpuTime);
    
    /**
     * Record step timing metrics
     * @param stepNumber Current step number
     * @param stepTime Duration of the step
     * @param spikesCount Number of spikes in this step
     * @param neuronsUpdated Number of neurons updated in this step
     */
    void recordStepMetrics(size_t stepNumber,
                           std::chrono::high_resolution_clock::duration stepTime,
                           size_t spikesCount, size_t neuronsUpdated);
    
    /**
     * Record throughput metrics
     * @param throughput Throughput metrics to record
     */
    void recordThroughput(const ThroughputMetrics& throughput);
    
    /**
     * Record simulation progress
     * @param stepNumber Current step number
     * @param maxSteps Maximum number of steps
     * @param simulatedTime Simulated time elapsed
     * @param realTime Time elapsed in real time
     */
    void recordProgress(size_t stepNumber, size_t maxSteps,
                       std::chrono::high_resolution_clock::duration simulatedTime,
                       std::chrono::high_resolution_clock::duration realTime);
    
    /**
     * Get memory usage metrics
     * @return Current memory usage metrics
     */
    ResourceAllocation getResourceAllocation() const;
    
    /**
     * Get throughput metrics
     * @return Current throughput metrics
     */
    ThroughputMetrics getThroughputMetrics() const;
    
    /**
     * Get performance statistics for a metric
     * @param metricName Name of the metric
     * @param windowSize Window size for calculation (0 = all data)
     * @return Performance statistics
     */
    PerformanceStats getPerformanceStats(const std::string& metricName,
                                        size_t windowSize = 0) const;
    
    /**
     * Get recent metric values
     * @param metricName Name of the metric
     * @param count Number of recent values to retrieve
     * @return Vector of metric values (most recent last)
     */
    std::vector<MetricValue> getRecentMetrics(const std::string& metricName,
                                            size_t count = 100) const;
    
    /**
     * Get recent performance alerts
     * @param count Number of recent alerts to retrieve
     * @return Vector of performance alerts
     */
    std::vector<PerformanceAlert> getRecentAlerts(size_t count = 50) const;
    
    /**
     * Add a custom metric callback
     * @param callback Function to call when metric is recorded
     */
    void addMetricCallback(const MetricCallback& callback);
    
    /**
     * Add a custom alert callback
     * @param callback Function to call when alert is triggered
     */
    void addAlertCallback(const AlertCallback& callback);
    
    /**
     * Add a custom optimization callback
     * @param callback Function to call when optimization suggestion is made
     */
    void addOptimizationCallback(const OptimizationCallback& callback);
    
    /**
     * Get performance bottlenecks
     * @return Vector of bottleneck descriptions
     */
    std::vector<std::string> getBottlenecks() const;
    
    /**
     * Generate performance report
     * @param format Report format
     * @param includeDetails Include detailed metrics in report
     * @return Performance report as string
     */
    std::string generateReport(const std::string& format = "text",
                              bool includeDetails = true) const;
    
    /**
     * Save performance data to file
     * @param filename File to save data to
     */
    void saveData(const std::string& filename) const;
    
    /**
     * Load performance data from file
     * @param filename File to load data from
     */
    void loadData(const std::string& filename);
    
    /**
     * Get current simulation step
     */
    size_t getCurrentStep() const { return currentStep_.load(std::memory_order_relaxed); }
    
    /**
     * Get simulation progress percentage
     */
    double getProgressPercentage() const;
    
    /**
     * Get time elapsed since start
     */
    std::chrono::high_resolution_clock::duration getElapsedTime() const;

private:
    // Thread management
    void monitoringThreadFunc();
    
    // Metric storage and management
    void storeMetric(const std::string& name, const MetricValue& value);
    void storeAlert(const PerformanceAlert& alert);
    
    // Alert threshold checking
    void checkThresholds();
    
    // Statistics calculation
    PerformanceStats calculateStats(const std::vector<double>& values) const;
    
    // Internal data structures
    mutable std::mutex metricsMutex_;
    mutable std::mutex alertsMutex_;
    std::atomic<bool> running_{false};
    std::atomic<size_t> currentStep_{0};
    
    size_t maxHistorySize_;
    double alertThresholdLow_;
    double alertThresholdHigh_;
    
    // Metric history
    std::unordered_map<std::string, std::vector<MetricValue>> metricHistory_;
    
    // Alert history
    std::vector<PerformanceAlert> alertHistory_;
    
    // Callbacks
    std::vector<MetricCallback> metricCallbacks_;
    std::vector<AlertCallback> alertCallbacks_;
    std::vector<OptimizationCallback> optimizationCallbacks_;
    
    // Current metrics
    mutable ResourceAllocation currentResourceAllocation_;
    mutable ThroughputMetrics currentThroughput_;
    std::chrono::high_resolution_clock::time_point simulationStartTime_;
    
    // Background thread
    std::thread monitoringThread_;
    std::condition_variable stopCondition_;
    std::mutex stopMutex_;
};

} // namespace nlm