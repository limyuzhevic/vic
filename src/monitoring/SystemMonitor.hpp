// System Health Monitoring Framework for NLM
// This file provides comprehensive system health monitoring, alerting, and performance tracking

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <map>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>

namespace nlm {

// Health Status Types
enum class HealthStatus {
    Healthy = 0,
    Warning = 1,
    Degraded = 2,
    Critical = 3,
    Unhealthy = 4,
    Unknown = 5
};

// Component Health Status
struct ComponentHealth {
    std::string componentId;
    std::string componentName;
    HealthStatus status;
    std::chrono::system_clock::time_point lastCheckTime;
    std::chrono::milliseconds lastCheckDuration;
    std::string lastError;
    double healthScore; // 0.0 to 1.0
    std::unordered_map<std::string, double> metrics;
    std::vector<std::string> recentErrors;
    std::vector<std::string> recentWarnings;
    
    ComponentHealth() : status(HealthStatus::Unknown), healthScore(0.0),
                       lastCheckDuration(std::chrono::milliseconds(0)),
                       isChecked(false), consecutiveFailures(0),
                       totalChecks(0), successRate(0.0) {}
    
    ComponentHealth(const std::string& id, const std::string& name)
        : componentId(id), componentName(name), status(HealthStatus::Unknown),
          healthScore(0.0), lastCheckDuration(std::chrono::milliseconds(0)),
          isChecked(false), consecutiveFailures(0), totalChecks(0), 
          successRate(0.0) {}
    
    bool isHealthy() const { return status == HealthStatus::Healthy; }
    bool needsAttention() const { return status == HealthStatus::Warning || 
                                        status == HealthStatus::Degraded ||
                                        status == HealthStatus::Critical ||
                                        status == HealthStatus::Unhealthy; }
    
    std::string getStatusString() const;
    std::string getHealthScoreString() const;
    std::string getRecentErrorsString() const;
    
    // Statistics
    bool isChecked;
    int consecutiveFailures;
    int totalChecks;
    double successRate;
    
    // Thread safety
    mutable std::mutex mutex;
};

// System Metrics
struct SystemMetrics {
    // CPU metrics
    double cpuUsage; // Percentage (0-100)
    int cpuCores;
    double cpuLoadAverage;
    std::vector<double> cpuPerCoreUsage;
    
    // Memory metrics
    size_t totalMemory;
    size_t usedMemory;
    size_t freeMemory;
    size_t availableMemory;
    double memoryUsagePercentage;
    std::vector<size_t> memoryPerProcess;
    
    // I/O metrics
    uint64_t networkInputBytes;
    uint64_t networkOutputBytes;
    uint64_t diskReadBytes;
    uint64_t diskWriteBytes;
    double networkThroughput;
    double diskIoThroughput;
    
    // Application metrics
    size_t activeThreads;
    size_t totalThreads;
    size_t pendingTasks;
    size_t completedTasks;
    double tasksPerSecond;
    std::chrono::milliseconds averageTaskTime;
    
    // Neural network specific metrics
    size_t totalNeurons;
    size_t activeNeurons;
    size_t firingNeurons;
    size_t totalSynapses;
    double averageFiringRate;
    double excitationInhibitionRatio;
    
    // Memory system metrics
    size_t workingMemoryUsage;
    size_t workingMemoryCapacity;
    size_t episodicMemoryUsage;
    size_t episodicMemoryCapacity;
    size_t associativeMemoryUsage;
    size_t associativeMemoryCapacity;
    
    // Performance metrics
    double performanceScore;
    double responsivenessScore;
    double stabilityScore;
    double efficiencyScore;
    
    // Time metrics
    std::chrono::system_clock::time_point timestamp;
    std::chrono::milliseconds updateInterval;
    
