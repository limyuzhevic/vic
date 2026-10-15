#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <condition_variable>
#include <queue>
#include "ExperimentProtocol.hpp"
#include "ExperimentStateManager.hpp"

namespace nlm {

// Experiment monitoring configuration
struct ExperimentMonitorConfig {
    std::string name;
    bool enabled;
    std::chrono::milliseconds checkInterval;
    std::chrono::milliseconds alertThreshold;
    std::vector<std::string> monitoredExperiments;
    std::map<std::string, double> resourceThresholds;
    std::map<std::string, double> performanceThresholds;
    std::map<std::string, std::string> notificationChannels;
    bool enableLogging;
    bool enableAlerts;
    bool enableAutoRecovery;
    bool enablePerformanceAnalysis;
    
    ExperimentMonitorConfig() 
        : enabled(true), checkInterval(std::chrono::milliseconds(1000)), 
          alertThreshold(std::chrono::milliseconds(30000)), enableLogging(true),
          enableAlerts(true), enableAutoRecovery(true), enablePerformanceAnalysis(true) {}
};

// Monitoring metrics
struct MonitoringMetrics {
    std::string experimentName;
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, double> resourceUsage; // CPU, memory, disk, network
    std::map<std::string, double> performanceMetrics; // throughput, latency, efficiency
    std::map<std::string, double> protocolMetrics; // protocol-specific metrics
    std::map<std::string, int> eventCounts;
    std::map<std::string, double> errorRates;
    double systemLoad;
    double memoryUsage;
    
    MonitoringMetrics() : systemLoad(0.0), memoryUsage(0.0), timestamp(std::chrono::system_clock::now()) {}
};

// Monitoring alert
struct MonitoringAlert {
    std::string alertId;
    std::string experimentName;
    std::chrono::system_clock::time_point timestamp;
    std::string alertType;
    std::string severity;
    std::string message;
    double currentValue;
    double thresholdValue;
    std::map<std::string, std::string> alertData;
    bool isAcknowledged;
    bool isResolved;
    
    MonitoringAlert() : timestamp(std::chrono::system_clock::now()), isAcknowledged(false), isResolved(false) {}
    MonitoringAlert(const std::string& id, const std::string& type, const std::string& severity, 
                   const std::string& msg, double current, double threshold)
        : alertId(id), alertType(type), severity(severity), message(msg), 
          currentValue(current), thresholdValue(threshold), 
          timestamp(std::chrono::system_clock::now()), isAcknowledged(false), isResolved(false) {}
};

// Monitoring event
struct MonitoringEvent {
    std::string eventId;
    std::string experimentName;
    std::chrono::system_clock::time_point timestamp;
    std::string eventType;
    std::string eventSource;
    std::map<std::string, std::string> eventData;
    std::map<std::string, std::string> metadata;
    
    MonitoringEvent() : timestamp(std::chrono::system_clock::now()) {}
    MonitoringEvent(const std::string& expName, const std::string& type, const std::string& source)
        : experimentName(expName), eventType(type), eventSource(source), timestamp(std::chrono::system_clock::now()) {}
};

// Alert callback type
using AlertCallback = std::function<void(const MonitoringAlert&)>
;
// Event callback type
using EventCallback = std::function<void(const MonitoringEvent&)>
;

class ExperimentMonitor {
public:
    ExperimentMonitor();
    ~ExperimentMonitor();
    
    // Disable copying, enable moving
    ExperimentMonitor(const ExperimentMonitor&) = delete;
    ExperimentMonitor& operator=(const ExperimentMonitor&) = delete;
    ExperimentMonitor(ExperimentMonitor&&) noexcept;
    ExperimentMonitor& operator=(ExperimentMonitor&&) noexcept;
    
    // Monitoring management
    bool startMonitoring(const std::string& experimentName);
    bool stopMonitoring(const std::string& experimentName);
    bool pauseMonitoring(const std::string& experimentName);
    bool resumeMonitoring(const std::string& experimentName);
    bool enableMonitor(const std::string& monitorName, bool enable);
    
    // Metric collection
    bool collectMetrics(const std::string& experimentName, const MonitoringMetrics& metrics);
    bool updateResourceUsage(const std::string& experimentName, 
                           const std::map<std::string, double>& resources);
    bool updatePerformanceMetrics(const std::string& experimentName,
                                 const std::map<std::string, double>& metrics);
    bool recordEvent(const std::string& experimentName, const MonitoringEvent& event);
    
    // Alert management
    bool addAlertCallback(AlertCallback callback);
    bool removeAlertCallback(AlertCallback callback);
    bool addEventCallback(EventCallback callback);
    bool removeEventCallback(EventCallback callback);
    bool clearCallbacks();
    
    // Alert handling
    bool triggerAlert(const MonitoringAlert& alert);
    bool acknowledgeAlert(const std::string& alertId);
    bool resolveAlert(const std::string& alertId);
    bool dismissAlert(const std::string& alertId);
    
    // Query and analysis
    MonitoringMetrics getLatestMetrics(const std::string& experimentName) const;
    std::vector<MonitoringMetrics> getMetricsHistory(const std::string& experimentName, 
                                                    size_t limit = 100) const;
    std::vector<MonitoringAlert> getActiveAlerts(const std::string& experimentName) const;
    std::vector<MonitoringAlert> getAllActiveAlerts() const;
    std::vector<MonitoringEvent> getEventHistory(const std::string& experimentName, 
                                                size_t limit = 100) const;
    
    // Performance analysis
    double calculatePerformanceTrend(const std::string& experimentName, 
                                     const std::string& metricName) const;
    double calculateEfficiency(const std::string& experimentName) const;
    std::string analyzeResourceUsage(const std::string& experimentName) const;
    std::string analyzePerformanceDegradation(const std::string& experimentName) const;
    
    // Predictive analysis
    bool predictResourceRequirements(const std::string& experimentName, 
                                    std::map<std::string, double>& predictedResources) const;
    bool predictPerformance(const std::string& experimentName, 
                           std::map<std::string, double>& predictedMetrics) const;
    bool detectAnomalies(const std::string& experimentName) const;
    
    // Alert threshold management
    bool setResourceThreshold(const std::string& experimentName, const std::string& resource, 
                             double threshold);
    bool setPerformanceThreshold(const std::string& experimentName, const std::string& metric, 
                               double threshold);
    double getResourceThreshold(const std::string& experimentName, const std::string& resource) const;
    double getPerformanceThreshold(const std::string& experimentName, const std::string& metric) const;
    
    // Export and reporting
    bool exportMetrics(const std::string& experimentName, const std::string& filepath, 
                      const std::string& format = "csv") const;
    bool exportAlerts(const std::string& experimentName, const std::string& filepath, 
                     const std::string& format = "json") const;
    bool exportEvents(const std::string& experimentName, const std::string& filepath, 
                    const std::string& format = "json") const;
    
    // Cleanup and maintenance
    bool cleanupOldMetrics(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupOldAlerts(const std::chrono::system_clock::time_point& cutoffTime);
    bool cleanupOldEvents(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetMonitoring(const std::string& experimentName);
    bool clearAllMonitoringData();
    std::vector<std::string> getMonitoredExperiments() const;
    size_t getMonitoredExperimentCount() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm