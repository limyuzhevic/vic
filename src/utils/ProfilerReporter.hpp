#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include "Profiler.hpp"
#include "../core/Logger.hpp"

namespace nlm {

// Performance report for different formats and analysis
struct PerformanceReport {
    std::string reportId;
    Timestamp generatedTime;
    std::string reportType; // "summary", "detailed", "timeline", "comparison"
    std::string format; // "text", "csv", "json", "html"
    
    // Performance data
    double averageCPUUsage;
    double averageGPUUsage;
    size_t averageMemoryUsage;
    std::map<std::string, double> componentMetrics;
    
    // Analysis results
    std::vector<std::string> performanceIssues;
    std::vector<std::string> optimizationRecommendations;
    std::map<std::string, double> benchmarkScores;
    
    // Metadata
    std::string sessionInfo;
    std::string configuration;
    std::vector<std::string> includedComponents;
};

// Historical performance data for trend analysis
struct HistoricalPerformanceData {
    std::string sessionId;
    Timestamp sessionStart;
    Timestamp sessionEnd;
    std::vector<PerformanceSample> samples;
    std::vector<TimeComplexityMeasure> timeComplexityMeasures;
    std::vector<PerformanceBottleneck> bottlenecks;
    std::string status; // "completed", "in_progress", "failed"
    std::map<std::string, double> finalMetrics;
};

// Performance trend analysis
struct PerformanceTrend {
    std::string metricName;
    std::vector<double> values; // Time series data
    double trendSlope; // Slope of linear regression
    double rSquared; // Coefficient of determination
    bool isImproving; // true if metric is getting better
    bool isDegrading; // true if metric is getting worse
    double predictedNextValue; // Extrapolated next value
};

// Comparative performance analysis
struct PerformanceComparison {
    std::string comparisonId;
    Timestamp comparisonTime;
    std::string baselineConfiguration;
    std::string testConfiguration;
    std::map<std::string, double> baselineMetrics;
    std::map<std::string, double> testMetrics;
    std::map<std::string, double> improvements; // test - baseline
    std::map<std::string, double> degradationRatios;
    bool isSignificantImprovement;
    bool isSignificantRegression;
};

// Performance reporter for generating reports and analysis
class ProfilerReporter {
public:
    ProfilerReporter();
    ~ProfilerReporter();

    // Report generation
    PerformanceReport generateSummaryReport(const std::string& title = "Performance Summary");
    PerformanceReport generateDetailedReport(const std::string& title = "Detailed Performance Analysis");
    PerformanceReport generateTimelineReport(const std::string& title = "Performance Timeline");
    PerformanceReport generateComparisonReport(const std::string& title = "Performance Comparison");
    
    // Report export
    bool exportReport(const PerformanceReport& report, const std::string& filename);
    bool exportReports(const std::string& outputDirectory, const std::string& pattern = "report_*.txt");
    
    // Historical data management
    void storeHistoricalData(const HistoricalPerformanceData& data);
    const HistoricalPerformanceData* getHistoricalData(const std::string& sessionId) const;
    std::vector<const HistoricalPerformanceData*> getHistoricalDataByDate(const std::string& date) const;
    
    // Performance trend analysis
    std::vector<PerformanceTrend> analyzeTrends(const std::string& metricName, 
                                                const std::vector<double>& data);
    void calculatePerformanceTrends();
    void detectPerformanceRegressions();
    
    // Performance comparison
    void addPerformanceComparison(const PerformanceComparison& comparison);
    std::vector<PerformanceComparison> getPerformanceComparisons() const;
    void generateBaselineComparisons();
    
    // Reporting configuration
    void setReportFormat(const std::string& format) { defaultFormat = format; }
    void setReportOutputDirectory(const std::string& directory) { outputDirectory = directory; }
    void setIncludeHistoricalData(bool include) { includeHistoricalData = include; }
    void setMaxHistoricalDataSize(size_t maxSize) { maxHistoricalDataSize = maxSize; }
    
    // Analysis options
    void enableTrendAnalysis(bool enable) { trendAnalysisEnabled = enable; }
    void enableRegressionDetection(bool enable) { regressionDetectionEnabled = enable; }
    void enableBenchmarkAnalysis(bool enable) { benchmarkAnalysisEnabled = enable; }
    
    // Statistics and reporting
    size_t getReportCount() const;
    size_t getHistoricalDataCount() const;
    size_t getComparisonCount() const;
    double getAveragePerformanceScore() const;
    std::string getReportStatistics() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Default configuration
    std::string defaultFormat;
    std::string outputDirectory;
    bool includeHistoricalData;
    size_t maxHistoricalDataSize;
    
    // Analysis flags
    bool trendAnalysisEnabled;
    bool regressionDetectionEnabled;
    bool benchmarkAnalysisEnabled;
};

} // namespace nlm