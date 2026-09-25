#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <limits>
#include <queue>
#include <set>

namespace nlm {

// Statistical analysis results
struct StatisticalAnalysis {
    double mean;
    double median;
    double mode;
    double standardDeviation;
    double variance;
    double min;
    double max;
    double range;
    double coefficientOfVariation;
    double skewness;
    double kurtosis;
    size_t sampleSize;
    
    StatisticalAnalysis() : mean(0.0), median(0.0), mode(0.0), standardDeviation(0.0),
                          variance(0.0), min(0.0), max(0.0), range(0.0),
                          coefficientOfVariation(0.0), skewness(0.0), kurtosis(0.0),
                          sampleSize(0) {}
};

// Performance comparison results
struct PerformanceComparison {
    std::string baselineOperation;
    std::string currentOperation;
    double baselineDuration;
    double currentDuration;
    double improvementPercent;
    double degradationPercent;
    bool isBetter;
    std::string trend;
    
    PerformanceComparison() : baselineDuration(0.0), currentDuration(0.0),
                              improvementPercent(0.0), degradationPercent(0.0),
                              isBetter(false) {}
};

// Anomaly detection results
struct AnomalyDetection {
    bool isAnomaly;
    double anomalyScore;
    std::string anomalyType;
    std::string description;
    std::chrono::system_clock::time_point anomalyTime;
    std::map<std::string, double> contributingFactors;
    
    AnomalyDetection() : isAnomaly(false), anomalyScore(0.0), anomalyType("None"),
                        description("No anomaly detected"), anomalyTime(std::chrono::system_clock::now()) {}
};

// Trend analysis results
struct TrendAnalysis {
    std::string trendType; // "increasing", "decreasing", "stable", "oscillating"
    double trendStrength; // 0.0 to 1.0
    double trendDirection; // -1.0 to 1.0
    std::vector<double> trendPoints;
    std::chrono::system_clock::time_point analysisTime;
    
    TrendAnalysis() : trendStrength(0.0), trendDirection(0.0) {
        analysisTime = std::chrono::system_clock::now();
    }
};

// Performance baseline management
struct PerformanceBaseline {
    std::string operation;
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point lastUpdated;
    StatisticalAnalysis baselineStats;
    std::vector<double> baselineValues;
    bool isActive;
    
    PerformanceBaseline() : isActive(true) {
        created = std::chrono::system_clock::now();
        lastUpdated = created;
    }
};

// Main MetricCalculator class
class MetricCalculator {
public:
    MetricCalculator();
    ~MetricCalculator();
    
    // Disable copying, enable moving
    MetricCalculator(const MetricCalculator&) = delete;
    MetricCalculator& operator=(const MetricCalculator&) = delete;
    MetricCalculator(MetricCalculator&&) noexcept;
    MetricCalculator& operator=(MetricCalculator&&) noexcept;
    
    // Statistical analysis
    StatisticalAnalysis analyze(const std::vector<double>& values) const;
    StatisticalAnalysis analyze(const std::vector<PerformanceDataPoint>& dataPoints) const;
    
    // Performance metrics
    PerformanceComparison compareWithBaseline(const std::string& operation,
                                             double currentDuration) const;
    PerformanceComparison compareMetrics(const std::map<std::string, double>& baseline,
                                        const std::map<std::string, double>& current) const;
    
    // Anomaly detection
    AnomalyDetection detectAnomaly(const std::vector<double>& values, 
                                  double threshold = 2.0) const;
    AnomalyDetection detectOutliers(const std::vector<PerformanceDataPoint>& dataPoints,
                                   double zScoreThreshold = 2.0) const;
    
    // Trend analysis
    TrendAnalysis analyzeTrend(const std::vector<std::pair<std::chrono::system_clock::time_point, double>>& timeSeries) const;
    TrendAnalysis analyzePerformanceTrend(const std::string& operation,
                                          const std::chrono::system_clock::time_point& since) const;
    
    // Performance statistics
    double calculatePerformanceScore(const std::vector<PerformanceDataPoint>& dataPoints) const;
    double calculateEfficiency(const PerformanceDataPoint& dataPoint) const;
    double calculateUtilization(const PerformanceDataPoint& dataPoint) const;
    
    // Baseline management
    void addBaseline(const std::string& operation, const std::vector<double>& values);
    void updateBaseline(const std::string& operation, double newValue);
    PerformanceBaseline getBaseline(const std::string& operation) const;
    void removeBaseline(const std::string& operation);
    void clearBaselines();
    
    // Time-based analysis
    std::vector<double> extractTimeSeries(const std::string& operation,
                                         const std::chrono::system_clock::time_point& since,
                                         const std::chrono::system_clock::time_point& until) const;
    std::map<std::string, std::vector<double>> extractAllTimeSeries(const std::chrono::system_clock::time_point& since) const;
    
    // Performance profiling
    std::vector<std::string> getOperations() const;
    bool hasOperation(const std::string& operation) const;
    std::chrono::system_clock::time_point getLastUpdateTime(const std::string& operation) const;
    
    // Threshold and alerting
    void setAnomalyThreshold(double threshold);
    void setPerformanceThreshold(double threshold);
    void setTrendStrengthThreshold(double threshold);
    
    double getAnomalyThreshold() const;
    double getPerformanceThreshold() const;
    double getTrendStrengthThreshold() const;
    
    // Static instance management
    static MetricCalculator& getInstance();
    static void setGlobal(std::shared_ptr<MetricCalculator> calculator);
    
    // Integration helpers
    void recordMetric(const std::string& name, double value, const std::chrono::system_clock::time_point& timestamp = std::chrono::system_clock::now());
    void recordPerformance(const std::string& operation, double duration, bool success = true);
    
    // Analysis utilities
    std::string getPerformanceReport() const;
    std::string getBaselineReport() const;
    std::string getAnomalyReport() const;
    std::string getTrendReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void cleanupOldData();
    double calculateMean(const std::vector<double>& values) const;
    double calculateMedian(const std::vector<double>& values) const;
    double calculateStandardDeviation(const std::vector<double>& values, double mean) const;
    std::string detectAnomalyType(const std::vector<double>& values) const;
    double calculateAnomalyScore(const std::vector<double>& values) const;
};

// Performance analyzer
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    // Performance profiling
    void profileFunction(const std::string& name, std::function<void()> func);
    void profileOperation(const std::string& operation, double duration, bool success = true);
    double getProfileTime(const std::string& name) const;
    
    // Bottleneck analysis
    std::vector<std::string> identifyBottlenecks(const std::string& operation) const;
    std::vector<std::pair<std::string, double>> getSlowestOperations(size_t limit = 10) const;
    std::vector<std::pair<std::string, double>> getFastestOperations(size_t limit = 10) const;
    
    // Resource analysis
    double calculateCPUUsage(const std::string& operation) const;
    uint64_t calculateMemoryUsage(const std::string& operation) const;
    
    // Scalability analysis
    std::vector<double> getScalabilityData(const std::string& operation, size_t scaleFactor = 10) const;
    std::string analyzeScalability(const std::vector<double>& scalabilityData) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void recordProfile(const std::string& name, double duration, bool success);
};

// Baseline analyzer
class BaselineAnalyzer {
public:
    BaselineAnalyzer();
    ~BaselineAnalyzer();
    
    // Baseline validation
    bool validateBaseline(const std::string& operation, const std::vector<double>& values) const;
    double calculateBaselineDeviation(const std::string& operation, double currentValue) const;
    
    // Baseline adjustment
    void adjustBaseline(const std::string& operation, double adjustmentFactor);
    void updateBaselineWithNewData(const std::string& operation, const std::vector<double>& newData);
    
    // Baseline analysis
    std::vector<std::string> getBaselineOperations() const;
    StatisticalAnalysis getBaselineStats(const std::string& operation) const;
    
    // Stability analysis
    bool isBaselineStable(const std::string& operation, double stabilityThreshold = 0.05) const;
    double calculateBaselineStability(const std::string& operation) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool isValueWithinThreshold(double value, double baseline, double threshold) const;
};

// Time series analyzer
class TimeSeriesAnalyzer {
public:
    TimeSeriesAnalyzer();
    ~TimeSeriesAnalyzer();
    
    // Time series construction
    std::vector<std::pair<std::chrono::system_clock::time_point, double>> buildTimeSeries(
        const std::vector<PerformanceDataPoint>& dataPoints) const;
    std::map<std::string, std::vector<std::pair<std::chrono::system_clock::time_point, double>>> buildAllTimeSeries(
        const std::vector<PerformanceDataPoint>& dataPoints) const;
    
    // Time series analysis
    TrendAnalysis analyzeTimeSeries(const std::vector<std::pair<std::chrono::system_clock::time_point, double>>& series) const;
    AnomalyDetection detectAnomalies(const std::vector<std::pair<std::chrono::system_clock::time_point, double>>& series) const;
    
    // Decomposition
    std::vector<double> decomposeTrend(const std::vector<double>& series) const;
    std::vector<double> decomposeSeasonality(const std::vector<double>& series) const;
    std::vector<double> decomposeNoise(const std::vector<double>& series) const;
    
    // Interpolation and smoothing
    std::vector<double> interpolateTimeSeries(const std::vector<std::pair<std::chrono::system_clock::time_point, double>>& series,
                                             const std::chrono::system_clock::time_point& targetTime) const;
    std::vector<double> smoothTimeSeries(const std::vector<double>& series, int windowSize = 5) const;
    
    // Forecast
    std::vector<double> forecast(const std::vector<double>& series, size_t forecastPoints = 10) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    double calculateLinearTrend(const std::vector<double>& series) const;
    double calculateExponentialTrend(const std::vector<double>& series) const;
};

// Metric utility functions
namespace Metrics {
    // Statistical utilities
    inline double mean(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    }
    
    inline double median(std::vector<double> values) {
        if (values.empty()) return 0.0;
        std::sort(values.begin(), values.end());
        size_t n = values.size();
        if (n % 2 == 0) {
            return (values[n/2 - 1] + values[n/2]) / 2.0;
        } else {
            return values[n/2];
        }
    }
    
    inline double standardDeviation(const std::vector<double>& values, double mean = -1.0) {
        if (values.empty()) return 0.0;
        if (mean < 0.0) mean = Metrics::mean(values);
        double sum = 0.0;
        for (double value : values) {
            sum += (value - mean) * (value - mean);
        }
        return std::sqrt(sum / values.size());
    }
    
    inline double coefficientOfVariation(const std::vector<double>& values, double mean = -1.0) {
        if (values.empty()) return 0.0;
        if (mean < 0.0) mean = Metrics::mean(values);
        double stddev = standardDeviation(values, mean);
        return (mean > 0.0) ? (stddev / mean) : 0.0;
    }
    
    // Time utilities
    inline std::string formatDuration(double seconds) {
        std::stringstream ss;
        if (seconds >= 3600) {
            ss << std::fixed << std::setprecision(1) << (seconds / 3600) << "h";
            seconds = std::fmod(seconds, 3600.0);
        }
        if (seconds >= 60) {
            ss << std::fixed << std::setprecision(0) << (seconds / 60) << "m";
            seconds = std::fmod(seconds, 60.0);
        }
        ss << std::fixed << std::setprecision(3) << seconds << "s";
        return ss.str();
    }
    
    inline std::string formatBytes(uint64_t bytes) {
        if (bytes >= 1024 * 1024 * 1024 * 1024) {
            return std::to_string(bytes / (1024.0 * 1024 * 1024 * 1024)) + " TB";
        } else if (bytes >= 1024 * 1024 * 1024) {
            return std::to_string(bytes / (1024.0 * 1024 * 1024)) + " GB";
        } else if (bytes >= 1024 * 1024) {
            return std::to_string(bytes / (1024.0 * 1024)) + " MB";
        } else if (bytes >= 1024) {
            return std::to_string(bytes / 1024.0) + " KB";
        } else {
            return std::to_string(bytes) + " B";
        }
    }
}

} // namespace nlm