#pragma once

/**
 * PerformanceMonitor - Real-time performance monitoring and analysis
 * 
 * Provides comprehensive performance monitoring including:
 * - Real-time performance metrics collection
 * - Performance profiling hooks
 * - Resource usage tracking
 * - Performance regression detection
 * - Automated performance analysis
 * - Performance optimization recommendations
 * - Performance alerting
 * - Performance dashboards
 * - Historical performance analysis
 * - Performance trend analysis
 * - Performance benchmarking integration
 * 
 * Features:
 * - Real-time performance monitoring
 * - Historical performance tracking
 * - Performance regression detection
 * - Automated performance analysis
 * - Performance optimization recommendations
 * - Performance alerting
 * - Performance dashboards
 * - Historical performance analysis
 * - Performance trend analysis
 * - Performance benchmarking integration
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <thread>
#include <queue>
#include <deque>
#include <map>
#include <set>
#include <array>
#include <variant>
#include <optional>
#include <regex>
#include <zlib.h>
#include <lz4.h>
#include <thread>

namespace nlm {

// Performance metric types
enum class MetricType {
    Gauge,           // Current value
    Counter,         // Monotonic increasing
    Histogram,       // Distribution
    Summary,         // Statistical summary
    Timer,           // Duration measurement
    Custom          // Custom metric type
};

// Performance metric
struct PerformanceMetric {
    std::string name;                     // Metric name
    MetricType type;                      // Metric type
    double value;                         // Metric value
    uint64_t timestamp;                   // Timestamp (ns)
    std::string label;                    // Metric label
    std::unordered_map<std::string, std::string> tags;   // Tags for grouping
    uint64_t count;                       // Count (for counters)
    double sum;                          // Sum (for histograms)
    double min;                          // Minimum (for histograms)
    double max;                          // Maximum (for histograms)
    double quantiles;                      // Quantiles (for histograms)
    std::string unit;                     // Unit of measurement
    std::string description;              // Metric description
};

// Performance alert
struct PerformanceAlert {
    std::string id;                       // Alert ID
    std::string name;                     // Alert name
    std::string description;              // Alert description
    MetricType metricType;                // Metric type
    std::string metricName;               // Metric name
    std::string condition;                // Alert condition (e.g., "value > 100")
    std::string severity;                 // Severity level (info, warning, error, critical)
    bool enabled;                         // Whether alert is enabled
    std::chrono::nanoseconds cooldown;    // Cooldown period (ns)
    std::string action;                   // Action to take (none, log, notify, throttle)
    std::string notificationChannels;     // Notification channels
    uint64_t lastTriggered;               // Last trigger timestamp
    uint64_t triggerCount;                // Number of times triggered
    std::string message;                  // Last alert message
};

// Performance analysis result
struct PerformanceAnalysis {
    std::string analysisName;             // Analysis name
    std::string description;              // Analysis description
    std::vector<std::string> metrics;     // Metrics analyzed
    double score;                         // Analysis score (0-100)
    std::string grade;                    // Letter grade (A-F)
    std::vector<std::string> recommendations; // Recommendations
    std::vector<std::string> issues;      // Issues found
    std::vector<std::string> warnings;    // Warnings
    double improvementPotential;          // Improvement potential (0-100)
    uint64_t analysisTime;                // Analysis time (ns)
    std::string analysisDetails;          // Detailed analysis
};

// Performance trend
struct PerformanceTrend {
    std::string metricName;               // Metric name
    double slope;                         // Trend slope
    double rSquared;                      // R-squared value
    std::string direction;                // Trend direction (increasing, decreasing, stable)
    std::chrono::nanoseconds period;      // Analysis period
    double averageChange;                 // Average change per period
    std::vector<std::pair<uint64_t, double>> dataPoints; // Data points
};

// Performance regression
struct PerformanceRegression {
    std::string metricName;               // Metric name
    double baselineValue;                  // Baseline value
    double currentValue;                   // Current value
    double regressionPercent;             // Regression percentage
    std::chrono::nanoseconds detectionTime; // Detection time
    std::string severity;                  // Severity level
    std::string description;               // Description
    std::string suggestedFix;             // Suggested fix
};

// Performance benchmark result
struct PerformanceBenchmark {
    std::string benchmarkName;            // Benchmark name
    std::string benchmarkSuite;           // Benchmark suite
    double executionTime;                 // Execution time (seconds)
    double operationsPerSecond;           // Operations per second
    double memoryUsage;                   // Memory usage (MB)
    double cpuUsage;                      // CPU usage (%)
    uint64_t timestamp;                   // Benchmark timestamp
    std::string hardwareInfo;             // Hardware information
    std::string configuration;            // Configuration used
    std::vector<PerformanceBenchmark> previousRuns; // Previous runs for comparison
};

/**
 * Metric collector
 */
