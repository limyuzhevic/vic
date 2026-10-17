// NLM Result Collector
// Implementation of result collection and aggregation

#include "ResultCollector.hpp"
#include "experiments/Phase6Experiment.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace nlm {

struct ResultCollector::Impl {
    std::string lastError;
    bool shuttingDown = false;
};

ResultCollector::ResultCollector() : pImpl(std::make_unique<Impl>()) {}
ResultCollector::~ResultCollector() = default;

bool ResultCollector::collectResults(const std::vector<ExperimentExecutionResult>& results) {
    // Clear previous results
    collectedResults.clear();
    metadata.clear();
    
    NLM_LOG_INFO("Collecting " + std::to_string(results.size()) + " experiment results");
    
    // Copy results
    collectedResults = results;
    
    // Generate metadata for each result
    for (const auto& result : results) {
        generateMetadata(result);
    }
    
    // Update statistics
    updateStatistics();
    
    NLM_LOG_INFO("Result collection completed: " + std::to_string(results.size()) +
                " results processed, " + std::to_string(stats.successfulExperiments) +
                " successful, " + std::to_string(stats.failedExperiments) + " failed");
    
    return true;
}

std::vector<ExperimentExecutionResult> ResultCollector::getResultsForExperiment(const std::string& experimentName) const {
    std::vector<ExperimentExecutionResult> filteredResults;
    
    for (const auto& result : collectedResults) {
        if (result.experimentName == experimentName) {
            filteredResults.push_back(result);
        }
    }
    
    return filteredResults;
}

std::vector<ComparisonResult> ResultCollector::getComparisonResults(
    const std::string& baselineExperiment,
    const std::vector<std::string>& experimentNames,
    const std::string& metricName) const {
    
    std::vector<ComparisonResult> comparisons;
    
    // Get baseline result
    auto baselineResults = getResultsForExperiment(baselineExperiment);
    if (baselineResults.empty()) {
        NLM_LOG_ERROR("Baseline experiment not found: " + baselineExperiment);
        return comparisons;
    }
    
    double baselineValue = extractMetricValue(baselineResults[0], metricName);
    
    // Compare with other experiments
    for (const auto& experimentName : experimentNames) {
        if (experimentName == baselineExperiment) continue;
        
        auto comparisonResults = getResultsForExperiment(experimentName);
        if (comparisonResults.empty()) {
            NLM_LOG_WARNING("Comparison experiment not found: " + experimentName);
            continue;
        }
        
        double comparisonValue = extractMetricValue(comparisonResults[0], metricName);
        
        ComparisonResult compResult(baselineExperiment, experimentName, metricName,
                                   baselineValue, comparisonValue);
        compResult.interpretation = interpretComparison(baselineValue, comparisonValue, metricName);
        
        comparisons.push_back(compResult);
    }
    
    return comparisons;
}

bool ResultCollector::exportResults(const std::string& filename, const std::string& format) {
    if (collectedResults.empty()) {
        NLM_LOG_ERROR("No results to export");
        return false;
    }
    
    NLM_LOG_INFO("Exporting " + std::to_string(collectedResults.size()) +
                " results to " + filename + " (format: " + format + ")");
    
    if (format == "json") {
        return exportToJSON(filename);
    } else if (format == "csv") {
        return exportToCSV(filename);
    } else {
        return exportToText(filename);
    }
}

void ResultCollector::processCollectedResults() {
    // Additional processing can be added here
    // For now, this is a placeholder for future enhancements
}

void ResultCollector::updateStatistics() {
    size_t total = collectedResults.size();
    if (total == 0) return;
    
    stats.experimentCount = total;
    
    // Count successful and failed experiments
    size_t successful = 0, failed = 0;
    std::vector<double> executionTimes;
    
    for (const auto& result : collectedResults) {
        if (result.success) {
            successful++;
        } else {
            failed++;
        }
        
        executionTimes.push_back(result.executionTime);
    }
    
    stats.successfulExperiments = successful;
    stats.failedExperiments = failed;
    
    // Calculate execution time statistics
    if (!executionTimes.empty()) {
        stats.totalExecutionTime = std::accumulate(executionTimes.begin(), executionTimes.end(), 0.0);
        stats.averageExecutionTime = stats.totalExecutionTime / total;
        
        if (total > 1) {
            double mean = stats.averageExecutionTime;
            double sumSqDiff = 0.0;
            for (double time : executionTimes) {
                double diff = time - mean;
                sumSqDiff += diff * diff;
            }
            stats.stddevExecutionTime = std::sqrt(sumSqDiff / (total - 1));
        }
    }
}