    SystemMetrics() : cpuUsage(0.0), cpuCores(0), cpuLoadAverage(0.0),
                     totalMemory(0), usedMemory(0), freeMemory(0),
                     availableMemory(0), memoryUsagePercentage(0.0),
                     networkInputBytes(0), networkOutputBytes(0),
                     diskReadBytes(0), diskWriteBytes(0),
                     networkThroughput(0.0), diskIoThroughput(0.0),
                     activeThreads(0), totalThreads(0), pendingTasks(0),
                     completedTasks(0), tasksPerSecond(0.0),
                     averageTaskTime(std::chrono::milliseconds(0)),
                     totalNeurons(0), activeNeurons(0), firingNeurons(0),
                     totalSynapses(0), averageFiringRate(0.0),
                     excitationInhibitionRatio(0.0),
                     workingMemoryUsage(0), workingMemoryCapacity(0),
                     episodicMemoryUsage(0), episodicMemoryCapacity(0),
                     associativeMemoryUsage(0), associativeMemoryCapacity(0),
                     performanceScore(0.0), responsivenessScore(0.0),
                     stabilityScore(0.0), efficiencyScore(0.0) {
        timestamp = std::chrono::system_clock::now();
        updateInterval = std::chrono::milliseconds(100);
    }
    
    double getMemoryUsagePercentage() const {
        return totalMemory > 0 ? (static_cast<double>(usedMemory) / totalMemory) * 100.0 : 0.0;
    }
    
    double getCpuLoadPercentage() const {
        return cpuLoadAverage / static_cast<double>(cpuCores);
    }
    
    std::string getFormattedMemoryUsage() const;
    std::string getFormattedCpuUsage() const;
    std::string getFormattedMetrics() const;
};

// Alert Levels
enum class AlertLevel {
    Info = 0,
    Low = 1,
    Medium = 2,
    High = 3,
    Critical = 4,
    Fatal = 5
};

// Alert Types
enum class AlertType {
    Performance,      // Performance degradation
    ResourceExhaustion, // Resource exhaustion
    ComponentFailure,   // Component failure
    ConfigurationError, // Configuration errors
    NetworkError,      // Network errors
    MemoryError,       // Memory errors
    ThreadError,       // Thread errors
    HealthCheck,       // Health check failures
    Custom            // Custom alerts
};

// Alert Information
struct Alert {
    std::string id;
    AlertType type;
    AlertLevel level;
    std::string title;
    std::string message;
    std::string component;
    HealthStatus healthStatus;
    std::chrono::system_clock::time_point timestamp;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    bool isActive;
    bool isAcknowledged;
    bool isSilenced;
    std::string acknowledgedBy;
    std::string severity;
    std::unordered_map<std::string, std::string> context;
    std::vector<std::string> affectedComponents;
    std::vector<std::string> suggestedActions;
    
    Alert() : type(AlertType::Custom), level(AlertLevel::Info), 
             healthStatus(HealthStatus::Unknown), isActive(false),
             isAcknowledged(false), isSilenced(false) {
        timestamp = std::chrono::system_clock::now();
        startTime = timestamp;
        endTime = timestamp;
    }
    
    Alert(const std::string& title, const std::string& message,
          AlertType type = AlertType::Custom,
          AlertLevel level = AlertLevel::Medium)
        : title(title), message(message), type(type), level(level),
          healthStatus(HealthStatus::Unknown), isActive(true),
          isAcknowledged(false), isSilenced(false) {
        timestamp = std::chrono::system_clock::now();
        startTime = timestamp;
        endTime = timestamp;
    }
    
    std::string getFormattedMessage() const;
    std::string getFormattedTimestamp() const;
    std::string getFormattedDuration() const;
    bool isActiveAlert() const { return isActive && !isSilenced; }
    bool isResolved() const { return !isActive; }
    std::chrono::milliseconds getDuration() const {
        auto end = isActive ? std::chrono::system_clock::now() : endTime;
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - startTime);
    }
};

// Monitoring Configuration
struct MonitoringConfig {
    bool enableSystemMonitoring;
    bool enableComponentMonitoring;
    bool enablePerformanceMonitoring;
    bool enableAlerting;
    
