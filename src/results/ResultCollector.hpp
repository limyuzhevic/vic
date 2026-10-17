// NLM Result Collector
// Collects and aggregates experiment results

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>
#include "experiments/Experiment.hpp"
#include "batch/BatchExecutor.hpp"

namespace nlm {

// Result metadata
struct ResultMetadata {
    std::string experimentName;
    std::string timestamp;
    double executionTime;
    std::map<std::string, std::string> parameters;
    std::string experimentType;
    
    ResultMetadata() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        timestamp = std::ctime(&time);
        timestamp.erase(timestamp.find_last_not_of("\n\r") + 1);
    }
};

// Aggregated result statistics
struct ResultStatistics {
    size_t experimentCount = 0;
    size_t successfulExperiments = 0;
    size_t failedExperiments = 0;
    double totalExecutionTime = 0.0;
    double averageExecutionTime = 0.0;
    double stddevExecutionTime = 0.0;
    std::map<std::string, double> metricStatistics;
    std::map<std::string, std::pair<double, double>> metricRanges;
    std::string generatedAt;
    
    ResultStatistics() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        generatedAt = std::ctime(&time);
        generatedAt.erase(generatedAt.find_last_not_of("\n\r") + 1);
    }
};

// Comparison result between experiments
struct ComparisonResult {
    std::string baselineExperiment;
    std::string comparisonExperiment;
    std::string metricName;
    double baselineValue;
    double comparisonValue;
    double difference;
    double percentageChange;
    std::string interpretation;
    
    ComparisonResult() {}
    ComparisonResult(const std::string& baseline, const std::string& comparison,
                    const std::string& metric, double baselineVal, double comparisonVal)
        : baselineExperiment(baseline), comparisonExperiment(comparison),
          metricName(metric), baselineValue(baselineVal), comparisonValue(comparisonVal) {
        difference = comparisonValue - baselineValue;
        percentageChange = (baselineValue != 0.0) ? (difference / baselineValue) * 100.0 : 0.0;
    }
};

// Main result collector class
class ResultCollector {
public:
    ResultCollector();
    ~ResultCollector();
    
    // Collect experiment results
    bool collectResults(const std::vector<ExperimentExecutionResult>& results);
    
    // Get all collected results
    const std::vector<ExperimentExecutionResult>& getResults() const { return collectedResults; }
    
    // Get results for specific experiment
    std::vector<ExperimentExecutionResult> getResultsForExperiment(const std::string& experimentName) const;
    
    // Get result metadata
    const std::vector<ResultMetadata>& getResultMetadata() const { return metadata; }
    
    // Get aggregated statistics
    ResultStatistics getStatistics() const { return stats; }
    
    // Get comparison results
    std::vector<ComparisonResult> getComparisonResults(const std::string& baselineExperiment,
                                                     const std::vector<std::string>& experimentNames,
                                                     const std::string& metricName) const;
    
    // Export results to file
    bool exportResults(const std::string& filename, const std::string& format = "json");
    
    // Clear collected results
    void clearResults() {
        collectedResults.clear();
        metadata.clear();
        stats = ResultStatistics();
    }
    
    // Static collector instance
    static ResultCollector& getInstance() {
        static ResultCollector instance;
        return instance;
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::vector<ExperimentExecutionResult> collectedResults;
    std::vector<ResultMetadata> metadata;
    ResultStatistics stats;
    
    // Internal processing methods
    void processCollectedResults();
    void updateStatistics();
    void generateMetadata(const ExperimentExecutionResult& result);
    
    // Calculate statistical measures
    double calculateMean(const std::vector<double>& values) const;
    double calculateStddev(const std::vector<double>& values, double mean) const;
    
    // Find metric value from experiment result
    double extractMetricValue(const ExperimentExecutionResult& result, const std::string& metricName) const;
    
    // Interpret comparison result
    std::string interpretComparison(double baseline, double comparison, const std::string& metricName) const;
    
    // Export helper methods
    bool exportToJSON(const std::string& filename);
    bool exportToCSV(const std::string& filename);
    bool exportToText(const std::string& filename);
};

} // namespace nlm