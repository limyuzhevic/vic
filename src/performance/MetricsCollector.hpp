#pragma once

/**
 * MetricsCollector.hpp - System-wide performance metrics collection and monitoring
 * 
 * Provides comprehensive performance metrics collection:
 * 
 * Features:
 * - System-wide performance metrics collection
 * - Real-time metrics monitoring
 * - Historical metrics storage and analysis
 * - Cross-component metrics correlation
 * - Performance alert and threshold management
 * - Metrics visualization and reporting
 * - Automated metrics collection
 * - Metrics export and integration
 * - Performance trend analysis
 * - Anomaly detection
 * - Custom metrics collection
 * 
 * Use cases:
 * - System performance monitoring
 * - Component performance analysis
 * - Performance trend identification
 * - Anomaly detection and alerting
 * - Performance capacity planning
 * - Performance reporting and documentation
 * 
 */

#ifndef NLM_METRICS_COLLECTOR_HPP
#define NLM_METRICS_COLLECTOR_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <chrono>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <optional>
#include <exception>
#include <thread>
#include <future>
#include <queue>
#include <set>
#include <cmath>
#include "Performance.hpp"

namespace nlm {

/**
 * System component identifiers
 */
enum class SystemComponent {
    Unknown,
    MemoryPool,
    ThreadPool,
    ParallelFor,
    NeuralProcessor,
    EventQueue,
    CheckpointSystem,
    SIMDVectorization,
    SparseConnectivity,
    OptimizedBrain,
    ConfigSystem,
    FileIO,
    Network,
    Database,
    Cache,
    Scheduler,
    GarbageCollector,
    Synchronization
};

/**
 * Metrics collection frequency
 */
enum class CollectionFrequency {
    High,      // Every few milliseconds
    Medium,    // Every few seconds
    Low,       // Every few minutes
    Custom    // Custom interval
};

/**
 * Metrics collection configuration
 */
struct MetricsConfig {
    std::string collectorName;              // Collector name
    CollectionFrequency collectionFrequency; // Collection frequency
    std::chrono::duration<double> collectionInterval; // Collection interval
    size_t historySize;                      // Size of history to keep (samples)
    bool enableRealTimeReporting;             // Enable real-time reporting
    bool enableHistoricalAnalysis;            // Enable historical analysis
    double collectionThreshold;              // Collection threshold (0.0-1.0)
    std::string outputFormat;                // Output format: "json", "csv", "prometheus", "custom"
    std::string outputFile;                  // Output file path
    bool enableMetricsExport;                // Enable metrics export
    bool enableAlerting;                      // Enable alerting
    double alertThreshold;                    // Alert threshold (0.0-1.0)
    std::function<bool(double)> isAlertThreshold; // Alert threshold function
    bool enableAnomalyDetection;              // Enable anomaly detection
    double anomalyDetectionThreshold;         // Anomaly detection threshold
    bool enablePerformanceBaseline;            // Enable performance baseline
    std::string baselineIdentifier;           // Performance baseline identifier
    bool enableCorrelationAnalysis;            // Enable correlation analysis
    
    MetricsConfig()
        : collectionFrequency(CollectionFrequency::Medium)
        , collectionInterval(std::chrono::seconds(5))
        , historySize(10000)
        , enableRealTimeReporting(true)
        , enableHistoricalAnalysis(true)
        , collectionThreshold(0.5)
        , outputFormat("json")
        , outputFile("")
        , enableMetricsExport(false)
        , enableAlerting(true)
        , alertThreshold(0.8)
        , enableAnomalyDetection(true)
        , anomalyDetectionThreshold(2.0)
        , enablePerformanceBaseline(false)
        , enableCorrelationAnalysis(true) {
        isAlertThreshold = [](double value) { return value > 0.8; };
    }
};

/**
 * Performance metrics point
 */
struct MetricsDataPoint {
    std::chrono::high_resolution_clock::time_point timestamp;
    SystemComponent component;               // Component identifier
    std::string componentName;                // Component name
    std::string metricName;                   // Metric name
    std::string metricDescription;            // Metric description
    double value;                            // Metric value
    BenchmarkUnit unit;                       // Metric unit
    
    // Additional metadata
    size_t threadId{0};                       // Thread ID
    uint64_t threadAffinity{0};                // Thread affinity
    std::string context;                      // Context/labels
    double confidenceInterval{0.0};           // Confidence interval
    bool isValid{true};                       // Data validity flag
    