    std::chrono::milliseconds systemMetricsInterval;
    std::chrono::milliseconds componentHealthInterval;
    std::chrono::milliseconds performanceMetricsInterval;
    
    double cpuUsageThreshold;
    double memoryUsageThreshold;
    double diskUsageThreshold;
    double networkUsageThreshold;
    
    int maxErrorHistory;
    int maxWarningHistory;
    int maxAlertHistory;
    
    bool enableEmailAlerts;
    bool enableSmsAlerts;
    bool enableSlackAlerts;
    bool enableWebhooks;
    
    std::string emailAddress;
    std::string phoneNumber;
    std::string slackWebhookUrl;
    std::string webhookUrl;
    
    MonitoringConfig() : enableSystemMonitoring(true),
                        enableComponentMonitoring(true),
                        enablePerformanceMonitoring(true),
                        enableAlerting(true),
                        systemMetricsInterval(std::chrono::milliseconds(1000)),
                        componentHealthInterval(std::chrono::milliseconds(5000)),
                        performanceMetricsInterval(std::chrono::milliseconds(5000)),
                        cpuUsageThreshold(80.0),
                        memoryUsageThreshold(85.0),
                        diskUsageThreshold(90.0),
                        networkUsageThreshold(100.0),
                        maxErrorHistory(1000),
                        maxWarningHistory(1000),
                        maxAlertHistory(1000),
                        enableEmailAlerts(false),
                        enableSmsAlerts(false),
                        enableSlackAlerts(false),
                        enableWebhooks(false) {}
};

// Monitoring System Interface
class MonitoringSystem {
public:
    virtual ~MonitoringSystem() = default;
    
    // System Metrics Collection
    virtual SystemMetrics collectSystemMetrics() = 0;
    virtual ComponentHealth collectComponentHealth(const std::string& componentId) = 0;
    
    // Component Registration
    virtual bool registerComponent(const std::string& componentId,
                                   const std::string& componentName,
                                   const std::unordered_map<std::string, std::string>& metadata = {}) = 0;
    virtual bool unregisterComponent(const std::string& componentId) = 0;
    virtual std::vector<std::string> getRegisteredComponents() const = 0;
    
    // Metrics Storage
    virtual void storeSystemMetrics(const SystemMetrics& metrics) = 0;
    virtual void storeComponentHealth(const ComponentHealth& health) = 0;
    
    // Metrics Retrieval
    virtual SystemMetrics getLatestSystemMetrics() const = 0;
    virtual ComponentHealth getLatestComponentHealth(const std::string& componentId) const = 0;
    virtual std::vector<SystemMetrics> getSystemMetricsHistory(size_t count = 100) const = 0;
    virtual std::vector<ComponentHealth> getComponentHealthHistory(const std::string& componentId, size_t count = 100) const = 0;
    
    // Alerting
    virtual bool createAlert(const Alert& alert) = 0;
    virtual bool acknowledgeAlert(const std::string& alertId, const std::string& acknowledgedBy = "") = 0;
    virtual bool silenceAlert(const std::string& alertId, int durationSeconds = 0) = 0;
    virtual bool resolveAlert(const std::string& alertId) = 0;
    
    virtual std::vector<Alert> getActiveAlerts() const = 0;
    virtual std::vector<Alert> getRecentAlerts(size_t count = 50) const = 0;
    virtual std::vector<Alert> getAlertsByComponent(const std::string& componentId) const = 0;
    
    // Configuration
    virtual void configure(const MonitoringConfig& config) = 0;
    virtual MonitoringConfig getConfiguration() const = 0;
    
    // Health Analysis
    virtual HealthStatus analyzeSystemHealth() const = 0;
    virtual std::unordered_map<std::string, HealthStatus> analyzeComponentHealth() const = 0;
    virtual double calculateOverallHealthScore() const = 0;
    
