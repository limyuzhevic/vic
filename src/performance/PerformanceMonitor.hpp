#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <atomic>
#include <memory>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

// Performance monitoring and statistics collection
class PerformanceMonitor {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start monitoring
    void start(const std::string& operation = "default");
    
    // Stop monitoring and record results
    double stop();
    
    // Get current elapsed time
    double getElapsedTime() const;
    
    // Check if monitoring is active
    bool isActive() const;
    
    // Get total time for an operation
    double getTotalTime(const std::string& operation) const;
    
    // Get average time for an operation
    double getAverageTime(const std::string& operation) const;
    
    // Get number of times operation was recorded
    size_t getRecordCount(const std::string& operation) const;
    
    // Get all operation names
    std::vector<std::string> getOperationNames() const;
    
    // Get performance statistics
    std::string getStatistics() const;
    
    // Clear all statistics
    void clear();
    
    // Get minimum time for operation
    double getMinTime(const std::string& operation) const;
    
    // Get maximum time for operation
    double getMaxTime(const std::string& operation) const;
    
    // Get performance level (operations per second)
    double getPerformanceLevel(const std::string& operation) const;
    
    // Set whether to track memory usage
    void setTrackMemory(bool track);
    
    // Set sample interval for statistics
    void setSampleInterval(double interval);
    
    // Export performance data to JSON
    bool exportToJSON(const std::string& filepath) const;
    
    // Export performance data to CSV
    bool exportToCSV(const std::string& filepath) const;
    
    // Add custom metric
    void addMetric(const std::string& name, double value);
    
    // Get custom metric value
    std::optional<double> getMetric(const std::string& name) const;
    
    // Get all custom metric names
    std::vector<std::string> getMetricNames() const;
    
    // Generate performance report
    std::string generateReport() const;
};

// Resource usage monitoring
class ResourceMonitor {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    ResourceMonitor();
    ~ResourceMonitor();
    
    // Start resource monitoring
    void start();
    
    // Stop resource monitoring
    void stop();
    
    // Get current memory usage in MB
    double getCurrentMemoryUsage() const;
    
    // Get peak memory usage in MB
    double getPeakMemoryUsage() const;
    
    // Get current CPU usage percentage
    double getCurrentCPUUsage() const;
    
    // Get memory usage history
    std::vector<double> getMemoryUsageHistory() const;
    
    // Get CPU usage history
    std::vector<double> getCPUUsageHistory() const;
    
    // Get timestamp of last measurement
    std::chrono::system_clock::time_point getLastMeasurementTime() const;
    
    // Get measurement interval
    double getMeasurementInterval() const;
    
    // Set measurement interval
    void setMeasurementInterval(double interval);
    
    // Enable/disable monitoring
    void enable(bool enable);
    
    // Check if monitoring is enabled
    bool isEnabled() const;
    
    // Clear history
    void clear();
    
    // Get resource usage statistics
    std::string getStatistics() const;
};

// Experiment performance analyzer
class ExperimentPerformanceAnalyzer {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    ExperimentPerformanceAnalyzer();
    ~ExperimentPerformanceAnalyzer();
    
    // Add experiment performance data
    void addExperimentPerformance(const std::string& experimentName,
                                 const std::string& operation,
                                 double executionTime,
                                 double memoryUsage = 0.0);
    
    // Get performance metrics for an experiment
    struct ExperimentMetrics {
        std::string name;
        double totalTime;
        double averageTime;
        double minTime;
        double maxTime;
        double memoryUsage;
        size_t executionCount;
        double performanceScore;
    };
    
    std::optional<ExperimentMetrics> getExperimentMetrics(const std::string& experimentName) const;
    
    // Get all experiment metrics
    std::vector<ExperimentMetrics> getAllExperimentMetrics() const;
    
    // Calculate performance score (higher is better)
    static double calculatePerformanceScore(double executionTime, 
                                          double memoryUsage,
                                          double optimalTime,
                                          double optimalMemory);
    
    // Get overall performance statistics
    std::string getOverallStatistics() const;
    
    // Generate performance comparison report
    std::string generateComparisonReport(const std::vector<std::string>& experimentNames) const;
    
    // Export performance data
    bool exportToJSON(const std::string& filepath) const;
    bool exportToCSV(const std::string& filepath) const;
    
    // Clear all data
    void clear();
    
    // Get experiment count
    size_t getExperimentCount() const;
};

// Real-time performance metrics collector
class RealTimeMetricsCollector {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    RealTimeMetricsCollector();
    ~RealTimeMetricsCollector();
    
    // Start collecting metrics
    void start();
    
    // Stop collecting metrics
    void stop();
    
    // Record a metric value
    void record(const std::string& name, double value,
               const std::map<std::string, std::string>& tags = {});
    
    // Get latest metric value
    std::optional<double> getLatest(const std::string& name) const;
    
    // Get metric history
    std::vector<std::pair<double, double>> getHistory(const std::string& name) const;
    
    // Get all metric names
    std::vector<std::string> getMetricNames() const;
    
    // Calculate statistics for a metric
    struct MetricStatistics {
        double min;
        double max;
        double mean;
        double stddev;
        double median;
        size_t count;
    };
    
    MetricStatistics calculateStatistics(const std::string& name) const;
    
    // Get metrics with tags
    std::vector<std::pair<std::string, double>> getMetricsWithTag(const std::string& tagName,
                                                                 const std::string& tagValue) const;
    
    // Filter metrics by tag
    std::vector<std::pair<std::string, double>> filterByTag(const std::string& tagName,
                                                          const std::string& tagValue) const;
    
    // Export metrics
    bool exportToJSON(const std::string& filepath) const;
    bool exportToCSV(const std::string& filepath) const;
    
    // Clear all metrics
    void clear();
    
    // Get total metric count
    size_t getTotalMetricCount() const;
};

// Convenience functions
inline std::shared_ptr<PerformanceMonitor> createPerformanceMonitor() {
    return std::make_shared<PerformanceMonitor>();
}

inline std::shared_ptr<ResourceMonitor> createResourceMonitor() {
    return std::make_shared<ResourceMonitor>();
}

inline std::shared_ptr<ExperimentPerformanceAnalyzer> createPerformanceAnalyzer() {
    return std::make_shared<ExperimentPerformanceAnalyzer>();
}

inline std::shared_ptr<RealTimeMetricsCollector> createMetricsCollector() {
    return std::make_shared<RealTimeMetricsCollector>();
}

// Performance monitoring utilities
namespace perf_utils {
    // Format bytes to human readable string
    std::string formatBytes(size_t bytes);
    
    // Format time for display
    std::string formatTime(double seconds);
    
    // Calculate frames per second
    double calculateFPS(double timePerFrame);
    
    // Calculate efficiency (useful work / total time)
    double calculateEfficiency(double usefulTime, double totalTime);
    
    // Normalize values to a common scale
    double normalize(double value, double min, double max);
    
    // Interpolate values
    double interpolate(double x, double x1, double y1, double x2, double y2);
    
    // Smooth moving average
    class MovingAverage {
    private:
        std::vector<double> values;
        size_t windowSize;
        size_t currentIndex;
    
    public:
        MovingAverage(size_t size = 10);
        void addValue(double value);
        double getAverage() const;
        void clear();
    };
    
    // Exponential moving average
    class ExponentialMovingAverage {
    private:
        double alpha;
        double current;
        bool initialized;
    
    public:
        ExponentialMovingAverage(double smoothingFactor = 0.1);
        void addValue(double value);
        double getAverage() const;
        void reset();
    };
}

} // namespace nlm