class MetricCollector {
public:
    MetricCollector() = default;
    ~MetricCollector() = default;
    
    /**
     * Record a metric
     */
    void recordMetric(const PerformanceMetric& metric);
    
    /**
     * Record a gauge metric
     */
    void recordGauge(const std::string& name, double value, 
                     const std::string& label = "",
                     const std::unordered_map<std::string, std::string>& tags = {});
    
    /**
     * Record a counter metric
     */
    void recordCounter(const std::string& name, uint64_t count = 1,
                      const std::string& label = "",
                      const std::unordered_map<std::string, std::string>& tags = {});
    
    /**
     * Record a histogram metric
     */
    void recordHistogram(const std::string& name, double value,
                        const std::string& label = "",
                        const std::unordered_map<std::string, std::string>& tags = {});
    
    /**
     * Record a timer metric
     */
    void recordTimer(const std::string& name, double duration,
                    const std::string& label = "",
                    const std::unordered_map<std::string, std::string>& tags = {});
    
    /**
     * Get metric by name
     */
    std::vector<PerformanceMetric> getMetric(const std::string& name) const;
    
    /**
     * Get metrics by type
     */
    std::vector<PerformanceMetric> getMetricsByType(MetricType type) const;
    
    /**
     * Get metrics by tag
     */
    std::vector<PerformanceMetric> getMetricsByTag(const std::string& tagName,
                                                   const std::string& tagValue) const;
    
    /**
     * Get all metrics
     */
    std::vector<PerformanceMetric> getAllMetrics() const;
    
    /**
     * Get metric statistics
     */
    struct MetricStats {
        double count;                       // Count
        double sum;                         // Sum
        double min;                         // Minimum
        double max;                         // Maximum
        double mean;                        // Mean
        double median;                      // Median
        double stddev;                      // Standard deviation
        double p95;                         // 95th percentile
        double p99;                         // 99th percentile
    };
    
    MetricStats getMetricStats(const std::string& name) const;
    
    /**
     * Clear metrics
     */
    void clearMetrics(const std::string& name = "");
    
    /**
     * Get metric names
     */
    std::vector<std::string> getMetricNames() const;
    
    /**
     * Get latest metric values
     */
    std::unordered_map<std::string, double> getLatestValues() const;

private:
    mutable std::mutex metricsMutex_;
    std::deque<PerformanceMetric> metrics_;
    uint64_t maxHistorySize_ = 100000;
};

/**
 * Alert manager
 */
class AlertManager {
public:
    AlertManager() = default;
    ~AlertManager() = default;
    
    /**
     * Add an alert
     */
    void addAlert(const PerformanceAlert& alert);
    
    /**
     * Remove an alert
     */
    void removeAlert(const std::string& alertId);
    
    /**
     * Update an alert
     */
    void updateAlert(const PerformanceAlert& alert);
    
    /**
     * Get alert by ID
     */
    std::optional<PerformanceAlert> getAlert(const std::string& alertId) const;
    
    /**
     * Get all alerts
     */
    std::vector<PerformanceAlert> getAllAlerts() const;
    
    /**
     * Get alerts by severity
     */
    std::vector<PerformanceAlert> getAlertsBySeverity(const std::string& severity) const;
    
    /**
     * Get alerts by metric
     */
    std::vector<PerformanceAlert> getAlertsByMetric(const std::string& metricName) const;
    
    /**
     * Check alerts against current metrics
     */
    std::vector<PerformanceAlert> checkAlerts(const MetricCollector& collector) const;
    
    /**
     * Clear triggered alerts
     */
    void clearTriggeredAlerts();
    
    /**
     * Enable/disable alert
     */
    void setAlertEnabled(const std::string& alertId, bool enabled);
    
    /**
     * Get alert statistics
     */
    struct AlertStats {
        uint32_t totalAlerts;               // Total alerts
        uint32_t enabledAlerts;             // Enabled alerts
        uint32_t triggeredAlerts;           // Triggered alerts
        std::unordered_map<std::string, uint32_t> severityCounts; // Severity counts
    };
    
    AlertStats getStats() const;

private:
    mutable std::mutex alertsMutex_;
    std::vector<PerformanceAlert> alerts_;
    bool alertsChanged_;
};

/**
 * Performance analyzer
 */
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer() = default;
    ~PerformanceAnalyzer() = default;
    
    /**
     * Analyze performance metrics
     */
    PerformanceAnalysis analyze(const MetricCollector& collector,
                               const std::vector<std::string>& metricNames = {});
    
    /**
     * Analyze performance trends
     */
    PerformanceTrend analyzeTrend(const std::vector<PerformanceMetric>& metrics,
                                 std::chrono::nanoseconds period);
    
    /**
     * Detect performance regressions
     */
    std::vector<PerformanceRegression> detectRegressions(
        const std::vector<PerformanceBenchmark>& benchmarks,
        double regressionThresholdPercent = 5.0);
    
    /**
     * Generate performance recommendations
     */
    std::vector<std::string> generateRecommendations(const PerformanceAnalysis& analysis);
    
    /**
     * Analyze metric distribution
     */
    std::string analyzeMetricDistribution(const std::vector<PerformanceMetric>& metrics);
    
    /**
     * Identify bottlenecks
     */
    std::vector<std::string> identifyBottlenecks(const MetricCollector& collector);
    
    /**
     * Calculate performance score
     */
    double calculatePerformanceScore(const PerformanceAnalysis& analysis);
    
    /**
     * Grade performance
     */
    std::string gradePerformance(double score);

private:
    /**
     * Calculate linear regression
     */
    static std::pair<double, double> linearRegression(const std::vector<std::pair<uint64_t, double>>& data);
    
    /**
     * Calculate standard deviation
     */
    static double calculateStdDev(const std::vector<double>& values);
    
    /**
     * Calculate coefficient of variation
     */
    static double calculateCV(const std::vector<double>& values, double mean);
};

/**
 * Performance reporter
 */
class PerformanceReporter {
public:
    struct ReportConfig {
        std::string format;                 // Report format (text, json, html)
        std::string outputFile;             // Output file
        bool includeMetrics;                // Include metrics
        bool includeAlerts;                 // Include alerts
        bool includeAnalysis;               // Include analysis
        bool includeTrends;                 // Include trends
        bool includeRegressions;            // Include regressions
        bool includeBenchmarks;             // Include benchmarks
        std::string metricFilter;           // Metric name filter
        std::unordered_map<std::string, std::string> tagFilters; // Tag filters
        uint32_t metricsPerPage;            // Metrics per page (for HTML)
        bool includeRawData;                // Include raw data
    };
    
    PerformanceReporter() = default;
    ~PerformanceReporter() = default;
    