    // Metrics Analysis
    virtual std::string analyzePerformanceTrends() const = 0;
    virtual std::string analyzeResourceUsage() const = 0;
    virtual std::string analyzeErrorPatterns() const = 0;
    
    // Reporting
    virtual std::string generateSystemReport() const = 0;
    virtual std::string generateComponentReport(const std::string& componentId) const = 0;
    virtual std::string generateAlertReport() const = 0;
    
    // Status
    virtual bool isRunning() const = 0;
    virtual bool isHealthy() const = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    
    virtual size_t getSystemMetricsCount() const = 0;
    virtual size_t getComponentHealthCount() const = 0;
    virtual size_t getAlertCount() const = 0;
    virtual std::chrono::system_clock::time_point getLastUpdateTime() const = 0;
};

// Default Monitoring System
class DefaultMonitoringSystem : public MonitoringSystem {
private:
    MonitoringConfig config;
    std::thread monitoringThread;
    std::atomic<bool> running;
    std::atomic<bool> paused;
    
    mutable std::mutex metricsMutex;
    mutable std::mutex alertsMutex;
    mutable std::mutex componentsMutex;
    
    std::deque<SystemMetrics> systemMetricsHistory;
    std::deque<Alert> alertHistory;
    std::unordered_map<std::string, ComponentHealth> componentHealthHistory;
    std::unordered_map<std::string, std::deque<ComponentHealth>> componentHealthHistories;
    
    std::unordered_map<std::string, std::chrono::system_clock::time_point> componentLastCheck;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> componentMetadata;
    
    std::condition_variable cv;
    std::mutex cvMutex;
    
    std::function<void(const Alert&)> alertCallback;
    std::function<void(const SystemMetrics&)> metricsCallback;
    std::function<void(const ComponentHealth&)> healthCallback;
    
    // Internal monitoring methods
    void monitorSystemMetrics();
    void monitorComponentHealth();
    void monitorPerformanceMetrics();
    
    void checkThresholds();
    void evaluateComponentHealth(const std::string& componentId);
    void checkSystemHealth();
    
    SystemMetrics collectCpuMetrics();
    SystemMetrics collectMemoryMetrics();
    SystemMetrics collectNetworkMetrics();
    SystemMetrics collectProcessMetrics();
    SystemMetrics collectNeuralMetrics();
    SystemMetrics collectMemorySystemMetrics();
    
    ComponentHealth evaluateComponentHealthScore(const std::string& componentId,
                                                const ComponentHealth& health);
    
    AlertLevel determineAlertLevel(const std::string& metric, double value, double threshold) const;
    Alert createAlertFromCondition(const std::string& condition,
                                   const std::string& message,
                                   AlertType type = AlertType::Custom,
                                   AlertLevel level = AlertLevel::Medium,
                                   const std::string& component = "System");
    
    bool shouldCreateAlert(const std::string& metric, double value, double threshold) const;
    
    std::string calculateTrend(const std::deque<double>& values) const;
    double calculateAverage(const std::deque<double>& values) const;
    double calculateStandardDeviation(const std::deque<double>& values, double mean) const;
    
    SystemMetrics aggregateSystemMetrics(const std::vector<SystemMetrics>& metrics) const;
    ComponentHealth aggregateComponentHealth(const std::vector<ComponentHealth>& healths) const;
    
public:
    DefaultMonitoringSystem() : running(false), paused(false) {}
    ~DefaultMonitoringSystem() {
        stop();
    }
    
    // System Metrics Collection
    SystemMetrics collectSystemMetrics() override;
    ComponentHealth collectComponentHealth(const std::string& componentId) override;
    
    // Component Registration
    bool registerComponent(const std::string& componentId,
                           const std::string& componentName,
                           const std::unordered_map<std::string, std::string>& metadata = {}) override;
    bool unregisterComponent(const std::string& componentId) override;
    std::vector<std::string> getRegisteredComponents() const override;
    
