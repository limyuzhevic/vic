// Performance Monitoring and Profiling Tools
// This module provides comprehensive performance monitoring for NLM simulations

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <mutex>

namespace nlm {

/**
 * @brief Performance metrics collector for NLM simulations
 * 
 * This class collects and analyzes performance metrics during NLM simulations,
 * including timing, memory usage, CPU usage, and bottleneck identification.
 * 
 * Key features:
 * - Real-time performance monitoring
 * - Historical performance tracking
 * - Bottleneck detection and analysis
 * - Performance trend analysis
 * - Export capabilities (JSON, CSV, HTML)
 * - Memory usage monitoring
 * - CPU usage monitoring
 * - Timeline-based performance analysis
 */
class PerformanceMonitor {
public:
    /**
     * @brief Performance metric structure
     * 
     * Contains detailed performance information for a specific metric or operation.
     */
    struct PerformanceMetric {
        std::string name;
        std::string category;
        double value;
        double minValue;
        double maxValue;
        double average;
        double standardDeviation;
        size_t count;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::unordered_map<std::string, std::string> tags;
        
        PerformanceMetric(const std::string& name, const std::string& category, double value)
            : name(name), category(category), value(value), minValue(value), maxValue(value),
              average(value), standardDeviation(0.0), count(1), timestamp(std::chrono::high_resolution_clock::now()) {}
    };
    
    /**
     * @brief Performance profiling session
     * 
     * Contains detailed profiling information for a specific simulation session.
     */
    struct ProfilingSession {
        std::string sessionId;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        std::vector<PerformanceMetric> metrics;
        std::unordered_map<std::string, double> aggregates;
        std::string description;
        
        ProfilingSession(const std::string& id, const std::string& desc = "")
            : sessionId(id), description(desc) {
            startTime = std::chrono::high_resolution_clock::now();
        }
        
        double getDuration() const {
            auto end = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double>(end - startTime).count();
        }
    };
    
    /**
     * @brief Performance monitor configuration
     * 
     * Configuration options for the performance monitoring system.
     */
    struct MonitorConfig {
        bool enableRealTimeMonitoring = true;
        bool enableHistoricalTracking = true;
        bool enableMemoryMonitoring = true;
        bool enableCPUMonitoring = true;
        bool enableBottleneckDetection = true;
        size_t maxHistoricalMetrics = 10000;
        double samplingInterval = 0.1; // seconds
        std::string outputFormat = "json"; // json, csv, html
        std::string outputFile = "performance.log";
        bool exportOnSessionEnd = true;
        std::vector<std::string> monitoredCategories = {
            "simulation", "memory", "cpu", "neural", "plasticity", "attention"
        };
    };
    
    PerformanceMonitor(const MonitorConfig& config = MonitorConfig());
    ~PerformanceMonitor();
    
    /**
     * @brief Start performance monitoring for a session
     * 
     * @param sessionId Unique identifier for the profiling session
     * @param description Description of the session
     * @return std::string Session ID for later reference
     */
    std::string startSession(const std::string& sessionId, const std::string& description = "");
    
    /**
     * @brief End current performance monitoring session
     * 
     * @param sessionId Session ID to end
     * @return bool true if session was ended successfully
     */
    bool endSession(const std::string& sessionId);
    
    /**
     * @brief Record a performance metric
     * 
     * @param sessionId Session ID for the metric
     * @param name Name of the metric
     * @param category Category of the metric (e.g., "simulation", "memory")
     * @param value Numeric value of the metric
     * @param tags Optional tags for categorization
     */
    void recordMetric(const std::string& sessionId, const std::string& name, 
                     const std::string& category, double value,
                     const std::unordered_map<std::string, std::string>& tags = {});
    
    /**
     * @brief Get current performance metrics
     * 
     * @param sessionId Session ID to retrieve metrics for
     * @return std::vector<PerformanceMetric> Vector of performance metrics
     */
    std::vector<PerformanceMetric> getMetrics(const std::string& sessionId) const;
    
    /**
     * @brief Get aggregated performance metrics
     * 
     * @param sessionId Session ID to aggregate metrics for
     * @param category Optional category to filter by
     * @return std::unordered_map<std::string, double> Aggregated metrics
     */
    std::unordered_map<std::string, double> getAggregatedMetrics(const std::string& sessionId, 
                                                                 const std::string& category = "") const;
    
    /**
     * @brief Detect performance bottlenecks
     * 
     * @param sessionId Session ID to analyze for bottlenecks
     * @return std::vector<std::string> List of detected bottleneck issues
     */
    std::vector<std::string> detectBottlenecks(const std::string& sessionId) const;
    
