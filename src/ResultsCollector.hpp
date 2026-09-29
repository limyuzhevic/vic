#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <algorithm>
#include <functional>

namespace nlm {

// Results processing and reporting
class ResultsCollector {
public:
    ResultsCollector();
    ~ResultsCollector();
    
    // Disable copying
    ResultsCollector(const ResultsCollector&) = delete;
    ResultsCollector& operator=(const ResultsCollector&) = delete;
    
    // Result data structure
    struct ResultEntry {
        std::string experimentName;
        std::string metricName;
        double value;
        double baseline;
        double threshold;
        std::string unit;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, std::string> metadata;
        
        ResultEntry() : value(0.0), baseline(0.0), threshold(0.0), timestamp(std::chrono::system_clock::now()) {}
        ResultEntry(const std::string& expName, const std::string& metric, double val,
                   const std::string& unit = "", const std::map<std::string, std::string>& meta = {})
            : experimentName(expName), metricName(metric), value(val), baseline(0.0), 
              threshold(0.0), unit(unit), timestamp(std::chrono::system_clock::now()),
              metadata(meta) {}
    };
    
    // Result analysis
    struct ResultAnalysis {
        std::string metricName;
        double mean;
        double median;
        double stddev;
        double min;
        double max;
        double percentile95;
        double percentile99;
        size_t count;
        std::chrono::system_clock::time_point analysisTime;
        
        ResultAnalysis()
            : mean(0.0), median(0.0), stddev(0.0), min(0.0), max(0.0),
              percentile95(0.0), percentile99(0.0), count(0),
              analysisTime(std::chrono::system_clock::now()) {}
    };
    
    // Result comparison
    struct ResultComparison {
        std::string metricName;
        double value1;
        double value2;
        double difference;
        double percentageChange;
        std::string interpretation;
        
        ResultComparison() : value1(0.0), value2(0.0), difference(0.0), percentageChange(0.0) {}
        ResultComparison(const std::string& metric, double v1, double v2)
            : metricName(metric), value1(v1), value2(v2),
              difference(v2 - v1), percentageChange(v1 != 0.0 ? ((v2 - v1) / v1) * 100.0 : 0.0) {}
    };
    
    // Result significance
    enum class SignificanceLevel {
        NOT_SIGNIFICANT,
        MARGINALLY_SIGNIFICANT,
        SIGNIFICANT,
        HIGHLY_SIGNIFICANT,
        EXTREMELY_SIGNIFICANT
    };
    
    // Add results
    bool addResult(const ResultEntry& result);
    bool addResult(const std::string& experimentName, const std::string& metricName,
                  double value, const std::string& unit = "", const std::map<std::string, std::string>& metadata = {});
    
    // Add multiple results
    bool addResults(const std::vector<ResultEntry>& results);
    
    // Get results
    std::vector<ResultEntry> getResults() const;
    std::vector<ResultEntry> getResults(const std::string& experimentName) const;
    std::vector<ResultEntry> getResults(const std::string& experimentName, const std::string& metricName) const;
    ResultEntry getResult(size_t index) const;
    
    // Result filtering
    std::vector<ResultEntry> getResultsByExperiment(const std::string& experimentName) const;
    std::vector<ResultEntry> getResultsByMetric(const std::string& metricName) const;
    std::vector<ResultEntry> getResultsByTimeRange(const std::chrono::system_clock::time_point& start,
                                                  const std::chrono::system_clock::time_point& end) const;
    
    // Result analysis
    ResultAnalysis analyzeResults(const std::string& metricName, const std::vector<ResultEntry>& results) const;
    std::map<std::string, ResultAnalysis> analyzeAllResults() const;
    
    // Result comparison
    ResultComparison compareResults(const std::vector<ResultEntry>& baselineResults,
                                   const std::vector<ResultEntry>& testResults) const;
    std::vector<ResultComparison> compareAllResults(const std::vector<ResultEntry>& baselineResults,
                                                   const std::vector<ResultEntry>& testResults) const;
    
    // Result significance
    SignificanceLevel determineSignificance(double pValue, double alpha = 0.05) const;
    bool isSignificant(double pValue, double alpha = 0.05) const;
    
    // Result interpretation
    std::string interpretResult(const ResultEntry& result, const std::string& context = "") const;
    std::vector<std::string> interpretAllResults(const std::vector<ResultEntry>& results) const;
    
    // Result reporting
    std::string generateReport(const std::vector<ResultEntry>& results) const;
    std::string generateDetailedReport(const std::vector<ResultEntry>& results) const;
    std::string generateStatisticalReport(const std::vector<ResultEntry>& results) const;
    