    /**
     * Generate report
     */
    std::string generateReport(const MetricCollector& collector,
                              const AlertManager& alertManager,
                              const PerformanceAnalyzer& analyzer,
                              const ReportConfig& config = ReportConfig());
    
    /**
     * Save report to file
     */
    bool saveReport(const std::string& filepath, const std::string& content);
    
    /**
     * Generate HTML report
     */
    std::string generateHTMLReport(const MetricCollector& collector,
                                  const AlertManager& alertManager,
                                  const PerformanceAnalyzer& analyzer,
                                  const ReportConfig& config = ReportConfig());
    
    /**
     * Generate JSON report
     */
    std::string generateJSONReport(const MetricCollector& collector,
                                  const AlertManager& alertManager,
                                  const PerformanceAnalyzer& analyzer,
                                  const ReportConfig& config = ReportConfig());
    
    /**
     * Generate text report
     */
    std::string generateTextReport(const MetricCollector& collector,
                                  const AlertManager& alertManager,
                                  const PerformanceAnalyzer& analyzer,
                                  const ReportConfig& config = ReportConfig());
    
    /**
     * Generate dashboard data
     */
    std::string generateDashboardData(const MetricCollector& collector,
                                     const AlertManager& alertManager,
                                     const PerformanceAnalyzer& analyzer,
                                     const ReportConfig& config = ReportConfig());

private:
    /**
     * Format metric value
     */
    static std::string formatMetricValue(double value, const std::string& unit);
    
    /**
     * Format timestamp
     */
    static std::string formatTimestamp(uint64_t timestampNs);
    
    /**
     * Escape HTML
     */
    static std::string escapeHTML(const std::string& input);
    
    /**
     * Generate CSS
     */
    static std::string generateCSS();
    
    /**
     * Generate JavaScript
     */
    static std::string generateJavaScript();
};

/**
 * Performance monitor - Main class
 */
class PerformanceMonitor {
public:
    struct Config {
        bool enableRealTimeMonitoring;       // Enable real-time monitoring
        uint32_t monitoringIntervalMs;       // Monitoring interval (ms)
        bool enableAlerting;                // Enable alerting
        bool enableAnalysis;                // Enable performance analysis
        bool enableReporting;                // Enable reporting
        std::string reportFormat;            // Report format
        std::string reportFile;              // Report file
        std::string alertConfigFile;        // Alert configuration file
        uint32_t maxHistorySize;             // Maximum history size
        bool enableHistoricalAnalysis;       // Enable historical analysis
        std::string historicalDataFile;     // Historical data file
        bool enableAutoOptimization;        // Enable automatic optimization
        std::string optimizationRulesFile;   // Optimization rules file
        bool enableResourceProfiling;        // Enable resource profiling
        uint32_t resourceProfilingIntervalMs; // Resource profiling interval (ms)
    };
    
    PerformanceMonitor(const Config& config);
    ~PerformanceMonitor();
    
    /**
     * Configure monitor
     */
    void configure(const Config& config);
    
    /**
     * Start monitoring
     */
    void startMonitoring();
    
    /**
     * Stop monitoring
     */
    void stopMonitoring();
    
    /**
     * Record a metric
     */
    void recordMetric(const PerformanceMetric& metric);
    
    /**
     * Record a gauge metric
     */
    void recordGauge(const std::string& name, double value,
                    const std::string& label = "",
                    const std::unordered_map<std::string, std::string>& tags = {});
    
    /**
     * Record a counter metric
     */
    void recordCounter(const std::string& name, uint64_t count = 1,
                      const std::string& label = "",
                      const std::unordered_map<std::string, std::string>& tags = {});
    
    /**
     * Record a histogram metric
     */
    void recordHistogram(const std::string& name, double value,
                        const std::string& label = "",
                        const std::unordered_map<std::string, std::string>& tags = {});
    
    /**
     * Record a timer metric
     */
    void recordTimer(const std::string& name, double duration,
                    const std::string& label = "",
                    const std::unordered_map<std::string, std::string>& tags = {});
    