    // Statistical fields
    double standardDeviation{0.0};             // Standard deviation
    double variance{0.0};                      // Variance
    double skewness{0.0};                      // Skewness
    double kurtosis{0.0};                      // Kurtosis
    
    // Performance metrics
    double rate{0.0};                         // Rate per second
    double efficiency{0.0};                   // Efficiency (0.0-1.0)
    double utilization{0.0};                   // Utilization (0.0-1.0)
    
    // Quality metrics
    bool isReliable{true};                     // Data reliability
    double qualityScore{1.0};                  // Quality score (0.0-1.0)
    
    MetricsDataPoint() = default;
    
    MetricsDataPoint(SystemComponent comp, const std::string& name,
                    const std::string& desc, double val, BenchmarkUnit u)
        : component(comp)
        , componentName(GetComponentName(comp))
        , metricName(name)
        , metricDescription(desc)
        , value(val)
        , unit(u)
        , timestamp(std::chrono::high_resolution_clock::now()) {}
    
    std::string getKey() const {
        return componentName + "." + metricName;
    }
    
    std::string getFullName() const {
        return componentName + "." + metricName + " (" + GetUnitString(unit) + ")";
    }
    
    static std::string GetComponentName(SystemComponent comp) {
        switch (comp) {
            case SystemComponent::MemoryPool: return "MemoryPool";
            case SystemComponent::ThreadPool: return "ThreadPool";
            case SystemComponent::ParallelFor: return "ParallelFor";
            case SystemComponent::NeuralProcessor: return "NeuralProcessor";
            case SystemComponent::EventQueue: return "EventQueue";
            case SystemComponent::CheckpointSystem: return "CheckpointSystem";
            case SystemComponent::SIMDVectorization: return "SIMDVectorization";
            case SystemComponent::SparseConnectivity: return "SparseConnectivity";
            case SystemComponent::OptimizedBrain: return "OptimizedBrain";
            case SystemComponent::ConfigSystem: return "ConfigSystem";
            case SystemComponent::FileIO: return "FileIO";
            case SystemComponent::Network: return "Network";
            case SystemComponent::Database: return "Database";
            case SystemComponent::Cache: return "Cache";
            case SystemComponent::Scheduler: return "Scheduler";
            case SystemComponent::GarbageCollector: return "GarbageCollector";
            case SystemComponent::Synchronization: return "Synchronization";
            default: return "Unknown";
        }
    }
    
    static std::string GetUnitString(BenchmarkUnit unit) {
        switch (unit) {
            case BenchmarkUnit::OperationsPerSecond: return "ops/sec";
            case BenchmarkUnit::Nanoseconds: return "ns";
            case BenchmarkUnit::Bytes: return "bytes";
            case BenchmarkUnit::Percent: return "%";
            case BenchmarkUnit::MegaBytesPerSecond: return "MB/s";
            case BenchmarkUnit::GigaOperationsPerSecond: return "GOps";
            case BenchmarkUnit::MicroSeconds: return "µs";
            case BenchmarkUnit::Count: return "count";
            default: return "";
        }
    }
};

/**
 * Metrics aggregation
 */
struct MetricsAggregation {
    std::chrono::high_resolution_clock::time_point startTime;  // Aggregation start time
    std::chrono::high_resolution_clock::time_point endTime;    // Aggregation end time
    
    // Aggregated values
    double minValue{0.0};                       // Minimum value
    double maxValue{0.0};                       // Maximum value
    double mean{0.0};                           // Mean value
    double median{0.0};                         // Median value
    double standardDeviation{0.0};              // Standard deviation
    double variance{0.0};                        // Variance
    double skewness{0.0};                        // Skewness
    double kurtosis{0.0};                        // Kurtosis
    
    // Performance metrics
    double averageRate{0.0};                    // Average rate per second
    double averageEfficiency{0.0};              // Average efficiency
    double averageUtilization{0.0};              // Average utilization
    
    // Sample information
    size_t sampleCount{0};                       // Number of samples
    size_t validSamples{0};                      // Number of valid samples
    
    // Time information
    double durationSeconds{0.0};                // Duration in seconds
    
    MetricsAggregation() = default;
    
    void reset() {
        startTime = std::chrono::high_resolution_clock::now();
        minValue = maxValue = mean = median = standardDeviation = variance = 0.0;
        skewness = kurtosis = averageRate = averageEfficiency = averageUtilization = 0.0;
        sampleCount = validSamples = 0;
        durationSeconds = 0.0;
    }
    
