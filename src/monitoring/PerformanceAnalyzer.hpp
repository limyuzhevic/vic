#pragma once

#include <vector>
#include <string>
#include <memory>
#include <mutex>
#include <chrono>
#include <cmath>
#include <map>
#include <algorithm>

namespace nlm {

// Statistical analysis results
struct AnalysisResult {
    std::string metricName;
    double mean;
    double median;\n    double standardDeviation;
    double variance;\n    double min;
    double max;
    double range;
    double skew;
    double kurtosis;
    std::chrono::system_clock::time_point analysisTime;
    
    AnalysisResult(const std::string& name) : metricName(name), mean(0.0), median(0.0),
                                             standardDeviation(0.0), variance(0.0), min(0.0),
                                             max(0.0), range(0.0), skew(0.0), kurtosis(0.0) {}
};

// Performance trend information
struct PerformanceTrend {
    std::string metricName;
    double trendValue;        // Rate of change per unit time
    std::string trendDirection;  // "increasing", "decreasing", "stable"
    double confidence;        // Confidence level (0.0 to 1.0)
    double rSquared;          // Coefficient of determination
    std::chrono::system_clock::time_point trendTime;
    
    PerformanceTrend(const std::string& name) : metricName(name), trendValue(0.0),
                                               confidence(0.0), rSquared(0.0) {}
};

// Anomaly detection result
struct AnomalyResult {
    std::string metricName;
    double anomalousValue;
    double expectedValue;
    double deviation;
    std::string anomalyType;  // "spike", "drop", "outlier"
    double severity;          // 0.0 to 1.0
    std::chrono::system_clock::time_point detectionTime;
    
    AnomalyResult(const std::string& name) : metricName(name), anomalousValue(0.0),
                                            expectedValue(0.0), deviation(0.0),
                                            severity(0.0) {}
};

// Performance analysis configuration
struct AnalysisConfig {
    double anomalyThreshold;      // Standard deviations for anomaly detection
    size_t movingAverageWindow;    // Window size for moving average
    size_t trendAnalysisWindow;    // Window size for trend analysis
    bool enableStatisticalTests;   // Enable statistical tests
    bool enableTrendAnalysis;      // Enable trend analysis
    bool enableAnomalyDetection;   // Enable anomaly detection
    std::string outputFormat;      // "text", "json", "csv"
    
    AnalysisConfig()
        : anomalyThreshold(3.0), movingAverageWindow(10), trendAnalysisWindow(100),
          enableStatisticalTests(true), enableTrendAnalysis(true),
          enableAnomalyDetection(true), outputFormat("text") {}
};

class PerformanceAnalyzer {
public:
    PerformanceAnalyzer(const AnalysisConfig& config = AnalysisConfig());
    ~PerformanceAnalyzer() = default;
    
    // Configure analyzer
    void configure(const AnalysisConfig& config) { config_ = config; }
    
    // Perform statistical analysis on metrics
    std::vector<AnalysisResult> analyze(const std::vector<MetricData>& metrics);
    
    // Perform trend analysis
    std::vector<PerformanceTrend> analyzeTrends(const std::vector<MetricData>& metrics);
    
    // Detect anomalies
    std::vector<AnomalyResult> detectAnomalies(const std::vector<MetricData>& metrics);
    
    // Generate performance report
    std::string generateReport(const std::vector<MetricData>& metrics,
                               const std::string& format = "auto") const;
    
    // Get analysis results for specific metric
    AnalysisResult getMetricAnalysis(const std::string& metricName,
                                     const std::vector<MetricData>& metrics) const;
    
    // Get performance summary
    std::string getPerformanceSummary(const std::vector<MetricData>& metrics) const;
    
    // Check if analysis is enabled for specific feature
    bool isStatisticalAnalysisEnabled() const { return config_.enableStatisticalTests; }
    bool isTrendAnalysisEnabled() const { return config_.enableTrendAnalysis; }
    bool isAnomalyDetectionEnabled() const { return config_.enableAnomalyDetection; }
    
    // Thread safety
    void lock() { std::lock_guard<std::mutex> lock(mutex_); }
    void unlock() { std::lock_guard<std::mutex> lock(mutex_); }
    
private:
    // Statistical calculations
    double calculateMean(const std::vector<double>& values) const;
    double calculateMedian(std::vector<double> values) const;
    double calculateStandardDeviation(const std::vector<double>& values, double mean) const;
    double calculateVariance(const std::vector<double>& values, double mean) const;
    double calculateSkew(const std::vector<double>& values, double mean, double stdDev) const;
    double calculateKurtosis(const std::vector<double>& values, double mean, double stdDev) const;
    
    // Trend analysis
    double calculateTrend(const std::vector<double>& values) const;
    std::string determineTrendDirection(double trendValue) const;
    double calculateRSquared(const std::vector<double>& x, const std::vector<double>& y) const;
    
    // Anomaly detection
    bool isOutlier(double value, double mean, double stdDev, double threshold) const;
    
    // Moving average
    std::vector<double> calculateMovingAverage(const std::vector<double>& values, size_t window) const;
    
    AnalysisConfig config_;
    mutable std::mutex mutex_;
    
    // Trend analysis cache
    mutable std::map<std::string, PerformanceTrend> trendCache_;
    
    // Analysis results cache
    mutable std::map<std::string, AnalysisResult> analysisCache_;
};

} // namespace nlm