    /**
     * Get metric collector
     */
    MetricCollector& getMetricCollector() { return metricCollector_; }
    
    /**
     * Get alert manager
     */
    AlertManager& getAlertManager() { return alertManager_; }
    
    /**
     * Get performance analyzer
     */
    PerformanceAnalyzer& getPerformanceAnalyzer() { return performanceAnalyzer_; }
    
    /**
     * Get performance reporter
     */
    PerformanceReporter& getPerformanceReporter() { return performanceReporter_; }
    
    /**
     * Generate report
     */
    std::string generateReport();
    
    /**
     * Save report to file
     */
    bool saveReport(const std::string& filepath = "");
    
    /**
     * Load report from file
     */
    bool loadReport(const std::string& filepath);
    
    /**
     * Get performance analysis
     */
    PerformanceAnalysis getPerformanceAnalysis();
    
    /**
     * Get performance alerts
     */
    std::vector<PerformanceAlert> getPerformanceAlerts();
    
    /**
     * Get performance benchmarks
     */
    std::vector<PerformanceBenchmark> getPerformanceBenchmarks();
    
    /**
     * Add performance benchmark
     */
    void addPerformanceBenchmark(const PerformanceBenchmark& benchmark);
    
    /**
     * Get performance trends
     */
    std::vector<PerformanceTrend> getPerformanceTrends();
    
    /**
     * Get performance regressions
     */
    std::vector<PerformanceRegression> getPerformanceRegressions();
    
    /**
     * Set performance alert
     */
    void setPerformanceAlert(const PerformanceAlert& alert);
    
    /**
     * Remove performance alert
     */
    void removePerformanceAlert(const std::string& alertId);
    
    /**
     * Is monitoring active
     */
    bool isMonitoring() const { return monitoring_; }
    
    /**
     * Get configuration
     */
    const Config& getConfig() const { return config_; }
    
    /**
     * Get monitoring status
     */
    std::string getMonitoringStatus() const;
    
    /**
     * Get system information
     */
    std::string getSystemInfo() const;
    
    /**
     * Get resource usage
     */
    std::string getResourceUsage() const;
    
    /**
     * Get performance recommendations
     */
    std::vector<std::string> getPerformanceRecommendations();
    
    /**
     * Get performance issues
     */
    std::vector<std::string> getPerformanceIssues();
    
    /**
     * Get performance warnings
     */
    std::vector<std::string> getPerformanceWarnings();

private:
    Config config_;
    bool monitoring_;
    mutable std::mutex monitorMutex_;
    
    // Components
    MetricCollector metricCollector_;
    AlertManager alertManager_;
    PerformanceAnalyzer performanceAnalyzer_;
    PerformanceReporter performanceReporter_;
    
    // Performance data
    std::vector<PerformanceBenchmark> performanceBenchmarks_;
    std::vector<PerformanceTrend> performanceTrends_;
    std::vector<PerformanceRegression> performanceRegressions_;
    
    // Background monitoring thread
    std::thread monitoringThread_;
    
    /**
     * Background monitoring loop
     */
    void monitoringLoop();
    
    /**
     * Update performance metrics
     */
    void updatePerformanceMetrics();
    
    /**
     * Check alerts
     */
    void checkAlerts();
    
    /**
     * Generate performance analysis
     */
    void generatePerformanceAnalysis();
    
    /**
     * Update performance trends
     */
    void updatePerformanceTrends();
    
    /**
     * Detect performance regressions
     */
    void detectPerformanceRegressions();
    
    /**
     * Generate performance reports
     */
    void generatePerformanceReports();
    
    /**
     * Apply automatic optimizations
     */
    void applyAutomaticOptimizations();
    
    /**
     * Load alert configuration
     */
    void loadAlertConfiguration();
    
    /**
     * Load optimization rules
     */
    void loadOptimizationRules();
};

} // namespace nlm