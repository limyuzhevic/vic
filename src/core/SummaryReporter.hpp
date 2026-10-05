// Comprehensive summary reporting for NLM
// Provides experiment summary reports, performance summaries, comparison reports, and statistical analysis

#pragma once

#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>

namespace nlm {

// Experiment result summary
struct ExperimentResultSummary {
    std::string experimentName;
    std::string experimentType;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    bool success;
    double executionTime; // in seconds
    size_t memoryUsage; // in bytes
    size_t peakMemoryUsage; // in bytes
    double throughput; // operations per second
    double efficiency; // percentage
    uint64_t operationsCompleted;
    std::vector<double> performanceMetrics;
    std::vector<std::pair<std::string, double>> metrics;
    std::string status; // "completed", "failed", "running", "aborted"
    std::string notes;
    
    ExperimentResultSummary() : success(false), executionTime(0.0), memoryUsage(0),
                                peakMemoryUsage(0), throughput(0.0), efficiency(0.0),
                                operationsCompleted(0) {}
};

// Performance summary
struct PerformanceSummary {
    std::string benchmarkName;
    std::chrono::system_clock::time_point timestamp;
    double avgExecutionTime; // in seconds
    double avgCPUUsage; // percentage 0.0 to 100.0
    double avgMemoryUsage; // in MB
    double avgThroughput; // operations per second
    double avgLatency; // in milliseconds
    double stdDevExecutionTime; // standard deviation
    double stdDevMemoryUsage; // standard deviation
    double reliability; // 0.0 to 1.0
    std::vector<double> executionTimes;
    std::vector<double> memoryUsages;
    
    PerformanceSummary() : avgExecutionTime(0.0), avgCPUUsage(0.0), avgMemoryUsage(0.0),
                           avgThroughput(0.0), avgLatency(0.0), stdDevExecutionTime(0.0),
                           stdDevMemoryUsage(0.0), reliability(0.0) {}
};

// Statistical analysis results
struct StatisticalAnalysis {
    std::string analysisType;
    std::vector<double> data;
    double mean;
    double median;
    double mode;
    double standardDeviation;
    double variance;
    double minValue;
    double maxValue;
    double range;
    double skewness;
    double kurtosis;
    std::chrono::system_clock::time_point timestamp;
    
    StatisticalAnalysis() : mean(0.0), median(0.0), mode(0.0), standardDeviation(0.0),
                           variance(0.0), minValue(0.0), maxValue(0.0), range(0.0),
                           skewness(0.0), kurtosis(0.0) {}
};

// Comparison results for multiple runs
struct ComparisonResult {
    std::string comparisonName;
    std::vector<ExperimentResultSummary> runs;
    std::map<std::string, StatisticalAnalysis> statistics;
    std::vector<std::pair<std::string, double>> improvements;
    std::vector<std::pair<std::string, double>> degradations;
    std::chrono::system_clock::time_point timestamp;
    
    ComparisonResult() : timestamp(std::chrono::system_clock::now()) {}
};

// Export configuration
struct ExportConfig {
    bool includeHeaders;
    bool includeTimestamps;
    bool includeStatistics;
    bool includeRawData;
    bool compactFormat;
    std::string dateFormat;
    std::string timeFormat;
    std::string decimalFormat;
    
    ExportConfig() : includeHeaders(true), includeTimestamps(true), includeStatistics(true),
                    includeRawData(false), compactFormat(false), dateFormat("%Y-%m-%d"),
                    timeFormat("%H:%M:%S"), decimalFormat("%.2f") {}
};

class SummaryReporter {
public:
    SummaryReporter();
    ~SummaryReporter();
    
    // Configure summary reporting
    void setUseColors(bool useColors);
    void setIncludeStatistics(bool includeStatistics);
    void setIncludeRawData(bool includeRawData);
    void setExportConfig(const ExportConfig& config);
    void setIncludeBrainData(bool include);
    void setIncludePerformanceData(bool include);
    void setIncludeExperimentData(bool include);
    
    // Experiment summary reporting
    std::string generateExperimentSummary(const ExperimentResultSummary& result) const;
    std::string generateExperimentComparison(const ComparisonResult& comparison) const;
    std::string generateExperimentStatistics(const StatisticalAnalysis& stats) const;
    
    // Performance summary reporting
    std::string generatePerformanceSummary(const PerformanceSummary& summary) const;
    std::string generatePerformanceReport(const std::vector<PerformanceSummary>& summaries) const;
    
    // Statistical analysis reporting
    std::string generateStatisticalAnalysisReport(const StatisticalAnalysis& analysis) const;
    std::string generateStatisticalAnalysisSummary(const std::vector<StatisticalAnalysis>& analyses) const;
    
    // Comparison reporting
    std::string generateComparisonReport(const ComparisonResult& comparison) const;
    std::string generatePerformanceComparison(const std::vector<PerformanceSummary>& summaries) const;
    
    // Comprehensive reports
    std::string generateComprehensiveReport(const std::vector<ExperimentResultSummary>& experiments,
                                            const std::vector<PerformanceSummary>& performances,
                                            const std::vector<StatisticalAnalysis>& statistics) const;
    
    // Statistical calculations
    StatisticalAnalysis calculateStatistics(const std::vector<double>& data) const;
    std::string formatStatisticalResults(const StatisticalAnalysis& stats) const;
    
    // Export functionality
    bool exportExperimentSummary(const std::string& filepath, const ExperimentResultSummary& result,
                                const std::string& format = "text") const;
    bool exportPerformanceSummary(const std::string& filepath, const PerformanceSummary& summary,
                                 const std::string& format = "text") const;
    bool exportStatisticalAnalysis(const std::string& filepath, const StatisticalAnalysis& analysis,
                                  const std::string& format = "text") const;
    bool exportComparisonReport(const std::string& filepath, const ComparisonResult& comparison,
                               const std::string& format = "text") const;
    
    // Report generation helpers
    std::string formatExperimentResult(const ExperimentResultSummary& result) const;
    std::string formatPerformanceResult(const PerformanceSummary& summary) const;
    std::string formatAnalysisResult(const StatisticalAnalysis& analysis) const;
    
    // Data management
    void addExperimentResult(const ExperimentResultSummary& result);
    void addPerformanceResult(const PerformanceSummary& result);
    void addStatisticalAnalysis(const StatisticalAnalysis& analysis);
    void clearExperimentResults();
    void clearPerformanceResults();
    void clearStatisticalAnalyses();
    
    // Report templates
    static std::string getExperimentReportTemplate();
    static std::string getPerformanceReportTemplate();
    static std::string getAnalysisReportTemplate();
    static std::string getComparisonReportTemplate();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    std::string formatTimestamp(std::chrono::system_clock::time_point timestamp) const;
    std::string formatDuration(double seconds) const;
    std::string formatBytes(size_t bytes) const;
    std::string formatPercentage(double value) const;
    std::string formatValue(double value, const std::string& format) const;
    
    // Statistical calculations
    double calculateMean(const std::vector<double>& data) const;
    double calculateMedian(const std::vector<double>& data) const;
    double calculateMode(const std::vector<double>& data) const;
    double calculateStandardDeviation(const std::vector<double>& data, double mean) const;
    double calculateVariance(const std::vector<double>& data, double mean) const;
    double calculateMin(const std::vector<double>& data) const;
    double calculateMax(const std::vector<double>& data) const;
    double calculateRange(const std::vector<double>& data) const;
    double calculateSkewness(const std::vector<double>& data, double mean, double stdDev) const;
    double calculateKurtosis(const std::vector<double>& data, double mean, double stdDev) const;
};

} // namespace nlm
