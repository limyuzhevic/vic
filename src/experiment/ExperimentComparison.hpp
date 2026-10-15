#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include "ExperimentProtocol.hpp"
#include "ExperimentResults.hpp"
#include "ExperimentMetrics.hpp"
#include "ExperimentStatistics.hpp"

namespace nlm {

// Comparison types
enum class ComparisonType {
    SimpleComparison,
    RankedComparison,
    RatioComparison,
    DifferenceComparison,
    PercentDifference,
    EffectSize,
    StandardizedDifference,
    RelativeRisk,
    OddsRatio,
    ChiSquareTest,
    TTest,
    ANOVA,
    CustomComparison
};

// Comparison result
struct ComparisonResult {
    std::string comparisonId;
    std::string experimentName1;
    std::string experimentName2;
    ComparisonType comparisonType;
    std::string metricName;
    double value1;
    double value2;
    double difference;
    double ratio;
    double percentageDifference;
    double effectSize;
    double standardizedDifference;
    std::chrono::system_clock::time_point comparisonTime;
    std::string comparisonMethod;
    std::map<std::string, double> comparisonParameters;
    std::map<std::string, std::string> comparisonMetadata;
    
    ComparisonResult() : comparisonTime(std::chrono::system_clock::now()) {}
    ComparisonResult(const std::string& id, const std::string& name1, const std::string& name2, ComparisonType type)
        : comparisonId(id), experimentName1(name1), experimentName2(name2), comparisonType(type),
          comparisonTime(std::chrono::system_clock::now()) {}
};

// Comparative analysis
struct ComparativeAnalysis {
    std::string analysisId;
    std::string experimentName;
    std::string analysisType; // "time_series", "cross_sectional", "longitudinal", "meta_analysis"
    std::chrono::system_clock::time_point analysisTime;
    std::vector<ComparisonResult> comparisonResults;
    std::map<std::string, double> aggregatedResults;
    std::map<std::string, double> trendAnalysis;
    std::map<std::string, std::vector<double>> timeSeriesData;
    std::map<std::string, std::string> analysisMetadata;
    
    ComparativeAnalysis() : analysisTime(std::chrono::system_clock::now()) {}
    ComparativeAnalysis(const std::string& id, const std::string& expName, const std::string& type)
        : analysisId(id), experimentName(expName), analysisType(type), analysisTime(std::chrono::system_clock::now()) {}
};

class ExperimentComparison {
public:
    ExperimentComparison();
    ~ExperimentComparison();
    
    // Disable copying, enable moving
    ExperimentComparison(const ExperimentComparison&) = delete;
    ExperimentComparison& operator=(const ExperimentComparison&) = delete;
    ExperimentComparison(ExperimentComparison&&) noexcept;
    ExperimentComparison& operator=(ExperimentComparison&&) noexcept;
    
    // Simple comparisons
    ComparisonResult compareExperiments(const std::string& experimentName1, const std::string& experimentName2) const;
    ComparisonResult compareMetrics(const std::string& experimentName, const std::string& metricName1, const std::string& metricName2) const;
    ComparisonResult compareResults(const std::string& experimentName1, const std::string& experimentName2, const std::string& metricName) const;
    
    // Advanced comparisons
    ComparisonResult compareByRank(const std::string& experimentName1, const std::string& experimentName2, const std::string& metricName) const;
    ComparisonResult compareByRatio(const std::string& experimentName1, const std::string& experimentName2, const std::string& metricName) const;
    ComparisonResult compareByDifference(const std::string& experimentName1, const std::string& experimentName2, const std::string& metricName) const;
    ComparisonResult compareByPercentage(const std::string& experimentName1, const std::string& experimentName2, const std::string& metricName) const;
    
    // Statistical comparisons
    ComparisonResult compareEffectSize(const std::string& experimentName1, const std::string& experimentName2, const std::string& metricName) const;
    ComparisonResult compareStandardizedDifference(const std::string& experimentName1, const std::string& experimentName2, const std::string& metricName) const;
    ComparisonResult compareRelativeRisk(const std::string& experimentName1, const std::string& experimentName2, const std::string& metricName) const;
    ComparisonResult compareOddsRatio(const std::string& experimentName1, const std::string& experimentName2, const std::string& metricName) const;
    