    void update(const MetricsDataPoint& dataPoint) {
        if (!dataPoint.isValid) return;
        
        validSamples++;
        
        if (sampleCount == 0) {
            minValue = maxValue = dataPoint.value;
        } else {
            minValue = std::min(minValue, dataPoint.value);
            maxValue = std::max(maxValue, dataPoint.value);
        }
        
        // Update averages (running mean)
        mean = ((mean * sampleCount) + dataPoint.value) / (sampleCount + 1);
        
        // Update other metrics
        if (dataPoint.rate > 0.0) {
            averageRate = ((averageRate * sampleCount) + dataPoint.rate) / (sampleCount + 1);
        }
        
        if (dataPoint.efficiency > 0.0) {
            averageEfficiency = ((averageEfficiency * sampleCount) + dataPoint.efficiency) / (sampleCount + 1);
        }
        
        if (dataPoint.utilization > 0.0) {
            averageUtilization = ((averageUtilization * sampleCount) + dataPoint.utilization) / (sampleCount + 1);
        }
        
        sampleCount++;
        
        // Update end time
        endTime = std::chrono::high_resolution_clock::now();
        durationSeconds = std::chrono::duration<double>(endTime - startTime).count();
    }
    
    double getCoefficientOfVariation() const {
        return mean > 0.0 ? (standardDeviation / mean) * 100.0 : 0.0;
    }
    
    double getQualityScore() const {
        double cv = getCoefficientOfVariation();
        double quality = 100.0;
        quality *= std::exp(-cv / 10.0);  // Penalize high CV
        quality *= (validSamples / static_cast<double>(sampleCount + 1));  // Penalize invalid samples
        return std::max(0.0, quality);
    }
};

/**
 * Metrics alert
 */
struct MetricsAlert {
    std::chrono::high_resolution_clock::time_point timestamp;  // Alert timestamp
    SystemComponent component;                // Component
    std::string metricName;                   // Metric name
    double currentValue{0.0};                 // Current value
    double thresholdValue{0.0};               // Threshold value
    std::string severity;                     // Severity: "info", "warning", "error", "critical"
    std::string message;                       // Alert message
    std::string description;                  // Alert description
    bool isActive{true};                       // Alert active status
    
    MetricsAlert() = default;
    
    MetricsAlert(SystemComponent comp, const std::string& metric, double current,
                double threshold, const std::string& sev, const std::string& msg,
                const std::string& desc)
        : timestamp(std::chrono::high_resolution_clock::now())
        , component(comp)
        , metricName(metric)
        , currentValue(current)
        , thresholdValue(threshold)
        , severity(sev)
        , message(msg)
        , description(desc)
        , isActive(true) {}
    
    bool isTriggered() const {
        return isActive && currentValue > thresholdValue;
    }
    
    std::string getSeverityLevel() const {
        if (severity == "critical" || severity == "error") return "HIGH";
        if (severity == "warning") return "MEDIUM";
        return "LOW";
    }
};

/**
 * Metrics anomaly detection
 */
struct AnomalyDetection {
    double threshold{2.0};                    // Anomaly detection threshold (standard deviations)
    std::string method;                       // Detection method: "zscore", "iqr", "isolationforest"
    bool enableTemporalDetection;             // Enable temporal anomaly detection
    bool enableCorrelationDetection;          // Enable correlation-based detection
    
    AnomalyDetection()
        : threshold(2.0)
        , method("zscore")
        , enableTemporalDetection(false)
        , enableCorrelationDetection(false) {}
    
    bool isAnomaly(double value, double mean, double stddev) const {
        if (stddev == 0.0) return false;
        return std::abs(value - mean) / stddev > threshold;
    }
    