    // Result export
    bool exportResults(const std::string& filename, const std::vector<ResultEntry>& results) const;
    bool exportResultsJSON(const std::string& filename, const std::vector<ResultEntry>& results) const;
    bool exportResultsCSV(const std::string& filename, const std::vector<ResultEntry>& results) const;
    bool exportResultsXML(const std::string& filename, const std::vector<ResultEntry>& results) const;
    
    // Result import
    bool importResults(const std::string& filename, std::vector<ResultEntry>& results);
    
    // Result validation
    bool validateResult(const ResultEntry& result) const;
    std::vector<std::string> validateAllResults() const;
    
    // Result clearing
    void clearResults();
    
    // Result statistics
    struct ResultsStatistics {
        size_t totalResults;
        size_t uniqueExperiments;
        size_t uniqueMetrics;
        std::chrono::system_clock::time_point earliestResult;
        std::chrono::system_clock::time_point latestResult;
        double averageValue;
        double medianValue;
        double stddevValue;
        double minValue;
        double maxValue;
        
        ResultsStatistics()
            : totalResults(0), uniqueExperiments(0), uniqueMetrics(0),
              averageValue(0.0), medianValue(0.0), stddevValue(0.0),
              minValue(0.0), maxValue(0.0) {}
    };
    
    ResultsStatistics getStatistics() const;
    
    // Result trend analysis
    std::map<std::string, std::vector<double>> getTrends(const std::string& metricName) const;
    double calculateTrend(const std::vector<ResultEntry>& results, const std::string& metricName) const;
    
    // Result prediction
    double predictResult(const std::vector<ResultEntry>& history, const std::string& metricName) const;
    
    // Result validation thresholds
    struct ValidationThresholds {
        double minValue;
        double maxValue;
        double warningMin;
        double warningMax;
        double criticalMin;
        double criticalMax;
        
        ValidationThresholds()
            : minValue(std::numeric_limits<double>::lowest()), maxValue(std::numeric_limits<double>::max()),
              warningMin(std::numeric_limits<double>::lowest()), warningMax(std::numeric_limits<double>::max()),
              criticalMin(std::numeric_limits<double>::lowest()), criticalMax(std::numeric_limits<double>::max()) {}
    };
    
    void setValidationThresholds(const std::string& metricName, const ValidationThresholds& thresholds);
    ValidationThresholds getValidationThresholds(const std::string& metricName) const;
    
    // Result validation
    bool validateResult(const ResultEntry& result) const;
    std::vector<std::string> getValidationErrors(const ResultEntry& result) const;
    
    // Result quality assessment
    double assessResultQuality(const ResultEntry& result) const;
    std::string assessResultQuality(const std::vector<ResultEntry>& results) const;
    
    // Result confidence interval
    struct ConfidenceInterval {
        double lower;
        double upper;
        double confidenceLevel;
        
        ConfidenceInterval() : lower(0.0), upper(0.0), confidenceLevel(0.95) {}
        ConfidenceInterval(double l, double u, double c = 0.95)
            : lower(l), upper(u), confidenceLevel(c) {}
    };
    
    ConfidenceInterval calculateConfidenceInterval(const std::vector<ResultEntry>& results, double confidenceLevel = 0.95) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Result calculation helpers
    void updateStatistics();
    double calculateMean(const std::vector<double>& values) const;
    double calculateStddev(const std::vector<double>& values, double mean) const;
    double calculateMedian(const std::vector<double>& values) const;
    
    // Result sorting helpers
    static bool sortResultsByValue(const ResultEntry& a, const ResultEntry& b);
    static bool sortResultsByTimestamp(const ResultEntry& a, const ResultEntry& b);
    
    // Result export helpers
    std::string resultEntryToJSON(const ResultEntry& result) const;
    std::string resultEntryToCSV(const ResultEntry& result) const;
    
    // Result import helpers
    bool parseResultFromJSON(const std::string& json, ResultEntry& result);
    bool parseResultFromCSV(const std::string& csv, ResultEntry& result);
    
    // Result validation helpers
    bool validateResultValue(const ResultEntry& result) const;
    bool validateResultMetadata(const ResultEntry& result) const;
    
    // Result quality helpers
    double calculateQualityScore(const ResultEntry& result) const;
    
    // Result trend helpers
    std::vector<double> extractValues(const std::vector<ResultEntry>& results) const;
    
    // Result prediction helpers
    double calculateTrend(const std::vector<double>& values) const;
}; // namespace nlm

// Global results collector instance
ResultsCollector& getResultsCollector();