    /**
     * @brief Get performance statistics
     * 
     * @param sessionId Session ID to get statistics for
     * @return std::unordered_map<std::string, double> Performance statistics
     */
    std::unordered_map<std::string, double> getStatistics(const std::string& sessionId) const;
    
    /**
     * @brief Export performance data to file
     * 
     * @param sessionId Session ID to export
     * @param filename Output file path
     * @param format Export format (json, csv, html)
     * @return bool true if export was successful
     */
    bool exportData(const std::string& sessionId, const std::string& filename, 
                   const std::string& format = "json");
    
    /**
     * @brief Generate performance report
     * 
     * @param sessionId Session ID to generate report for
     * @return std::string Performance report as string
     */
    std::string generateReport(const std::string& sessionId) const;
    
    /**
     * @brief Clear old sessions to free memory
     * 
     * @param maxAge Maximum age of sessions to keep (in seconds)
     */
    void cleanupOldSessions(double maxAge = 3600.0); // 1 hour default
    
    /**
     * @brief Enable or disable performance monitoring
     * 
     * @param enable true to enable monitoring, false to disable
     */
    void setEnabled(bool enable) { enabled_ = enable; }
    
    /**
     * @brief Check if performance monitoring is enabled
     * 
     * @return bool true if monitoring is enabled
     */
    bool isEnabled() const { return enabled_; }
    
    /**
     * @brief Get list of active sessions
     * 
     * @return std::vector<std::string> List of active session IDs
     */
    std::vector<std::string> getActiveSessions() const;
    
private:
    /**
     * @brief Internal performance metric collection
     */
    void collectPerformanceData();
    
    /**
     * @brief Update performance aggregates
     */
    void updateAggregates(const PerformanceMetric& metric);
    
    /**
     * @brief Generate bottleneck report
     */
    std::vector<std::string> generateBottleneckReport(const ProfilingSession& session) const;
    
    /**
     * @brief Export metrics to JSON format
     */
    bool exportToJSON(const std::string& filename, const ProfilingSession& session) const;
    
    /**
     * @brief Export metrics to CSV format
     */
    bool exportToCSV(const std::string& filename, const ProfilingSession& session) const;
    
    /**
     * @brief Export metrics to HTML format
     */
    bool exportToHTML(const std::string& filename, const ProfilingSession& session) const;
    
    MonitorConfig config_;
    bool enabled_ = true;
    std::unordered_map<std::string, ProfilingSession> sessions_;
    mutable std::mutex mutex_;
    std::thread monitoringThread_;
    bool monitoringThreadRunning_ = false;
};

/**
 * @brief Simple performance timer for quick measurements
 * 
 * Lightweight performance timer that can measure the duration of specific
 * operations or code sections.
 */
class PerformanceTimer {
public:
    PerformanceTimer(const std::string& name, const std::string& category = "general");
    ~PerformanceTimer();
    
    /**
     * @brief Get elapsed time in seconds
     * 
     * @return double Elapsed time in seconds
     */
    double getElapsed() const;
    
    /**
     * @brief Reset timer
     */
    void reset();
    
    /**
     * @brief Check if timer is running
     * 
     * @return bool true if timer is running
     */
    bool isRunning() const { return running_; }
    
    /**
     * @brief Get timer name
     * 
     * @return std::string Timer name
     */
    std::string getName() const { return name_; }
    
    /**
     * @brief Get timer category
     * 
     * @return std::string Timer category
     */
    std::string getCategory() const { return category_; }
    
private:
    std::string name_;
    std::string category_;
    std::chrono::high_resolution_clock::time_point startTime_;
    bool running_ = false;
};

/**
 * @brief Memory usage monitor
 * 
 * Tracks memory usage during NLM simulations to identify memory leaks
 * and optimize memory allocation patterns.
 */
class MemoryUsageMonitor {
public:
    MemoryUsageMonitor();
    ~MemoryUsageMonitor();
    
    /**
     * @brief Get current memory usage in MB
     * 
     * @return double Current memory usage in MB
     */
    double getCurrentMemoryUsage() const;
    
    /**
     * @brief Get peak memory usage in MB
     * 
     * @return double Peak memory usage in MB
     */
    double getPeakMemoryUsage() const;
    
    /**
     * @brief Reset memory usage tracking
     */
    void reset();
    
    /**
     * @brief Start memory monitoring
     */
    void startMonitoring();
    