void ResultCollector::generateMetadata(const ExperimentExecutionResult& result) {
    ResultMetadata meta;
    meta.experimentName = result.experimentName;
    meta.executionTime = result.executionTime;
    meta.parameters = result.metrics;  // In real implementation, extract from experiment config
    meta.experimentType = "phase6";  // In real implementation, determine from experiment class
    
    metadata.push_back(meta);
}

double ResultCollector::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

double ResultCollector::calculateStddev(const std::vector<double>& values, double mean) const {
    if (values.empty()) return 0.0;
    
    double sumSqDiff = 0.0;
    for (double value : values) {
        double diff = value - mean;
        sumSqDiff += diff * diff;
    }
    
    return std::sqrt(sumSqDiff / values.size());
}

double ResultCollector::extractMetricValue(const ExperimentExecutionResult& result, const std::string& metricName) const {
    // In real implementation, this would extract actual metric values from experiment results
    // For now, return a default value based on experiment success/failure
    if (result.success) {
        return 100.0;  // Simulate success metric
    } else {
        return 0.0;   // Simulate failure metric
    }
}

std::string ResultCollector::interpretComparison(double baseline, double comparison, const std::string& metricName) const {
    if (comparison > baseline) {
        return "Improved";  // Greater is better
    } else if (comparison < baseline) {
        return "Degraded";  // Less is better (for some metrics)
    } else {
        return "No change";
    }
}

bool ResultCollector::exportToJSON(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for JSON export: " + filename);
        return false;
    }
    
    file << "{\n";
    file << "  \"results\": [\n";
    
    for (size_t i = 0; i < collectedResults.size(); ++i) {
        const auto& result = collectedResults[i];
        file << "    {\n";
        file << "      \"experiment\": \"" << result.experimentName << "\",\n";
        file << "      \"success\": " << (result.success ? "true" : "false") << ",\n";
        file << "      \"execution_time\": " << result.executionTime << ",\n";
        file << "      \"error\": \"" << result.errorMessage << "\"\n";
        file << "    }";
        
        if (i < collectedResults.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    
    file << "  ],\n";
    file << "  \"statistics\": {\n";
    file << "    \"total_experiments\": " << stats.experimentCount << ",\n";
    file << "    \"successful\": " << stats.successfulExperiments << ",\n";
    file << "    \"failed\": " << stats.failedExperiments << ",\n";
    file << "    \"avg_execution_time\": " << stats.averageExecutionTime << ",\n";
    file << "    \"stddev_execution_time\": " << stats.stddevExecutionTime << "\n";
    file << "  }\n";
    file << "}\n";
    
    file.close();
    NLM_LOG_INFO("Results exported to JSON: " + filename);
    return true;
}

bool ResultCollector::exportToCSV(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for CSV export: " + filename);
        return false;
    }
    
    file << "Experiment,Success,ExecutionTime,ErrorMessage\n";
    
    for (const auto& result : collectedResults) {
        // Escape quotes in error message
        std::string errorMsg = result.errorMessage;
        size_t pos = 0;
        while ((pos = errorMsg.find('"', pos)) != std::string::npos) {
            errorMsg.replace(pos, 1, "\"\"");
            pos += 3;
        }
        
        file << "\"" << result.experimentName << "\",";
        file << (result.success ? "true" : "false") << ",";
        file << result.executionTime << ",";
        file << "\"" << errorMsg << "\"";
        file << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Results exported to CSV: " + filename);
    return true;
}

bool ResultCollector::exportToText(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open file for text export: " + filename);
        return false;
    }
    
    file << "Experiment Results Summary\n";
    file << "==========================\n\n";
    
    file << "Total Experiments: " << stats.experimentCount << "\n";
    file << "Successful: " << stats.successfulExperiments << "\n";
    file << "Failed: " << stats.failedExperiments << "\n";
    file << "Average Execution Time: " << stats.averageExecutionTime << " seconds\n";
    file << "Standard Deviation: " << stats.stddevExecutionTime << " seconds\n\n";
    
    file << "Detailed Results:\n";
    file << "---------------\n\n";
    
    for (const auto& result : collectedResults) {
        file << "Experiment: " << result.experimentName << "\n";
        file << "  Status: " << (result.success ? "SUCCESS" : "FAILED") << "\n";
        file << "  Execution Time: " << result.executionTime << " seconds\n";
        if (!result.errorMessage.empty()) {
            file << "  Error: " << result.errorMessage << "\n";
        }
        file << "\n";
    }
    
    file.close();
    NLM_LOG_INFO("Results exported to text: " + filename);
    return true;
}

} // namespace nlm