    bool isAnomaly(const std::vector<double>& values) const {
        if (values.empty()) return false;
        
        double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        double variance = 0.0;
        for (double val : values) {
            variance += (val - mean) * (val - mean);
        }
        variance /= values.size();
        double stddev = std::sqrt(variance);
        
        if (stddev == 0.0) return false;
        
        size_t anomalyCount = 0;
        for (double val : values) {
            if (std::abs(val - mean) / stddev > threshold) {
                anomalyCount++;
            }
        }
        
        return (double)anomalyCount / values.size() > 0.1;  // 10% threshold
    }
};

/**
 * System-wide metrics collector
 */
class MetricsCollector {
public:
    /**
     * Metrics collector configuration
     */
    struct CollectorConfig {
        std::string collectorId;                 // Collector ID
        MetricsConfig metricsConfig;             // Metrics configuration
        bool enableAutoCollection;                // Enable automatic collection
        std::chrono::duration<double> autoCollectionInterval; // Auto collection interval
        size_t maxDataPointsPerMetric;            // Maximum data points per metric
        bool enableMetricsCorrelation;            // Enable metrics correlation
        bool enableSystemWideMetrics;             // Enable system-wide metrics
        std::vector<SystemComponent> trackedComponents; // Tracked components
        std::unordered_map<std::string, std::function<double()>> customMetrics; // Custom metrics
        bool enableHealthMonitoring;              // Enable health monitoring
        double healthThreshold;                   // Health threshold
        
        CollectorConfig()
            : metricsConfig()
            , enableAutoCollection(true)
            , autoCollectionInterval(std::chrono::seconds(1))
            , maxDataPointsPerMetric(10000)
            , enableMetricsCorrelation(true)
            , enableSystemWideMetrics(true)
            , enableHealthMonitoring(true)
            , healthThreshold(0.5) {}
    };
    
    MetricsCollector(const CollectorConfig& config = CollectorConfig());
    ~MetricsCollector();
    
    // Disable copying
    MetricsCollector(const MetricsCollector&) = delete;
    MetricsCollector& operator=(const MetricsCollector&) = delete;
    
    // Enable moving
    MetricsCollector(MetricsCollector&& other) noexcept;
    MetricsCollector& operator=(MetricsCollector&& other) noexcept;
    
    /**
     * Start metrics collection
     */
    void startCollection();
    
    /**
     * Stop metrics collection
     */
    void stopCollection();
    
    /**
     * Is collection active?
     */
    bool isCollecting() const { return isCollecting_; }
    
    /**
     * Collect a metrics data point
     */
    void collectData(const MetricsDataPoint& dataPoint);
    
    /**
     * Collect metrics from performance monitor
     */
    void collectFromPerformanceMonitor(const PerformanceMonitor& monitor);
    
    /**
     * Collect metrics from memory pool
     */
    template<typename T>
    void collectFromMemoryPool(const MemoryPool<T>& pool, const std::string& context = "");
    
    /**
     * Collect metrics from thread pool
     */
    void collectFromThreadPool(const ThreadPool& pool, const std::string& context = "");
    
    /**
     * Collect metrics from parallel processing
     */
    void collectFromParallelProcessing(const ParallelFor& parallelFor, const std::string& context = "");
    
    /**
     * Collect system-wide metrics
     */
    void collectSystemMetrics();
    
    /**
     * Collect custom metrics
     */
    void collectCustomMetrics(const std::unordered_map<std::string, double>& metrics, 
                             const std::string& context = "");
    
    /**
     * Get aggregated metrics for a specific component and metric
     */
    MetricsAggregation getAggregatedMetrics(SystemComponent component,
                                           const std::string& metricName,
                                           std::chrono::duration<double> timeWindow = std::chrono::seconds(60)) const;
    
    /**
     * Get metrics data points for a specific component and metric
     */
    std::vector<MetricsDataPoint> getMetricsDataPoints(SystemComponent component,
                                                       const std::string& metricName,
                                                       std::chrono::duration<double> timeWindow = std::chrono::seconds(60)) const;
    
    /**
     * Get all metrics data points
     */
    std::vector<MetricsDataPoint> getAllMetricsDataPoints(std::chrono::duration<double> timeWindow = std::chrono::seconds(60)) const;
    
    /**
     * Get metrics for a specific time range
     */
    std::vector<MetricsDataPoint> getMetricsInTimeRange(std::chrono::high_resolution_clock::time_point startTime,
                                                        std::chrono::high_resolution_clock::time_point endTime) const;
    
    /**
     * Get active alerts
     */
    std::vector<MetricsAlert> getActiveAlerts() const;
    
    /**
     * Get alert summary
     */
    struct AlertSummary {
        size_t totalAlerts{0};
        size_t activeAlerts{0};
        std::string highestSeverity;              // Highest severity: "LOW", "MEDIUM", "HIGH"
        std::chrono::high_resolution_clock::time_point lastAlertTime;
    };
    
    AlertSummary getAlertSummary() const;
    
