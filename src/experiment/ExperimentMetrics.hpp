#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <thread>
#include <mutex>
#include "ExperimentProtocol.hpp"
#include "ExperimentResults.hpp"

namespace nlm {

// Experiment metrics configuration
struct ExperimentMetricsConfig {
    std::string experimentName;
    bool enableRealTimeMetrics;
    std::chrono::milliseconds metricsCollectionInterval;
    std::vector<std::string> enabledMetrics;
    std::map<std::string, double> metricThresholds;
    std::map<std::string, std::string> metricUnits;
    bool enableMetricValidation;
    bool enableMetricAlerting;
    bool enableMetricOptimization;
    std::map<std::string, std::function<double(const ExperimentResults&)>> customMetricCalculators;
    
    ExperimentMetricsConfig() 
        : enableRealTimeMetrics(true), metricsCollectionInterval(std::chrono::milliseconds(100)),
          enableMetricValidation(true), enableMetricAlerting(true), enableMetricOptimization(false) {}
};

// Metrics collector callback
using MetricsCollectorCallback = std::function<void(const std::string&, const std::map<std::string, double>&)>
;

// Metrics alert callback
using MetricsAlertCallback = std::function<void(const std::string&, const std::string&, double, double)>
;

class ExperimentMetrics {
public:
    ExperimentMetrics();
    ~ExperimentMetrics();
    
    // Disable copying, enable moving
    ExperimentMetrics(const ExperimentMetrics&) = delete;
    ExperimentMetrics& operator=(const ExperimentMetrics&) = delete;
    ExperimentMetrics(ExperimentMetrics&&) noexcept;
    ExperimentMetrics& operator=(ExperimentMetrics&&) noexcept;
    
    // Metrics management
    bool startMetricsCollection(const std::string& experimentName);
    bool stopMetricsCollection(const std::string& experimentName);
    bool pauseMetricsCollection(const std::string& experimentName);
    bool resumeMetricsCollection(const std::string& experimentName);
    
    // Metrics collection
    bool collectMetrics(const std::string& experimentName, const std::map<std::string, double>& metrics);
    bool updateMetric(const std::string& experimentName, const std::string& metricName, double value);
    bool calculateCustomMetric(const std::string& experimentName, const std::string& metricName,
                              std::function<double(const ExperimentResults&)> calculator);
    
    // Metrics querying
    double getMetric(const std::string& experimentName, const std::string& metricName) const;
    std::map<std::string, double> getAllMetrics(const std::string& experimentName) const;
    std::chrono::system_clock::time_point getMetricTimestamp(const std::string& experimentName, 
                                                            const std::string& metricName) const;
    bool hasMetric(const std::string& experimentName, const std::string& metricName) const;
    
    // Metrics history
    std::vector<std::map<std::string, double>> getMetricsHistory(const std::string& experimentName, 
                                                                 size_t limit = 100) const;
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> getMetricHistory(const std::string& experimentName, 
                                                                                       const std::string& metricName,
                                                                                       size_t limit = 100) const;
    
    // Metrics analysis
    double calculateMetricMean(const std::string& experimentName, const std::string& metricName) const;
    double calculateMetricStandardDeviation(const std::string& experimentName, const std::string& metricName) const;
    double calculateMetricVariance(const std::string& experimentName, const std::string& metricName) const;
    double calculateMetricMedian(const std::string& experimentName, const std::string& metricName) const;
    double calculateMetricPercentile(const std::string& experimentName, const std::string& metricName,
                                    double percentile) const;
    double calculateMetricTrend(const std::string& experimentName, const std::string& metricName) const;
    std::string analyzeMetricDistribution(const std::string& experimentName, const std::string& metricName) const;
    
    // Metrics comparison
    double compareMetrics(const std::string& experimentName1, const std::string& experimentName2,
                         const std::string& metricName) const;
    std::map<std::string, double> compareAllMetrics(const std::string& experimentName1,
                                                   const std::string& experimentName2) const;
    std::vector<std::string> getCommonMetrics(const std::string& experimentName1,
                                              const std::string& experimentName2) const;
    
    // Metrics optimization
    bool optimizeMetricCollection(const std::string& experimentName);
    bool enableMetricsSampling(const std::string& experimentName, size_t sampleSize);
    bool setMetricCalculationInterval(const std::string& experimentName,
                                     const std::chrono::milliseconds& interval);
    bool enableMetricCaching(const std::string& experimentName, bool enable);
    bool isMetricCached(const std::string& experimentName, const std::string& metricName) const;
    
    // Metrics validation
    bool validateMetric(const std::string& experimentName, const std::string& metricName,
                       double value) const;
    bool validateAllMetrics(const std::string& experimentName) const;
    bool enableMetricValidation(const std::string& experimentName, bool enable);
    bool isMetricValidationEnabled(const std::string& experimentName) const;
    
    // Metrics alerting
    bool addMetricAlertCallback(MetricsAlertCallback callback);
    bool removeMetricAlertCallback(MetricsAlertCallback callback);
    bool addMetricsCollectorCallback(MetricsCollectorCallback callback);
    bool removeMetricsCollectorCallback(MetricsCollectorCallback callback);
    bool clearCallbacks();
    
    bool triggerMetricAlert(const std::string& experimentName, const std::string& metricName,
                           double currentValue, double thresholdValue);
    bool setMetricThreshold(const std::string& experimentName, const std::string& metricName,
                          double threshold);
    double getMetricThreshold(const std::string& experimentName, const std::string& metricName) const;
    
    // Metrics export
    bool exportMetrics(const std::string& experimentName, const std::string& filepath,
                      const std::string& format = "csv") const;
    bool exportMetricsToDatabase(const std::string& experimentName, 
                                const std::string& connectionString,
                                const std::string& tableName) const;
    
    // Metrics import
    bool importMetrics(const std::string& experimentName, const std::string& filepath,
                      const std::string& format = "csv");
    bool importMetricsFromDatabase(const std::string& experimentName,
                                   const std::string& connectionString,
                                   const std::string& tableName);
    
    // Metrics configuration
    void setMetricsConfig(const std::string& experimentName, const ExperimentMetricsConfig& config);
    ExperimentMetricsConfig getMetricsConfig(const std::string& experimentName) const;
    void addEnabledMetric(const std::string& experimentName, const std::string& metricName);
    void removeEnabledMetric(const std::string& experimentName, const std::string& metricName);
    std::vector<std::string> getEnabledMetrics(const std::string& experimentName) const;
    
    // Metrics monitoring
    bool enableRealTimeMonitoring(const std::string& experimentName, bool enable);
    bool isRealTimeMonitoringEnabled(const std::string& experimentName) const;
    std::chrono::system_clock::time_point getLastMetricsUpdate(const std::string& experimentName) const;
    std::chrono::milliseconds getMetricsCollectionInterval(const std::string& experimentName) const;
    
    // Metrics cleanup
    bool cleanupMetrics(const std::string& experimentName);
    bool cleanupOldMetrics(const std::string& experimentName, 
                          const std::chrono::system_clock::time_point& cutoffTime);
    bool clearAllMetrics(const std::string& experimentName);
    std::vector<std::string> getMonitoredExperiments() const;
    size_t getMonitoredExperimentCount() const;
    
    // Metrics statistics
    size_t getMetricCount(const std::string& experimentName) const;
    std::chrono::system_clock::time_point getMetricsStartTime(const std::string& experimentName) const;
    std::chrono::seconds getMetricsDuration(const std::string& experimentName) const;
    std::map<std::string, double> getResourceUtilization(const std::string& experimentName) const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm