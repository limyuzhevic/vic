#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <functional>
#include <set>
#include <limits>
#include <sstream>

namespace nlm {

class PerformanceAnalyzer {
public:
    // Statistical analysis results
    struct StatisticalSummary {
        std::string metricName;
        double mean;
        double median;
        double standardDeviation;
        double variance;
        double minValue;
        double maxValue;
        double range;
        size_t sampleCount;
        double coefficientOfVariation;
        std::vector<double> percentiles; // 25th, 50th, 75th, 90th, 95th, 99th
        
        StatisticalSummary(const std::string& name) : metricName(name), mean(0.0), median(0.0),
                                                       standardDeviation(0.0), variance(0.0),
                                                       minValue(0.0), maxValue(0.0), range(0.0),
                                                       sampleCount(0), coefficientOfVariation(0.0) {}
    };

    struct TrendAnalysis {
        std::string metricName;
        std::vector<double> values;
        std::vector<std::chrono::time_point<std::chrono::system_clock>> timestamps;
        double slope;           // Linear regression slope
        double rSquared;        // Coefficient of determination
        std::string trend;      // "INCREASING", "DECREASING", "STABLE", "VOLATILE"
        double volatility;      // Standard deviation of first differences
        std::chrono::time_point<std::chrono::system_clock> trendStart;
        std::chrono::time_point<std::chrono::system_clock> trendEnd;
        
        TrendAnalysis(const std::string& name) : metricName(name), slope(0.0), rSquared(0.0),
                                                 volatility(0.0) {}
    };

    struct CorrelationAnalysis {
        std::string metric1;
        std::string metric2;
        double correlationCoefficient;
        double pValue;
        std::string relationship; // "STRONG_POSITIVE", "WEAK_POSITIVE", "NONE", "WEAK_NEGATIVE", "STRONG_NEGATIVE"
        size_t sampleSize;
        std::vector<std::pair<double, double>> dataPoints;
        
        CorrelationAnalysis(const std::string& m1, const std::string& m2) 
            : metric1(m1), metric2(m2), correlationCoefficient(0.0), pValue(0.0), sampleSize(0) {}
    };

    struct OptimizationSuggestion {
        std::string metricName;
        double currentValue;
        double targetValue;
        double improvementPercentage;
        std::string suggestedAction;
        std::string priority; // "HIGH", "MEDIUM", "LOW"
        std::string impact;    // "PERFORMANCE", "MEMORY", "CPU", "NETWORK"
        double estimatedGain;
        std::string implementationDifficulty; // "EASY", "MEDIUM", "HARD"
        std::string relatedBenchmarks;
        
        OptimizationSuggestion(const std::string& name) : metricName(name), currentValue(0.0),
                                                           targetValue(0.0), improvementPercentage(0.0) {}
    };

    struct PerformanceModel {
        std::string modelName;
        std::string metricBeingPredicted;
        std::string inputParameters;
        std::function<double(const std::map<std::string, double>&)> predictionFunction;
        double rSquared;
        std::chrono::time_point<std::chrono::system_clock> lastTrained;
        size_t trainingSampleCount;
        
        PerformanceModel(const std::string& name, const std::string& metric)
            : modelName(name), metricBeingPredicted(metric), rSquared(0.0), trainingSampleCount(0) {}
    };

    struct PerformancePrediction {
        std::string predictionId;
        std::string metricName;
        std::chrono::time_point<std::chrono::system_clock> predictionTime;
        std::chrono::time_point<std::chrono::system_clock> predictionHorizon;
        double predictedValue;
        double confidenceInterval;
        double uncertainty;
        std::string scenario; // "NORMAL", "STRESS", "EXTREME"
        
        PerformancePrediction(const std::string& id, const std::string& metric)
            : predictionId(id), metricName(metric), predictedValue(0.0), 
              confidenceInterval(0.0), uncertainty(0.0) {}
    };

    // Time series data point
    struct TimeSeriesPoint {
        std::chrono::time_point<std::chrono::system_clock> timestamp;
        std::string metricName;
        std::map<std::string, double> measurements;
        std::string benchmarkName;
        
        TimeSeriesPoint(const std::chrono::time_point<std::chrono::system_clock>& time,
                       const std::string& metric, const std::string& benchmark)
            : timestamp(time), metricName(metric), benchmarkName(benchmark) {}
    };

    // Constructor and destructor
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();

    // Initialize analyzer
    bool initialize();
    void shutdown();