    /**
     * Check for anomalies in metrics
     */
    std::vector<MetricsDataPoint> detectAnomalies(SystemComponent component,
                                                 const std::string& metricName,
                                                 std::chrono::duration<double> timeWindow = std::chrono::seconds(60)) const;
    
    /**
     * Get metrics correlation
     */
    std::map<std::string, double> getMetricsCorrelation(SystemComponent component1,
                                                         const std::string& metricName1,
                                                         SystemComponent component2,
                                                         const std::string& metricName2,
                                                         std::chrono::duration<double> timeWindow = std::chrono::seconds(60)) const;
    
    /**
     * Get health score
     */
    double getHealthScore() const { return healthScore_.load(std::memory_order_relaxed); }
    
    /**
     * Get system health status
     */
    std::string getSystemHealthStatus() const;
    
    /**
     * Export metrics to JSON
     */
    std::string exportToJSON(SystemComponent component = SystemComponent::Unknown,
                            const std::string& metricName = "",
                            std::chrono::duration<double> timeWindow = std::chrono::seconds(60)) const;
    
    /**
     * Export metrics to CSV
     */
    std::string exportToCSV(SystemComponent component = SystemComponent::Unknown,
                           const std::string& metricName = "",
                           std::chrono::duration<double> timeWindow = std::chrono::seconds(60)) const;
    
    /**
     * Export metrics to Prometheus format
     */
    std::string exportToPrometheus(SystemComponent component = SystemComponent::Unknown,
                                   const std::string& metricName = "",
                                   std::chrono::duration<double> timeWindow = std::chrono::seconds(60)) const;
    
    /**
     * Generate metrics report
     */
    std::string generateReport(SystemComponent component = SystemComponent::Unknown,
                               const std::string& metricName = "",
                               std::chrono::duration<double> timeWindow = std::chrono::seconds(60)) const;
    
    /**
     * Generate system-wide report
     */
    std::string generateSystemReport() const;
    
    /**
     * Clear metrics data
     */
    void clearMetrics();
    
    /**
     * Set metrics collection enabled/disabled
     */
    void setCollecting(bool enabled) { isCollecting_ = enabled; }
    
    /**
     * Set health monitoring enabled/disabled
     */
    void setHealthMonitoringEnabled(bool enabled) { healthMonitoringEnabled_ = enabled; }
    
    /**
     * Set anomaly detection enabled/disabled
     */
    void setAnomalyDetectionEnabled(bool enabled) { anomalyDetectionEnabled_ = enabled; }
    
    /**
     * Set metrics correlation enabled/disabled
     */
    void setCorrelationAnalysisEnabled(bool enabled) { correlationAnalysisEnabled_ = enabled; }
    
    /**
     * Get collector statistics
     */
    struct CollectorStats {
        size_t totalDataPointsCollected{0};        // Total data points collected
        size_t totalAlertsGenerated{0};           // Total alerts generated
        size_t totalAnomaliesDetected{0};         // Total anomalies detected
        double currentHealthScore{0.0};          // Current health score
        std::chrono::high_resolution_clock::time_point lastCollectionTime; // Last collection time
        size_t activeMetricsCount{0};             // Number of active metrics
        size_t trackedComponentsCount{0};         // Number of tracked components
        
        double collectionRate{0.0};              // Collection rate (data points per second)
        double alertRate{0.0};                    // Alert rate (alerts per second)
        double anomalyRate{0.0};                  // Anomaly rate (anomalies per second)
    };
    
    CollectorStats getCollectorStats() const;
    
private:
    CollectorConfig config_;
    
    // Metrics data storage
    struct MetricsStorage {
        std::vector<MetricsDataPoint> dataPoints;
        std::chrono::high_resolution_clock::time_point lastAccess;
        std::mutex mutex;
        
        void addDataPoint(const MetricsDataPoint& dataPoint) {
            std::lock_guard<std::mutex> lock(mutex);
            dataPoints.push_back(dataPoint);
            lastAccess = std::chrono::high_resolution_clock::now();
            
            // Limit data points
            if (dataPoints.size() > config_.maxDataPointsPerMetric) {
                dataPoints.erase(dataPoints.begin());
            }
        }
        
        std::vector<MetricsDataPoint> getDataPointsInTimeRange(std::chrono::high_resolution_clock::time_point startTime,
                                                               std::chrono::high_resolution_clock::time_point endTime) const {
            std::lock_guard<std::mutex> lock(mutex);
            std::vector<MetricsDataPoint> result;
            for (const auto& dp : dataPoints) {
                if (dp.timestamp >= startTime && dp.timestamp <= endTime) {
                    result.push_back(dp);
                }
            }
            return result;
        }
        
