// Enhanced status reporting for NLM
// Provides detailed system status, brain state, and resource usage reports

#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <atomic>

namespace nlm {

// System component status
struct SystemComponentStatus {
    std::string name;
    bool isOperational;
    std::string status; // "active", "idle", "error", "starting", "stopping"
    double performance; // 0.0 to 1.0
    std::string lastUpdate;
    std::string details;
    
    SystemComponentStatus() : isOperational(false), performance(0.0) {}
    SystemComponentStatus(const std::string& name, bool operational, const std::string& status)
        : name(name), isOperational(operational), status(status), performance(1.0) {}
};

// Brain state metrics
struct BrainStateMetrics {
    size_t neuronCount;
    size_t synapseCount;
    size_t firingCount;
    float averageFiringRate;
    float averageMembranePotential;
    float averageInputCurrent;
    float energyConsumption; // in arbitrary units
    float computationalLoad; // 0.0 to 1.0
    std::chrono::system_clock::time_point lastUpdate;
    
    BrainStateMetrics() : neuronCount(0), synapseCount(0), firingCount(0), 
                         averageFiringRate(0.0), averageMembranePotential(0.0),
                         averageInputCurrent(0.0), energyConsumption(0.0),
                         computationalLoad(0.0) {}
};

// Resource usage metrics
struct ResourceMetrics {
    double cpuUsage; // percentage 0.0 to 100.0
    double memoryUsage; // in MB
    double memoryAvailable; // in MB
    double memoryReserved; // in MB
    double diskUsage; // in MB
    double networkUsage; // in MB/s
    size_t threadCount;
    size_t openFileHandles;
    std::chrono::system_clock::time_point timestamp;
    
    ResourceMetrics() : cpuUsage(0.0), memoryUsage(0.0), memoryAvailable(0.0),
                       memoryReserved(0.0), diskUsage(0.0), networkUsage(0.0),
                       threadCount(0), openFileHandles(0) {}
};

// Configuration validation results
struct ValidationResult {
    std::string section;
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::chrono::system_clock::time_point timestamp;
    
    ValidationResult() : isValid(true), timestamp(std::chrono::system_clock::now()) {}
    ValidationResult(const std::string& section, bool valid) 
        : section(section), isValid(valid), timestamp(std::chrono::system_clock::now()) {}
};

// Performance metrics
struct PerformanceMetrics {
    double cpuPerformance; // normalized 0.0 to 1.0
    double memoryEfficiency; // normalized 0.0 to 1.0
    double throughput; // operations per second
    double latency; // in milliseconds
    double reliability; // 0.0 to 1.0
    std::chrono::system_clock::time_point timestamp;
    
    PerformanceMetrics() : cpuPerformance(0.0), memoryEfficiency(0.0), 
                          throughput(0.0), latency(0.0), reliability(0.0) {}
};

// Live monitoring data point
struct MonitoringDataPoint {
    std::chrono::system_clock::time_point timestamp;
    std::string component;
    std::string metric;
    double value;
    std::string unit;
    std::map<std::string, std::string> tags;
    
    MonitoringDataPoint() : value(0.0) {}
    MonitoringDataPoint(const std::string& component, const std::string& metric, 
                       double value, const std::string& unit)
        : component(component), metric(metric), value(value), unit(unit) {}
};

class EnhancedStatusReporter {
public:
    EnhancedStatusReporter();
    ~EnhancedStatusReporter();
    
    // Configure status reporting
    void setUpdateInterval(std::chrono::milliseconds interval);
    void setIncludeResourceMetrics(bool include);
    void setIncludeBrainMetrics(bool include);
    void setIncludePerformanceMetrics(bool include);
    void setUseColors(bool useColors);
    void setMonitoringEnabled(bool enabled);
    void setAlertThresholds(const std::map<std::string, double>& thresholds);
    
    // Component status management
    void addComponentStatus(const SystemComponentStatus& status);
    void updateComponentStatus(const std::string& name, bool operational, 
                               const std::string& status, double performance = 1.0,
                               const std::string& details = "");
    void removeComponentStatus(const std::string& name);
    void clearComponentStatus();
    
    // Resource monitoring
    void updateResourceMetrics(const ResourceMetrics& metrics);
    void startResourceMonitoring(const std::chrono::milliseconds& interval = std::chrono::milliseconds(1000));
    void stopResourceMonitoring();
    
    // Brain state reporting
    void updateBrainState(const BrainStateMetrics& metrics);
    void setBrain(std::shared_ptr<Brain> brain);
    
    // Configuration validation
    void addValidationResult(const ValidationResult& result);
    void addValidationError(const std::string& section, const std::string& error);
    void addValidationWarning(const std::string& section, const std::string& warning);
    void clearValidationResults();
    
    // Performance metrics
    void updatePerformanceMetrics(const PerformanceMetrics& metrics);
    void addPerformanceMeasurement(const std::string& operation, double duration,
                                   size_t memoryUsed = 0);
    void clearPerformanceMetrics();
    
    // Live monitoring
    void addMonitoringDataPoint(const MonitoringDataPoint& dataPoint);
    void addMonitoringDataPoint(const std::string& component, const std::string& metric,
                               double value, const std::string& unit = "",
                               const std::map<std::string, std::string>& tags = {});
    void clearMonitoringData();
    std::vector<MonitoringDataPoint> getMonitoringData(const std::string& component = "",
                                                      const std::string& metric = "",
                                                      const std::chrono::system_clock::time_point& since = 
                                                      std::chrono::system_clock::time_point::min()) const;
    
    // Generate comprehensive status report
    std::string generateSystemStatusReport() const;
    std::string generateBrainStateReport() const;
    std::string generateResourceReport() const;
    std::string generateConfigurationValidationReport() const;
    std::string generatePerformanceReport() const;
    std::string generateLiveMonitoringReport() const;
    
    // Generate summary report
    std::string generateSummaryReport() const;
    
    // Alert detection and reporting
    struct Alert {
        std::string component;
        std::string metric;
        std::string severity; // "info", "warning", "error", "critical"
        std::string message;
        std::chrono::system_clock::time_point timestamp;
        double value;
        double threshold;
    };
    
    void checkAlerts();
    std::vector<Alert> getAlerts() const;
    void clearAlerts();
    void setAlertCallback(const std::function<void(const Alert&)>& callback);
    
    // Real-time monitoring dashboard
    void startDashboard(std::chrono::seconds updateInterval = std::chrono::seconds(5));
    void stopDashboard();
    bool isDashboardRunning() const;
    std::string generateDashboardOutput() const;
    
    // Export functionality
    bool exportStatusReport(const std::string& filepath, const std::string& format = "json",
                           const std::string& reportType = "comprehensive") const;
    
    // Configuration validation and summary reporting (already implemented)
    bool validateConfiguration() const;
    std::string getConfigurationSummary() const;
    
    // Performance metrics reporting (already implemented)
    double getAverageCPUUsage() const;
    double getPeakCPUUsage() const;
    double getAverageMemoryUsage() const;
    double getPeakMemoryUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    void formatTimestamp(std::stringstream& ss, std::chrono::system_clock::time_point timestamp) const;
    std::string getSeverityColor(const std::string& severity) const;
    bool checkThreshold(const std::string& metric, double value, double threshold) const;
    
    // Alert detection
    void detectResourceAlerts();
    void detectBrainAlerts();
    void detectPerformanceAlerts();
    
    // Dashboard rendering
    void renderDashboard() const;
    void renderComponentStatus(std::stringstream& ss, const SystemComponentStatus& status) const;
    void renderResourceMetrics(std::stringstream& ss, const ResourceMetrics& metrics) const;
    void renderBrainState(std::stringstream& ss, const BrainStateMetrics& metrics) const;
};

} // namespace nlm
