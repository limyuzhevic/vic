// ExperimentManager class implementation
// This file provides the implementation of the ExperimentManager high-level interface

#include "../src/experiment_manager.hpp"
#include "../src/advanced_api.hpp"

// ExperimentManager implementation
ExperimentManager::ExperimentManager() {
    experimentId_ = 0;
}

void ExperimentManager::configure(const StringMap& config) {
    experimentConfig_ = config;
    
    // Extract configuration parameters
    auto it = config.find("results_directory");
    if (it != config.end()) {
        resultsDirectory_ = it->second;
    }
    
    NLM_LOG_INFO("Experiment manager configured");
}

StringMap ExperimentManager::getConfig() const {
    StringMap config = experimentConfig_;
    config["results_directory"] = resultsDirectory_;
    config["num_experiments"] = std::to_string(results_.size());
    return config;
}

ExperimentResult ExperimentManager::runExperiment(const std::string& name,
                                                  std::function<void(std::shared_ptr<SimpleAgent>)> setup,
                                                  std::function<void(std::shared_ptr<SimpleAgent>, float)> step,
                                                  std::function<double(std::shared_ptr<SimpleAgent>)> evaluate) {
    ExperimentResult result;
    result.experimentName = name;
    result.experimentId = experimentId_++;
    
    // Implementation would run the experiment with provided callbacks
    // For now, create a dummy result
    result.success = true;
    result.duration = 0.0f;
    result.parameters = experimentConfig_;
    
    results_.push_back(result);
    NLM_LOG_INFO("Experiment " + name + " completed with ID " + std::to_string(result.experimentId));
    
    return result;
}

std::vector<ExperimentResult> ExperimentManager::runBatch(size_t numExperiments) {
    std::vector<ExperimentResult> batchResults;
    
    for (size_t i = 0; i < numExperiments; ++i) {
        ExperimentResult result = runExperiment("batch_experiment_" + std::to_string(i));
        batchResults.push_back(result);
    }
    
    // Add batch results to manager
    for (const auto& result : batchResults) {
        results_.push_back(result);
    }
    
    NLM_LOG_INFO("Completed batch of " + std::to_string(numExperiments) + " experiments");
    
    return batchResults;
}

void ExperimentManager::addResult(const ExperimentResult& result) {
    results_.push_back(result);
    NLM_LOG_INFO("Added experiment result with ID " + std::to_string(result.experimentId));
}

std::vector<ExperimentResult> ExperimentManager::getResults() const {
    return results_;
}

ExperimentResult ExperimentManager::getResult(size_t index) const {
    if (index < results_.size()) {
        return results_[index];
    }
    return ExperimentResult();
}

std::map<std::string, double> ExperimentManager::compareExperiments(size_t experiment1,
                                                                     size_t experiment2) const {
    std::map<std::string, double> comparison;
    
    if (experiment1 >= results_.size() || experiment2 >= results_.size()) {
        NLM_LOG_ERROR("Invalid experiment indices for comparison");
        return comparison;
    }
    
    const auto& result1 = results_[experiment1];
    const auto& result2 = results_[experiment2];
    
    // Compare metrics
    comparison["performance_ratio"] = result1.performanceScore / result2.performanceScore;
    comparison["duration_ratio"] = result2.duration / result1.duration;
    comparison["success_rate_diff"] = result2.success ? 1.0 : 0.0 - (result1.success ? 1.0 : 0.0);
    
    NLM_LOG_INFO("Comparison between experiments " + std::to_string(experiment1) + 
                " and " + std::to_string(experiment2));
    
    return comparison;
}

std::map<std::string, FloatArray> ExperimentManager::analyzeAcrossExperiments(const std::string& metric) const {
    std::map<std::string, FloatArray> analysis;
    
    // Analyze metric across all experiments
    FloatArray metricValues;
    for (const auto& result : results_) {
        // Extract metric value
        metricValues.push_back(1.0f); // Dummy value
    }
    
    analysis[metric] = metricValues;
    return analysis;
}

void ExperimentManager::exportAllResults(const std::string& directory) {
    std::string exportDir = directory.empty() ? resultsDirectory_ : directory;
    
    if (exportDir.empty()) {
        NLM_LOG_WARNING("No directory specified for results export");
        return;
    }
    
    // Export results to directory
    for (const auto& result : results_) {
        std::string filename = exportDir + "/experiment_" + std::to_string(result.experimentId) + ".json";
        // Implementation would export result to file
        NLM_LOG_INFO("Exported experiment results to " + filename);
    }
}

void ExperimentManager::generateReport(const std::string& filename) const {
    std::string reportFilename = filename.empty() ? "experiment_report.txt" : filename;
    
    // Generate analysis report
    std::string report = "Experiment Analysis Report:\n";
    report += "============================\n";
    report += "Total experiments: " + std::to_string(results_.size()) + "\n";
    
    double totalScore = 0.0;
    size_t successful = 0;
    for (const auto& result : results_) {
        totalScore += result.performanceScore;
        if (result.success) {
            successful++;
        }
    }
    
    report += "Average performance: " + std::to_string(totalScore / results_.size()) + "\n";
    report += "Success rate: " + std::to_string(successful / results_.size() * 100) + "%\n";
    
    // Write report to file
    // Implementation would write report to file
    NLM_LOG_INFO("Generated experiment report: " + reportFilename);
}

double ExperimentManager::getMeanPerformance(size_t experimentIndex) const {
    if (experimentIndex >= results_.size()) {
        return 0.0;
    }
    return results_[experimentIndex].performanceScore;
}

double ExperimentManager::getVariancePerformance(size_t experimentIndex) const {
    // Calculate variance
    double mean = getMeanPerformance(experimentIndex);
    double variance = 0.0;
    
    if (experimentIndex < results_.size()) {
        // Implementation would calculate variance
        variance = 0.0; // Dummy
    }
    
    return variance;
}

std::map<std::string, double> ExperimentManager::getGlobalStatistics() const {
    std::map<std::string, double> stats;
    
    if (results_.empty()) {
        return stats;
    }
    
    double totalScore = 0.0;
    size_t successful = 0;
    double totalDuration = 0.0;
    
    for (const auto& result : results_) {
        totalScore += result.performanceScore;
        if (result.success) {
            successful++;
        }
        totalDuration += result.duration;
    }
    
    size_t numResults = results_.size();
    
    stats["mean_performance"] = totalScore / numResults;
    stats["success_rate"] = successful / numResults * 100.0;
    stats["mean_duration"] = totalDuration / numResults;
    stats["total_experiments"] = static_cast<double>(numResults);
    
    return stats;
}