    // Data ingestion
    void addBenchmarkResult(const std::string& benchmarkName, const std::string& metricName,
                           double value, const std::chrono::time_point<std::chrono::system_clock>& timestamp = std::chrono::system_clock::now());
    void addTimeSeriesPoint(const TimeSeriesPoint& point);
    void addBenchmarkRun(const std::string& benchmarkName, 
                        const std::map<std::string, double>& metrics,
                        const std::chrono::time_point<std::chrono::system_clock>& timestamp = std::chrono::system_clock::now());

    // Statistical analysis
    StatisticalSummary calculateStatistics(const std::string& metricName, 
                                          size_t windowSize = 100) const;
    std::vector<StatisticalSummary> calculateStatisticsForAllMetrics(size_t windowSize = 100) const;
    double calculateMean(const std::vector<double>& values) const;
    double calculateMedian(const std::vector<double>& values) const;
    double calculateStandardDeviation(const std::vector<double>& values, double mean = 0.0) const;
    std::vector<double> calculatePercentiles(const std::vector<double>& values, 
                                            const std::vector<double>& percentiles) const;

    // Trend analysis
    TrendAnalysis analyzeTrend(const std::string& metricName, 
                              size_t windowSize = 50) const;
    std::vector<TrendAnalysis> analyzeTrendsForAllMetrics(size_t windowSize = 50) const;
    double linearRegressionSlope(const std::vector<double>& xValues, 
                                 const std::vector<double>& yValues) const;
    double calculateRSquared(const std::vector<double>& xValues, 
                            const std::vector<double>& yValues, 
                            double slope, double meanY) const;

    // Correlation analysis
    CorrelationAnalysis calculateCorrelation(const std::string& metric1, 
                                            const std::string& metric2,
                                            size_t windowSize = 100) const;
    std::vector<CorrelationAnalysis> calculateCorrelations(const std::vector<std::string>& metricNames,
                                                          size_t windowSize = 100) const;
    double pearsonCorrelation(const std::vector<double>& xValues, 
                            const std::vector<double>& yValues) const;
    double spearmanCorrelation(const std::vector<double>& xValues, 
                              const std::vector<double>& yValues) const;

    // Optimization guidance
    std::vector<OptimizationSuggestion> generateOptimizationSuggestions() const;
    OptimizationSuggestion analyzeBottleneck(const std::string& metricName) const;
    std::vector<OptimizationSuggestion> analyzeResourceConstraints() const;

    // Performance modeling
    bool trainModel(const std::string& modelName, 
                   const std::string& targetMetric,
                   const std::map<std::string, std::vector<double>>& featureData,
                   const std::string& modelType = "linear_regression");
    double predictPerformance(const std::string& modelName,
                             const std::map<std::string, double>& inputFeatures) const;
    PerformancePrediction predictFuturePerformance(const std::string& metricName,
                                                   const std::string& scenario = "NORMAL",
                                                   const std::chrono::duration<double>& horizon = std::chrono::hours(24)) const;

    // Performance regression detection
    bool detectRegression(const std::string& metricName, 
                         double threshold = 0.05,
                         size_t regressionWindow = 10) const;
    std::vector<std::string> detectRegressions(const std::vector<std::string>& metricNames,
                                              double threshold = 0.05) const;

    // Performance validation
    bool validatePerformanceRequirements(const std::map<std::string, double>& requirements) const;
    double calculatePerformanceScore(const std::map<std::string, double>& metrics) const;
    std::string getPerformanceGrade(double score) const;

    // Reporting
    std::string generatePerformanceReport() const;
    std::string generateTrendReport() const;
    std::string generateCorrelationReport() const;
    std::string generateOptimizationReport() const;
    void exportAnalysisResults(const std::string& filename, const std::string& format = "json") const;

    // Configuration
    void setAnalysisWindow(size_t windowSize);
    void enableTrendAnalysis(bool enable);
    void enableCorrelationAnalysis(bool enable);
    void enableOptimizationSuggestions(bool enable);
    void setMinimumSampleSize(size_t size);
    void setConfidenceLevel(double level);

    // Time series analysis
    std::vector<TimeSeriesPoint> getTimeSeriesData(const std::string& metricName,
                                                 const std::chrono::time_point<std::chrono::system_clock>& startTime,
                                                 const std::chrono::time_point<std::system_clock>& endTime) const;
    std::map<std::string, std::vector<double>> getTimeSeriesMatrix(const std::vector<std::string>& metricNames,
                                                                  const std::chrono::time_point<std::chrono::system_clock>& startTime,
                                                                  const std::chrono::time_point<std::chrono::system_clock>& endTime) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    void cleanupOldData();
    std::vector<double> getRecentValues(const std::string& metricName, size_t windowSize) const;
    std::chrono::time_point<std::chrono::system_clock> getTimestampFromIndex(size_t index) const;
    bool validateDataQuality(const std::vector<double>& values) const;
};

} // namespace nlm
