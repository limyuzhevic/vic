#pragma once

/**
 * Real-Time Monitor
 * 
 * Live performance monitoring for the NLM brain simulation project.
 * 
 * Features:
 * - Real-time performance visualization
 * - Performance threshold monitoring
 * - Live statistics calculation
 * - Anomaly detection
 * - Performance alerting
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace nlm {

class RealTimeMonitor {
public:
    /**
     * Performance data point
     */
    struct PerformanceDataPoint {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string component_name;
        double cpu_usage;
        double memory_usage;
        double disk_io;
        double network_io;
        double response_time;
        int thread_count;
        double memory_fragmentation;
    };

    /**
     * Performance alert
     */
    struct PerformanceAlert {
        std::string component_name;
        std::string alert_type; // "threshold", "anomaly", "degradation"
        std::string message;
        double severity; // 0.0 to 1.0
        std::chrono::high_resolution_clock::time_point timestamp;
        std::function<void()> action;
    };

    RealTimeMonitor();
    ~RealTimeMonitor();
    
    /** Record performance data */
    void recordData(const PerformanceDataPoint& data_point);
    
    /** Get latest performance data */
    const std::vector<PerformanceDataPoint>& getLatestData() const { return data_points_; }
    
    /** Get performance statistics */
    struct Statistics {
        double average_cpu_usage;
        double average_memory_usage;
        double max_cpu_usage;
        double max_memory_usage;
        double min_response_time;
        double max_response_time;
    };
    
    Statistics getStatistics() const;
    
    /** Get performance alerts */
    const std::vector<PerformanceAlert>& getAlerts() const { return alerts_; }
    
    /** Add performance alert */
    void addAlert(const std::string& component_name, const std::string& alert_type,
                  const std::string& message, double severity = 0.5);
    
    /** Set performance thresholds */
    void setThresholds(const std::unordered_map<std::string, double>& thresholds);
    
    /** Get threshold for component */
    double getThreshold(const std::string& component_name) const;
    
    /** Start monitoring */
    void startMonitoring();
    
    /** Stop monitoring */
    void stopMonitoring();
    
    /** Check for alerts based on current data */
    void checkForAlerts();
    
    /** Get monitoring status */
    bool isMonitoring() const { return monitoring_; }
    
    /** Get data sampling interval */
    std::chrono::milliseconds getSamplingInterval() const { return sampling_interval_; }
    
    /** Set data sampling interval */
    void setSamplingInterval(std::chrono::milliseconds interval) { sampling_interval_ = interval; }
    
private:
    std::vector<PerformanceDataPoint> data_points_;
    std::vector<PerformanceAlert> alerts_;
    std::unordered_map<std::string, double> thresholds_;
    bool monitoring_;
    std::chrono::milliseconds sampling_interval_;
    size_t max_data_points_;
};

} // namespace nlm
