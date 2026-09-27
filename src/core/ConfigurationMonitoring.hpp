#pragma once

/**
 * Configuration Monitoring
 * 
 * Configuration monitoring system for tracking configuration usage, performance,
 * and monitoring configuration-related metrics for the NLM brain simulation project.
 * 
 * Features:
 * - Configuration usage monitoring
 * - Configuration performance monitoring
 * * Configuration memory monitoring
 * - Configuration access pattern tracking
 * - Configuration change monitoring
 * - Configuration health monitoring
 * - Configuration anomaly detection
 * - Configuration performance metrics
 * - Configuration monitoring alerts
 * - Configuration monitoring dashboards
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <numeric>

namespace nlm {
namespace core {

class ConfigurationManager;

/**
 * Monitoring metrics
 */
struct MonitoringMetrics {
    double timestamp;
    size_t configurationAccessCount;
    double averageAccessTime;
    size_t configurationChangeCount;
    double averageChangeTime;
    size_t configurationValidationCount;
    double averageValidationTime;
    size_t configurationLoadCount;
    double averageLoadTime;
    size_t memoryUsage;
    double cpuUsage;
    
    MonitoringMetrics() : timestamp(0.0), configurationAccessCount(0), averageAccessTime(0.0),
                         configurationChangeCount(0), averageChangeTime(0.0),
                         configurationValidationCount(0), averageValidationTime(0.0),
                         configurationLoadCount(0), averageLoadTime(0.0),
                         memoryUsage(0), cpuUsage(0.0) {}
};

/**
 * Configuration access pattern
 */
struct ConfigAccessPattern {
    std::string key;
    std::string component;
    double lastAccessTime;
    size_t accessCount;
    double averageAccessTime;
    
    ConfigAccessPattern(const std::string& k, const std::string& comp)
        : key(k), component(comp), lastAccessTime(0.0), accessCount(0), averageAccessTime(0.0) {}
};

/**
 * Configuration anomaly
 */
struct ConfigAnomaly {
    double timestamp;
    std::string type;
    std::string description;
    std::string severity;
    std::map<std::string, std::string> metrics;
    std::string suggestedAction;
    
    ConfigAnomaly(const std::string& t, const std::string& desc, const std::string& sev)
        : timestamp(0.0), type(t), description(desc), severity(sev) {}
};

/**
 * Configuration monitor
 */
class ConfigurationMonitor {
public:
    ConfigurationMonitor();
    ~ConfigurationMonitor();
    
    // Disable copying, enable moving
    ConfigurationMonitor(const ConfigurationMonitor&) = delete;
    ConfigurationMonitor& operator=(const ConfigurationMonitor&) = delete;
    ConfigurationMonitor(ConfigurationMonitor&&) noexcept = default;
    ConfigurationMonitor& operator=(ConfigurationMonitor&&) noexcept = default;
    
    // Monitoring configuration
    void enableMonitoring(bool enable) { enabled_ = enable; }
    bool isMonitoringEnabled() const { return enabled_; }
    void setMonitoringInterval(double interval) { monitoring_interval_ = interval; }
    double getMonitoringInterval() const { return monitoring_interval_; }
    
    // Metrics collection
    void recordAccess(const std::string& key, const std::string& component, double accessTime);
    void recordChange(const std::string& key, double changeTime);
    void recordValidation(const std::string& component, double validationTime);
    void recordLoad(const std::string& filePath, bool success, double loadTime);
    
    // Pattern tracking
    void trackAccessPattern(const std::string& key, const std::string& component, double accessTime);
    std::vector<ConfigAccessPattern> getAccessPatterns(const std::string& component = "") const;
    
    // Anomaly detection
    bool detectAnomalies();
    std::vector<ConfigAnomaly> getAnomalies() const { return anomalies_; }
    bool clearAnomalies();
    
    // Alert system
    using AlertCallback = std::function<void(const ConfigAnomaly& anomaly)>;
    void addAlertCallback(const AlertCallback& callback);
    void removeAlertCallback(const AlertCallback& callback);
    void triggerAlert(const ConfigAnomaly& anomaly);
    
    // Metrics retrieval
    MonitoringMetrics getCurrentMetrics() const { return current_metrics_; }
    MonitoringMetrics getAggregatedMetrics(double timeWindow) const;
    std::vector<MonitoringMetrics> getMetricsHistory(size_t limit = 100) const;
    
    // Performance analysis
    double getPerformanceScore() const;
    std::vector<std::string> getPerformanceRecommendations() const;
    std::string getPerformanceReport() const;
    
    // Configuration-specific monitoring
    void monitorConfigurationUsage(const ConfigurationManager& configManager);
    void monitorConfigurationHealth(const ConfigurationManager& configManager);
    bool isConfigurationHealthy() const;
    
    // Memory monitoring
    void updateMemoryUsage();
    size_t getMemoryUsage() const { return memory_usage_; }
    bool isMemoryUsageExceeded() const;
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Debug and diagnostics
    std::string dump() const;
    std::string getStatisticsString() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    bool enabled_ = true;
    double monitoring_interval_ = 1.0; // seconds
    
    MonitoringMetrics current_metrics_;
    std::vector<MonitoringMetrics> metrics_history_;
    std::vector<ConfigAccessPattern> access_patterns_;
    std::vector<ConfigAnomaly> anomalies_;
    std::vector<AlertCallback> alert_callbacks_;
    
    size_t memory_usage_ = 0;
    double last_memory_check_ = 0.0;
    
    // Internal helpers
    void updateMetrics();
    void detectMemoryAnomalies();
    void detectAccessPatternAnomalies();
    void detectPerformanceAnomalies();
    double calculateAnomalyScore(const ConfigAnomaly& anomaly) const;
    
    friend class ConfigurationManager;
};

} // namespace core
} // namespace nlm
