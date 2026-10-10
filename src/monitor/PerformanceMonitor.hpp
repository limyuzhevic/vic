#pragma once

#include "core/Types/Types.hpp"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <deque>

namespace nlm {

// Alert structure for monitoring thresholds
struct Alert {
    std::string metricName;
    double currentValue;
    double thresholdValue;
    std::string severity; // "low", "medium", "high", "critical"
    std::string message;
    Timestamp timestamp;
};

// Metric data structure for storing measurements
struct MetricData {
    std::string name;
    double value;
    Timestamp timestamp;
    std::string source;
};

// PerformanceMonitor - Core performance monitoring class
class PerformanceMonitor {
public:
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start/stop monitoring
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Metric recording
    void recordMetric(const std::string& name, double value);
    void recordMetric(const std::string& name, double value, const std::string& source);
    
    // Metric retrieval
    std::vector<MetricData> getMetrics(const std::string& name, 
                                      Timestamp startTime, 
                                      Timestamp endTime) const;
    std::vector<MetricData> getAllMetrics(Timestamp startTime, 
                                         Timestamp endTime) const;
    
    // Alert management
    void setAlertThreshold(const std::string& name, double threshold,
                          const std::string& severity = "medium");
    void addAlertListener(std::function<void(const Alert&)> listener);
    void removeAlertListener(std::function<void(const Alert&)> listener);
    
    // Dashboard and export
    std::string generateDashboardHTML() const;
    bool exportMetrics(const std::string& filepath, const std::string& format = "csv") const;
    
    // Statistics
    size_t getMetricCount() const;
    std::map<std::string, double> getCurrentMetrics() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm