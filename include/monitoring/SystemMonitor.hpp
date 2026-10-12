#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <functional>
#include <map>

namespace nlm {

// Forward declarations
class AlertManager;
class MetricCollector;

// Base monitoring component
class MonitoringComponent {
public:
    virtual ~MonitoringComponent() = default;
    virtual void startMonitoring() = 0;
    virtual void stopMonitoring() = 0;
    virtual void update() = 0;
    virtual bool isMonitoring() const = 0;
    virtual double getLastUpdateTime() const = 0;
    virtual void setUpdateInterval(double interval) = 0;
    virtual double getUpdateInterval() const = 0;
};

// System monitor - Monitors CPU, memory, and I/O usage
class SystemMonitor : public MonitoringComponent {
public:
    SystemMonitor();
    ~SystemMonitor() override = default;
    
    // Initialize system monitor
    bool initialize();
    
    // Start/stop monitoring
    void startMonitoring() override;
    void stopMonitoring() override;
    
    // Update monitoring data
    void update() override;
    
    // Check if monitoring is active
    bool isMonitoring() const override;
    
    // Get monitoring data
    double getCPUUsage() const;
    double getMemoryUsage() const;
    double getIOUsage() const;
    double getDiskUsage() const;
    double getNetworkUsage() const;
    
    // Get time information
    double getLastUpdateTime() const override;
    
    // Configuration
    void setUpdateInterval(double interval) override;
    double getUpdateInterval() const override;
    
    // Threshold alerts
    void setCPUThreshold(double threshold, AlertManager* alertManager = nullptr);
    void setMemoryThreshold(double threshold, AlertManager* alertManager = nullptr);
    void setIOThreshold(double threshold, AlertManager* alertManager = nullptr);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance monitor - Monitors system performance metrics
class PerformanceMonitor : public MonitoringComponent {
public:
    PerformanceMonitor();
    ~PerformanceMonitor() override = default;
    
    // Initialize performance monitor
    bool initialize();
    
    // Start/stop monitoring
    void startMonitoring() override;
    void stopMonitoring() override;
    
    // Update performance metrics
    void update() override;
    
    // Check if monitoring is active
    bool isMonitoring() const override;
    
    // Get performance metrics
    double getCPUPerformance() const;
    double getMemoryPerformance() const;
    double getIOPerformance() const;
    double getSystemEfficiency() const;
    double getResponsiveness() const;
    
    // Get time information
    double getLastUpdateTime() const override;
    
    // Configuration
    void setUpdateInterval(double interval) override;
    double getUpdateInterval() const override;
    
    // Bottleneck detection
    void setBottleneckThreshold(double threshold, AlertManager* alertManager = nullptr);
    std::vector<std::string> getCurrentBottlenecks() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Experiment monitor - Monitors experiment progress and execution
class ExperimentMonitor : public MonitoringComponent {
public:
    ExperimentMonitor();
    ~ExperimentMonitor() override = default;
    
    // Initialize experiment monitor
    bool initialize();
    
    // Start/stop monitoring
    void startMonitoring() override;
    void stopMonitoring() override;
    
    // Update experiment progress
    void update() override;
    
    // Check if monitoring is active
    bool isMonitoring() const override;
    
    // Get experiment progress
    double getProgress() const;
    double getEstimatedTimeRemaining() const;
    std::string getCurrentPhase() const;
    std::string getCurrentStage() const;
    size_t getTotalSteps() const;
    size_t getCompletedSteps() const;
    
    // Get time information
    double getLastUpdateTime() const override;
    
    // Configuration
    void setUpdateInterval(double interval) override;
    double getUpdateInterval() const override;
    
    // Progress alerts
    void setProgressThreshold(double threshold, AlertManager* alertManager = nullptr);
    void setPhaseChangeAlert(bool enable, AlertManager* alertManager = nullptr);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Resource monitor - Monitors resource consumption patterns
class ResourceMonitor : public MonitoringComponent {
public:
    ResourceMonitor();
    ~ResourceMonitor() override = default;
    
    // Initialize resource monitor
    bool initialize();
    
    // Start/stop monitoring
    void startMonitoring() override;
    void stopMonitoring() override;
    
    // Update resource usage data
    void update() override;
    
    // Check if monitoring is active
    bool isMonitoring() const override;
    
    // Get resource metrics
    double getMemoryAllocation() const;
    double getMemoryFragmentation() const;
    double getThreadCount() const;
    double getFileDescriptorUsage() const;
    double getNetworkConnections() const;
    
    // Get time information
    double getLastUpdateTime() const override;
    
    // Configuration
    void setUpdateInterval(double interval) override;
    double getUpdateInterval() const override;
    
    // Resource alerts
    void setMemoryAllocationThreshold(double threshold, AlertManager* alertManager = nullptr);
    void setFragmentationThreshold(double threshold, AlertManager* alertManager = nullptr);
    std::vector<std::string> getResourceAlerts() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Network monitor - Monitors network performance metrics
class NetworkMonitor : public MonitoringComponent {
public:
    NetworkMonitor();
    ~NetworkMonitor() override = default;
    
    // Initialize network monitor
    bool initialize();
    
    // Start/stop monitoring
    void startMonitoring() override;
    void stopMonitoring() override;
    
    // Update network metrics
    void update() override;
    
    // Check if monitoring is active
    bool isMonitoring() const override;
    
    // Get network performance
    double getLatency() const;
    double getThroughput() const;
    double getPacketLoss() const;
    double getBandwidthUtilization() const;
    double getConnectionCount() const;
    
    // Get time information
    double getLastUpdateTime() const override;
    
    // Configuration
    void setUpdateInterval(double interval) override;
    double getUpdateInterval() const override;
    
    // Network alerts
    void setLatencyThreshold(double threshold, AlertManager* alertManager = nullptr);
    void setThroughputThreshold(double threshold, AlertManager* alertManager = nullptr);
    std::vector<std::string> getNetworkAlerts() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Anomaly detector - Automatically detects unusual patterns
class AnomalyDetector : public MonitoringComponent {
public:
    AnomalyDetector();
    ~AnomalyDetector() override = default;
    
    // Initialize anomaly detector
    bool initialize();
    
    // Start/stop monitoring
    void startMonitoring() override;
    void stopMonitoring() override;
    
    // Update anomaly detection
    void update() override;
    
    // Check if monitoring is active
    bool isMonitoring() const override;
    
    // Get anomaly data
    std::vector<std::string> getCurrentAnomalies() const;
    double getAnomalyScore() const;
    std::vector<double> getAnomalyValues() const;
    
    // Get time information
    double getLastUpdateTime() const override;
    
    // Configuration
    void setUpdateInterval(double interval) override;
    double getUpdateInterval() const override;
    
    // Anomaly detection thresholds
    void setAnomalyThreshold(double threshold, AlertManager* alertManager = nullptr);
    void setDetectionSensitivity(double sensitivity);
    void setDetectionWindow(double window);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Alert manager - Manages alerts and notifications
class AlertManager : public MonitoringComponent {
public:
    AlertManager();
    ~AlertManager() override = default;
    
    // Initialize alert manager
    bool initialize();
    
    // Start/stop monitoring
    void startMonitoring() override;
    void stopMonitoring() override;
    
    // Update alerts
    void update() override;
    
    // Check if monitoring is active
    bool isMonitoring() const override;
    
    // Alert management
    void addThresholdAlert(const std::string& metric, double threshold, AlertType type = AlertType::Warning);
    void addAnomalyAlert(const std::string& metric, std::function<bool()> detector);
    void removeAlert(const std::string& metric);
    void clearAlerts();
    
    // Get alerts
    std::vector<std::string> getActiveAlerts() const;
    std::vector<std::string> getRecentAlerts() const;
    
    // Get time information
    double getLastUpdateTime() const override;
    
    // Configuration
    void setUpdateInterval(double interval) override;
    double getUpdateInterval() const override;
    
    // Notification settings
    void setNotificationEnabled(bool enable);
    void setNotificationChannel(const std::string& channel);
    void setAlertHistorySize(size_t size);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Metric collector - Collects and aggregates metrics
class MetricCollector : public MonitoringComponent {
public:
    MetricCollector();
    ~MetricCollector() override = default;
    
    // Initialize metric collector
    bool initialize();
    
    // Start/stop monitoring
    void startMonitoring() override;
    void stopMonitoring() override;
    
    // Update metrics
    void update() override;
    
    // Check if monitoring is active
    bool isMonitoring() const override;
    
    // Metric collection
    void addCustomMetric(const std::string& name, std::function<double()> collector);
    void removeCustomMetric(const std::string& name);
    double getMetricValue(const std::string& name) const;
    std::vector<std::string> getAvailableMetrics() const;
    
    // Get time information
    double getLastUpdateTime() const override;
    
    // Configuration
    void setUpdateInterval(double interval) override;
    double getUpdateInterval() const override;
    
    // Aggregation
    void setAggregationWindow(double window);
    void enableRollingAverage(bool enable);
    void setSampleRate(double rate);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

enum class AlertType {
    Info,
    Warning,
    Error,
    Critical
};

} // namespace nlm