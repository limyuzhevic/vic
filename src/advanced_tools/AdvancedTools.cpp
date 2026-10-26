#include "AdvancedTools.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <filesystem>

namespace nlm {

// CustomExperiment implementation

std::vector<CustomExperiment::TrialResult> CustomExperiment::runExperiment(
    const std::function<std::shared_ptr<Brain>(const std::map<std::string, std::string>&)>& brainFactory,
    size_t numTrials,
    const std::map<std::string, std::string>& experimentParams) {
    
    std::vector<TrialResult> results;
    
    for (size_t trialId = 0; trialId < numTrials; ++trialId) {
        TrialResult result;
        result.trialId = trialId;
        result.success = true;
        
        try {
            auto startTime = std::chrono::steady_clock::now();
            
            // Create brain using factory
            std::shared_ptr<Brain> brain = brainFactory(experimentParams);
            
            // Run experiment with brain
            result = runExperiment(brain, 1, experimentParams)[0];
            
            auto endTime = std::chrono::steady_clock::now();
            result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
            
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = e.what();
        }
        
        {
            std::lock_guard<std::mutex> lock(resultsMutex_);
            results.push_back(result);
        }
    }
    
    return results;
}

std::vector<CustomExperiment::TrialResult> CustomExperiment::runExperiment(
    std::shared_ptr<Brain> brain,
    size_t numTrials,
    const std::map<std::string, std::string>& experimentParams) {
    
    std::vector<TrialResult> results;
    
    for (size_t trialId = 0; trialId < numTrials; ++trialId) {
        TrialResult result;
        result.trialId = trialId;
        result.success = true;
        
        try {
            auto startTime = std::chrono::steady_clock::now();
            
            // Reset brain for each trial
            brain->reset();
            brain->initialize();
            
            // Run simulation
            size_t steps = std::stoi(experimentParams.at("steps"));
            double timestep = std::stod(experimentParams.at("timestep"));
            
            for (size_t step = 0; step < steps; ++step) {
                brain->step(step, step * timestep);
                
                // Record metrics
                result.metrics.push_back(static_cast<double>(brain->getTotalSpikeCount()));
                result.metrics.push_back(static_cast<double>(brain->getFiringNeuronCount()));
            }
            
            // Store metadata
            result.metadata["steps"] = std::to_string(steps);
            result.metadata["timestep"] = experimentParams.at("timestep");
            result.metadata["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
            result.metadata["firing_neurons"] = std::to_string(brain->getFiringNeuronCount());
            
            auto endTime = std::chrono::steady_clock::now();
            result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
            
            // Check success criteria
            if (config_.successCriteria) {
                result.success = config_.successCriteria(result.metrics);
            }
            
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = e.what();
        }
        
        {
            std::lock_guard<std::mutex> lock(resultsMutex_);
            results.push_back(result);
        }
    }
    
    return results;
}

bool CustomExperiment::exportResults(const std::string& filepath, const std::string& format) {
    if (format == "csv") {
        return exportResultsCSV(filepath);
    } else if (format == "json") {
        return exportResultsJSON(filepath);
    } else if (format == "matlab") {
        return exportResultsMatlab(filepath);
    }
    return false;
}

bool CustomExperiment::exportResultsJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    file << "  \"experiment\": \"" << config_.name << "\",\n";
    file << "  \"description\": \"" << config_.description << "\",\n";
    file << "  \"trials\": [\n";
    
    for (size_t i = 0; i < results_.size(); ++i) {
        file << "    {\n";
        file << "      \"trial_id\": " << results_[i].trialId << ",\n";
        file << "      \"success\": " << (results_[i].success ? "true" : "false") << ",\n";
        file << "      \"execution_time\": " << results_[i].executionTime << ",\n";
        file << "      \"metrics\": [";
        
        for (size_t j = 0; j < results_[i].metrics.size(); ++j) {
            file << results_[i].metrics[j];
            if (j < results_[i].metrics.size() - 1) file << ", ";
        }
        
        file << "],\n";
        file << "      \"metadata\": {\n";
        
        size_t metaCount = 0;
        for (const auto& meta : results_[i].metadata) {
            file << "        \"" << meta.first << "\": \"" << meta.second << "\"";
            if (++metaCount < results_[i].metadata.size()) file << ",\n";
        }
        
        file << "\n      }\n";
        
        if (i < results_.size() - 1) file << "    },\n";
        else file << "    }\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    
    return file.good();
}

bool CustomExperiment::exportResultsCSV(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    file << "trial_id,success,execution_time,total_spikes,firing_neurons,";
    file << "metric_1,metric_2,metric_3,metric_4,metric_5,\n";
    
    for (const auto& result : results_) {
        file << result.trialId << ",";
        file << (result.success ? "true" : "false") << ",";
        file << result.executionTime << ",";
        file << result.metadata.at("total_spikes") << ",";
        file << result.metadata.at("firing_neurons") << ",";
        
        for (size_t i = 0; i < std::min(result.metrics.size(), static_cast<size_t>(5)); ++i) {
            file << result.metrics[i];
            if (i < 4 && i < result.metrics.size() - 1) file << ",";
        }
        
        file << "\n";
    }
    
    return file.good();
}

bool CustomExperiment::exportResultsMatlab(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "% NLM Experiment Results\n";
    file << "% Generated on " << std::chrono::system_clock::now() << "\n\n";
    
    file << "experiment_name = \"" << config_.name << "\";\n";
    file << "description = \"" << config_.description << "\";\n\n";
    
    file << "trials = {\n";
    for (const auto& result : results_) {
        file << "  struct(trial_id=" << result.trialId << ", ";
        file << "success=" << (result.success ? "true" : "false") << ", ";
        file << "execution_time=" << result.executionTime << ", ";
        file << "total_spikes=" << result.metadata.at("total_spikes") << ", ";
        file << "firing_neurons=" << result.metadata.at("firing_neurons") << ", ";
        file << "metrics=[";
        
        for (size_t i = 0; i < result.metrics.size(); ++i) {
            file << result.metrics[i];
            if (i < result.metrics.size() - 1) file << " ";
        }
        
        file << "]);\n";
    }
    
    file << "}\n";
    
    return file.good();
}

std::map<std::string, CustomExperiment::StatisticalAnalysis> CustomExperiment::analyzeResults() const {
    std::map<std::string, StatisticalAnalysis> analyses;
    
    if (results_.empty()) return analyses;
    
    // Analyze metrics
    std::vector<std::string> metricNames = {"spikes", "firing_neurons", "execution_time"};
    
    for (const auto& metricName : metricNames) {
        std::vector<double> values;
        
        for (const auto& result : results_) {
            if (metricName == "spikes") {
                if (result.metadata.find("total_spikes") != result.metadata.end()) {
                    values.push_back(std::stod(result.metadata.at("total_spikes")));
                }
            } else if (metricName == "firing_neurons") {
                if (result.metadata.find("firing_neurons") != result.metadata.end()) {
                    values.push_back(std::stod(result.metadata.at("firing_neurons")));
                }
            } else if (metricName == "execution_time") {
                values.push_back(result.executionTime);
            }
        }
        
        if (!values.empty()) {
            StatisticalAnalysis analysis;
            analysis.mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
            
            double sqSum = std::accumulate(values.begin(), values.end(), 0.0,
                                          [](double sum, double val) { return sum + val * val; });
            analysis.stddev = std::sqrt(sqSum / values.size() - analysis.mean * analysis.mean);
            
            analysis.min = *std::min_element(values.begin(), values.end());
            analysis.max = *std::max_element(values.begin(), values.end());
            
            std::vector<double> sorted = values;
            std::sort(sorted.begin(), sorted.end());
            size_t mid = sorted.size() / 2;
            analysis.median = (sorted.size() % 2 == 0) 
                ? (sorted[mid - 1] + sorted[mid]) / 2.0 
                : sorted[mid];
            
            analysis.confidenceInterval = 1.96 * analysis.stddev / std::sqrt(values.size());
            
            analyses[metricName] = analysis;
        }
    }
    
    return analyses;
}

std::vector<CustomExperiment::HypothesisTest> CustomExperiment::runHypothesisTests() const {
    std::vector<HypothesisTest> tests;
    
    if (results_.size() < 2) return tests;
    
    // Simple t-test for execution time
    HypothesisTest test;
    test.hypothesis = "Execution times across trials are normally distributed";
    test.testType = "t-test";
    
    std::vector<double> executionTimes;
    for (const auto& result : results_) {
        executionTimes.push_back(result.executionTime);
    }
    
    double mean = std::accumulate(executionTimes.begin(), executionTimes.end(), 0.0) / executionTimes.size();
    double variance = 0.0;
    for (double time : executionTimes) {
        variance += (time - mean) * (time - mean);
    }
    variance /= executionTimes.size();
    double stddev = std::sqrt(variance);
    
    // Calculate p-value (simplified)
    double tStatistic = mean / (stddev / std::sqrt(executionTimes.size()));
    // Very rough p-value approximation
    test.pValue = 2.0 * (1.0 - 0.5 * std::abs(tStatistic));
    
    test.rejectNullHypothesis = test.pValue < 0.05;
    test.interpretation = test.pValue < 0.05 
        ? "Significant difference detected" 
        : "No significant difference detected";
    
    tests.push_back(test);
    
    return tests;
}

std::vector<CustomExperiment::ComparisonResult> CustomExperiment::compareWithBaseline(
    const std::map<std::string, std::vector<double>>& baselineResults) const {
    
    std::vector<ComparisonResult> comparisons;
    
    for (const auto& baseline : baselineResults) {
        ComparisonResult comparison;
        comparison.baselineName = baseline.first;
        comparison.baselineMetrics = baseline.second;
        
        // Calculate average metrics for this experiment
        double expMean = 0.0;
        size_t count = 0;
        
        for (const auto& result : results_) {
            if (result.metadata.find("total_spikes") != result.metadata.end()) {
                expMean += std::stod(result.metadata.at("total_spikes"));
                count++;
            }
        }
        
        if (count > 0) {
            expMean /= count;
        }
        
        // Baseline mean
        double baselineMean = std::accumulate(baseline.second.begin(), 
                                             baseline.second.end(), 0.0) / baseline.second.size();
        
        // Calculate improvement
        comparison.improvement = (expMean - baselineMean) / baselineMean * 100.0;
        
        // Determine significance (very rough)
        comparison.significant = std::abs(comparison.improvement) > 10.0; // > 10% change
        
        comparison.conclusion = comparison.significant 
            ? (comparison.improvement > 0 ? "Significant improvement" : "Significant degradation") 
            : "No significant difference";
        
        comparisons.push_back(comparison);
    }
    
    return comparisons;
}

std::string CustomExperiment::getSummary() const {
    std::ostringstream oss;
    oss << "Custom Experiment Summary\n";
    oss << "==========================\n\n";
    oss << "Experiment: " << config_.name << "\n";
    oss << "Description: " << config_.description << "\n";
    oss << "Number of trials: " << results_.size() << "\n\n";
    
    if (!results_.empty()) {
        oss << "Success rate: " << (std::count_if(results_.begin(), results_.end(),
                                               [](const TrialResult& r) { return r.success; }) / results_.size() * 100.0)
            << "%\n";
        
        double avgExecutionTime = 0.0;
        for (const auto& result : results_) {
            avgExecutionTime += result.executionTime;
        }
        avgExecutionTime /= results_.size();
        
        oss << "Average execution time: " << avgExecutionTime << " seconds\n";
    }
    
    return oss.str();
}

// Factory functions for custom experiments

std::shared_ptr<CustomExperiment> createExperiment(
    const std::string& type,
    const std::map<std::string, std::string>& config) {
    
    CustomExperiment::ExperimentConfig expConfig;
    expConfig.name = config.at("name");
    expConfig.description = config.at("description");
    expConfig.parameters = config;
    
    // Set success criteria based on type
    if (type == "learning") {
        expConfig.successCriteria = [](const std::vector<double>& metrics) {
            return metrics.size() >= 3 && metrics[0] > 100.0; // At least 100 spikes
        };
        expConfig.successThreshold = 0.8;
    } else if (type == "performance") {
        expConfig.successCriteria = [](const std::vector<double>& metrics) {
            return metrics.size() >= 2 && metrics[0] / metrics.size() > 10.0; // Average spikes > 10
        };
        expConfig.successThreshold = 0.9;
    } else {
        expConfig.successCriteria = nullptr;
        expConfig.successThreshold = 0.0;
    }
    
    return std::make_shared<CustomExperiment>(expConfig);
}

} // namespace nlm