    // Metrics Storage
    void storeSystemMetrics(const SystemMetrics& metrics) override;
    void storeComponentHealth(const ComponentHealth& health) override;
    
    // Metrics Retrieval
    SystemMetrics getLatestSystemMetrics() const override;
    ComponentHealth getLatestComponentHealth(const std::string& componentId) const override;
    std::vector<SystemMetrics> getSystemMetricsHistory(size_t count = 100) const override;
    std::vector<ComponentHealth> getComponentHealthHistory(const std::string& componentId, size_t count = 100) const override;
    
    // Alerting
    bool createAlert(const Alert& alert) override;
    bool acknowledgeAlert(const std::string& alertId, const std::string& acknowledgedBy = "") override;
    bool silenceAlert(const std::string& alertId, int durationSeconds = 0) override;
    bool resolveAlert(const std::string& alertId) override;
    
    std::vector<Alert> getActiveAlerts() const override;
    std::vector<Alert> getRecentAlerts(size_t count = 50) const override;
    std::vector<Alert> getAlertsByComponent(const std::string& componentId) const override;
    
    // Configuration
    void configure(const MonitoringConfig& config) override;
    MonitoringConfig getConfiguration() const override;
    
    // Health Analysis
    HealthStatus analyzeSystemHealth() const override;
    std::unordered_map<std::string, HealthStatus> analyzeComponentHealth() const override;
    double calculateOverallHealthScore() const override;
    
    // Metrics Analysis
    std::string analyzePerformanceTrends() const override;
    std::string analyzeResourceUsage() const override;
    std::string analyzeErrorPatterns() const override;
    
    // Reporting
    std::string generateSystemReport() const override;
    std::string generateComponentReport(const std::string& componentId) const override;
    std::string generateAlertReport() const override;
    
    // Status
    bool isRunning() const override { return running; }
    bool isHealthy() const override;
    void start() override;
    void stop() override;
    void pause() override;
    void resume() override;
    
    size_t getSystemMetricsCount() const override;
    size_t getComponentHealthCount() const override;
    size_t getAlertCount() const override;
    std::chrono::system_clock::time_point getLastUpdateTime() const override;
    
    // Callback registration
    void setAlertCallback(const std::function<void(const Alert&)>& callback) {
        alertCallback = callback;
    }
    
    void setMetricsCallback(const std::function<void(const SystemMetrics&)>& callback) {
        metricsCallback = callback;
    }
    
    void setHealthCallback(const std::function<void(const ComponentHealth&)>& callback) {
        healthCallback = callback;
    }
    
    // Manual metric collection
    void forceSystemMetricsCollection();
    void forceComponentHealthCollection(const std::string& componentId);
    void forcePerformanceMetricsCollection();
};

// Global Monitoring System Instance
inline MonitoringSystem& getDefaultMonitoringSystem() {
    static DefaultMonitoringSystem system;
    return system;
}

// Convenience Functions
inline void startMonitoring() {
    getDefaultMonitoringSystem().start();
}

inline void stopMonitoring() {
    getDefaultMonitoringSystem().stop();
}

inline SystemMetrics collectSystemMetrics() {
    return getDefaultMonitoringSystem().collectSystemMetrics();
}

inline ComponentHealth collectComponentHealth(const std::string& componentId) {
    return getDefaultMonitoringSystem().collectComponentHealth(componentId);
}

inline bool registerComponent(const std::string& componentId,
                             const std::string& componentName,
                             const std::unordered_map<std::string, std::string>& metadata = {}) {
    return getDefaultMonitoringSystem().registerComponent(componentId, componentName, metadata);
}

inline std::vector<Alert> getActiveAlerts() {
    return getDefaultMonitoringSystem().getActiveAlerts();
}

inline std::string generateSystemReport() {
    return getDefaultMonitoringSystem().generateSystemReport();
}

inline HealthStatus getSystemHealthStatus() {
    return getDefaultMonitoringSystem().analyzeSystemHealth();
}

} // namespace nlm