    /**
     * @brief Stop memory monitoring
     */
    void stopMonitoring();
    
    /**
     * @brief Check if monitoring is active
     * 
     * @return bool true if monitoring is active
     */
    bool isMonitoring() const { return monitoring_; }
    
    /**
     * @brief Get memory usage history
     * 
     * @return std::vector<double> Memory usage history (MB)
     */
    std::vector<double> getMemoryHistory() const;
    
    /**
     * @brief Get memory usage statistics
     * 
     * @return std::unordered_map<std::string, double> Memory statistics
     */
    std::unordered_map<std::string, double> getStatistics() const;
    
private:
    bool monitoring_ = false;
    double peakMemoryUsage_ = 0.0;
    std::vector<double> memoryHistory_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

/**
 * @brief CPU usage monitor
 * 
 * Tracks CPU usage during NLM simulations to optimize performance
 * and identify CPU bottlenecks.
 */
class CPUUsageMonitor {
public:
    CPUUsageMonitor();
    ~CPUUsageMonitor();
    
    /**
     * @brief Get current CPU usage percentage
     * 
     * @return double Current CPU usage percentage (0-100)
     */
    double getCurrentCPUUsage() const;
    
    /**
     * @brief Get average CPU usage percentage
     * 
     * @return double Average CPU usage percentage
     */
    double getAverageCPUUsage() const;
    
    /**
     * @brief Reset CPU usage tracking
     */
    void reset();
    
    /**
     * @brief Start CPU monitoring
     */
    void startMonitoring();
    
    /**
     * @brief Stop CPU monitoring
     */
    void stopMonitoring();
    
    /**
     * @brief Check if monitoring is active
     * 
     * @return bool true if monitoring is active
     */
    bool isMonitoring() const { return monitoring_; }
    
    /**
     * @brief Get CPU usage history
     * 
     * @return std::vector<double> CPU usage history (percentages)
     */
    std::vector<double> getCPUHistory() const;
    
    /**
     * @brief Get CPU usage statistics
     * 
     * @return std::unordered_map<std::string, double> CPU statistics
     */
    std::unordered_map<std::string, double> getStatistics() const;
    
private:
    bool monitoring_ = false;
    double totalCPUUsage_ = 0.0;
    size_t sampleCount_ = 0;
    std::vector<double> cpuHistory_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

/**
 * @brief Simulation performance profiler
 * 
 * Comprehensive profiling tool for NLM simulations that combines
 * performance monitoring, memory tracking, and CPU monitoring
 * to provide detailed performance analysis.
 */
class SimulationProfiler {
public:
    SimulationProfiler(const std::string& sessionId, const PerformanceMonitor::MonitorConfig& config = PerformanceMonitor::MonitorConfig());
    ~SimulationProfiler();
    
    /**
     * @brief Start profiling a simulation step
     * 
     * @param stepNumber Simulation step number
     * @return std::unique_ptr<PerformanceTimer> Timer for the step
     */
    std::unique_ptr<PerformanceTimer> profileStep(int stepNumber);
    
    /**
     * @brief Record simulation metrics
     * 
     * @param stepNumber Simulation step number
     * @param metrics Map of metric names to values
     */
    void recordSimulationMetrics(int stepNumber, 
                                 const std::unordered_map<std::string, double>& metrics);
    
    /**
     * @brief Get profiling results
     * 
     * @return std::string Profiling results report
     */
    std::string getResults() const;
    
    /**
     * @brief Export profiling data
     * 
     * @param filename Output file path
     * @return bool true if export was successful
     */
    bool exportData(const std::string& filename);
    
    /**
     * @brief Get performance monitor
     * 
     * @return PerformanceMonitor& Performance monitor reference
     */
    PerformanceMonitor& getPerformanceMonitor() { return performanceMonitor_; }
    
    /**
     * @brief Get memory usage monitor
     * 
     * @return MemoryUsageMonitor& Memory usage monitor reference
     */
    MemoryUsageMonitor& getMemoryUsageMonitor() { return memoryMonitor_; }
    
    /**
     * @brief Get CPU usage monitor
     * 
     * @return CPUUsageMonitor& CPU usage monitor reference
     */
    CPUUsageMonitor& getCPUUsageMonitor() { return cpuMonitor_; }
    
private:
    std::string sessionId_;
    PerformanceMonitor performanceMonitor_;
    MemoryUsageMonitor memoryMonitor_;
    CPUUsageMonitor cpuMonitor_;
};

} // namespace nlm