        void clear() {
            std::lock_guard<std::mutex> lock(mutex);
            dataPoints.clear();
        }
    };
    
    std::unordered_map<std::string, MetricsStorage> metricsStorage_;
    mutable std::mutex storageMutex_;
    
    // Alerts
    std::vector<MetricsAlert> alerts_;
    std::mutex alertsMutex_;
    
    // Anomaly detection
    AnomalyDetection anomalyDetection_;
    bool anomalyDetectionEnabled_;
    
    // Health monitoring
    std::atomic<double> healthScore_;
    bool healthMonitoringEnabled_;
    
    // Correlation analysis
    bool correlationAnalysisEnabled_;
    
    // Collection state
    bool isCollecting_;
    std::chrono::high_resolution_clock::time_point lastCollectionTime_;
    std::future<void> collectionFuture_;
    
    // Background collection thread
    std::thread collectionThread_;
    std::atomic<bool> stopCollectionThread_;
    
    // Statistics
    std::atomic<size_t> totalDataPointsCollected_;
    std::atomic<size_t> totalAlertsGenerated_;
    std::atomic<size_t> totalAnomaliesDetected_;
    
    /**
     * Auto collection thread function
     */
    void autoCollectionThread();
    
    /**
     * Update health score
     */
    void updateHealthScore();
    
    /**
     * Check for alerts
     */
    void checkAlerts();
    
    /**
     * Detect anomalies
     */
    void detectAnomalies();
    
    /**
     * Generate system-wide metrics
     */
    void generateSystemMetrics();
    
    /**
     * Generate correlation metrics
     */
    void generateCorrelationMetrics();
    
    /**
     * Collect memory pool metrics
     */
    template<typename T>
    void collectMemoryPoolMetrics(const MemoryPool<T>& pool, const std::string& context);
    
    /**
     * Collect thread pool metrics
     */
    void collectThreadPoolMetrics(const ThreadPool& pool, const std::string& context);
    
    /**
     * Collect parallel processing metrics
     */
    void collectParallelProcessingMetrics(const ParallelFor& parallelFor, const std::string& context);
    
    /**
     * Calculate metrics aggregation
     */
    static MetricsAggregation calculateAggregation(const std::vector<MetricsDataPoint>& dataPoints);
    
    /**
     * Calculate correlation
     */
    static double calculateCorrelation(const std::vector<MetricsDataPoint>& dataPoints1,
                                       const std::vector<MetricsDataPoint>& dataPoints2);
    
    /**
     * Generate report
     */
    static std::string generateReportInternal(const std::vector<MetricsDataPoint>& dataPoints,
                                               SystemComponent component,
                                               const std::string& metricName);
    
    /**
     * Get global metrics collector instance
     */
    static MetricsCollector& getInstance() {
        static MetricsCollector instance;
        return instance;
    }
    
    /**
     * Generate unique data point key
     */
    static std::string generateDataPointKey(SystemComponent component,
                                            const std::string& metricName,
                                            const std::string& context = "") {
        std::string key = MetricsDataPoint::GetComponentName(component) + "." + metricName;
        if (!context.empty()) {
            key += "." + context;
        }
        return key;
    }
};

// Global metrics collector instance
inline MetricsCollector& gMetricsCollector = MetricsCollector::getInstance();

/**
 * Convenience functions for metrics collection
 */
namespace MetricsUtils {
    
    /**
     * Collect metrics from performance monitor
     */
    inline void collectFromPerformanceMonitor(const PerformanceMonitor& monitor) {
        gMetricsCollector.collectFromPerformanceMonitor(monitor);
    }
    
    /**
     * Collect system metrics
     */
    inline void collectSystemMetrics() {
        gMetricsCollector.collectSystemMetrics();
    }
    
    /**
     * Get health score
     */
    inline double getHealthScore() {
        return gMetricsCollector.getHealthScore();
    }
    
    /**
     * Get system health status
     */
    inline std::string getSystemHealthStatus() {
        return gMetricsCollector.getSystemHealthStatus();
    }
    
    /**
     * Generate report
     */
    inline std::string generateReport() {
        return gMetricsCollector.generateSystemReport();
    }
}

} // namespace nlm

#endif // NLM_METRICS_COLLECTOR_HPP