    // Statistical tests
    ComparisonResult performChiSquareTest(const std::string& experimentName1, const std::string& experimentName2, const std::string& metricName) const;
    ComparisonResult performTTest(const std::string& experimentName1, const std::string& experimentName2, const std::string& metricName) const;
    ComparisonResult performANOVATest(const std::vector<std::string>& experimentNames, const std::string& metricName) const;
    
    // Comparative analysis
    ComparativeAnalysis performComparativeAnalysis(const std::string& experimentName, const std::string& analysisType) const;
    ComparativeAnalysis performTimeSeriesComparison(const std::string& experimentName, const std::string& metricName) const;
    ComparativeAnalysis performCrossSectionalComparison(const std::vector<std::string>& experimentNames, const std::string& metricName) const;
    ComparativeAnalysis performLongitudinalComparison(const std::string& experimentName, const std::vector<std::chrono::system_clock::time_point>& timePoints, const std::string& metricName) const;
    
    // Comparison aggregation
    std::map<std::string, double> aggregateComparisonResults(const std::vector<ComparisonResult>& comparisonResults) const;
    std::map<std::string, double> calculateTrendAnalysis(const std::vector<ComparisonResult>& comparisonResults) const;
    std::map<std::string, std::vector<double>> extractTimeSeriesData(const ComparativeAnalysis& analysis) const;
    
    // Comparison visualization
    bool generateComparisonChart(const ComparisonResult& comparison, const std::string& outputPath) const;
    bool generateComparisonTable(const std::vector<ComparisonResult>& comparisons, const std::string& outputPath) const;
    bool generateComparisonBoxPlot(const std::vector<ComparisonResult>& comparisons, const std::string& outputPath) const;
    bool generateComparisonHistogram(const std::vector<ComparisonResult>& comparisons, const std::string& outputPath) const;
    
    // Comparison export
    bool exportComparisonResults(const std::vector<ComparisonResult>& comparisons, const std::string& filepath) const;
    bool exportComparisonAnalysis(const ComparativeAnalysis& analysis, const std::string& filepath) const;
    bool exportComparisonToDatabase(const std::vector<ComparisonResult>& comparisons, const std::string& connectionString, const std::string& tableName) const;
    
    // Comparison import
    bool importComparisonResults(const std::string& filepath);
    bool importComparisonAnalysis(const std::string& filepath);
    bool importComparisonFromDatabase(const std::string& connectionString, const std::string& tableName);
    
    // Comparison management
    bool addComparisonResult(const ComparisonResult& comparison);
    bool removeComparisonResult(const std::string& comparisonId);
    bool updateComparisonResult(const ComparisonResult& comparison);
    ComparisonResult getComparisonResult(const std::string& comparisonId) const;
    std::vector<ComparisonResult> getAllComparisonResults() const;
    std::vector<ComparisonResult> getComparisonResultsForExperiments(const std::string& experimentName1, const std::string& experimentName2) const;
    
    // Comparison analysis
    double calculateComparisonSignificance(const ComparisonResult& comparison) const;
    std::string interpretComparisonResult(const ComparisonResult& comparison) const;
    bool validateComparisonResult(const ComparisonResult& comparison) const;
    
    // Comparison optimization
    bool optimizeComparisonAlgorithms();
    bool enableComparisonCaching(bool enable);
    bool setComparisonPrecision(int precision);
    bool enableComparisonValidation(bool enable);
    
    // Comparison cleanup
    bool cleanupComparisonResults(const std::string& experimentName1, const std::string& experimentName2);
    bool cleanupOldComparisonResults(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetComparisonAnalysis(const std::string& experimentName1, const std::string& experimentName2);
    bool clearAllComparisonResults();
    std::vector<std::string> getComparisonExperiments() const;
    size_t getComparisonCount() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm