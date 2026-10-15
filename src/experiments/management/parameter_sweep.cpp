#include "parameter_sweep.hpp"
#include "experiment_factory.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// ParameterSweepManager implementation
std::vector<SweepResult> ParameterSweepManager::runSweep(const std::string& templateName,
                                                         const std::map<std::string, std::vector<std::string>>& parameterMatrix,
                                                         size_t numIterations) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<SweepResult> results;
    
    // Generate all parameter combinations
    std::vector<std::map<std::string, std::string>> parameterCombinations;
    generateParameterCombinations(parameterMatrix, parameterCombinations, {}, 0);
    
    // Limit iterations
    if (numIterations > 0 && numIterations < parameterCombinations.size()) {
        parameterCombinations.resize(numIterations);
    }
    
    // Run sweep for each parameter combination
    size_t experimentId = 0;
    ExperimentFactory factory(config_);
    
    for (const auto& params : parameterCombinations) {
        SweepResult result;
        result.parameterName = templateName;
        result.experiment = factory.createExperimentFromTemplate(templateName, params);
        
        if (result.experiment) {
            std::stringstream paramStream;
            bool first = true;
            for (const auto& param : params) {
                if (!first) paramStream << ",";
                paramStream << param.first << "=" << param.second;
                first = false;
            }
            result.parameterValue = paramStream.str();
            
            // Set experiment ID
            result.experiment->setParameter("experiment_id", std::to_string(experimentId++));
            
            // Initialize and run experiment
            if (result.experiment->initialize()) {
                auto startTime = std::chrono::high_resolution_clock::now();
                bool success = result.experiment->run();
                auto endTime = std::chrono::high_resolution_clock::now();
                
                result.executionTime = endTime - startTime;
                result.success = success;
                result.errorMessage = success ? "" : "Experiment failed";
                
                if (success) {
                    result.experiment->finalize();
                    result.metricValue = result.experiment->calculatePerformanceScore();
                }
            }
        }
        
        results.push_back(result);
    }
    
    // Store results
    sweepResults_[templateName] = results;
    
    return results;
}

OptimizationResult ParameterSweepManager::optimizeParameters(const std::string& templateName,
                                                             const std::map<std::string, std::vector<std::string>>& parameterSpace,
                                                             const std::string& objective) {
    OptimizationResult result;
    result.optimizationGoal = objective;
    
    // Run parameter sweep
    std::vector<SweepResult> sweepResults = runSweep(templateName, parameterSpace, 0); // 0 means use all combinations
    
    // Analyze results
    std::vector<std::pair<std::string, double>> performanceValues;
    std::vector<std::shared_ptr<BaseExperiment>> experiments;
    
    for (const auto& sweepResult : sweepResults) {
        if (sweepResult.success) {
            performanceValues.emplace_back(sweepResult.parameterValue, sweepResult.metricValue);
            experiments.push_back(sweepResult.experiment);
        }
    }
    
    // Select Pareto front (for multi-objective optimization)
    if (objective == "multi_objective") {
        // Simple Pareto front selection - would need more sophisticated implementation
        for (size_t i = 0; i < std::min<size_t>(3, performanceValues.size()); ++i) {
            result.paretoFront.push_back(performanceValues[i]);
            result.experiments.push_back(experiments[i]);
        }
    } else {
        // Single objective optimization - find best parameter
        if (!performanceValues.empty()) {
            auto bestIt = std::max_element(performanceValues.begin(), performanceValues.end(),
                                         [](const auto& a, const auto& b) { return a.second < b.second; });
            result.paretoFront.push_back(*bestIt);
            if (bestIt->first < experiments.size()) {
                result.experiments.push_back(experiments[bestIt->first]);
            }
        }
    }
    
    // Calculate convergence metric
    if (!performanceValues.empty()) {
        double sum = std::accumulate(performanceValues.begin(), performanceValues.end(), 0.0,
                                   [](double acc, const auto& p) { return acc + p.second; });
        double mean = sum / performanceValues.size();
        double variance = 0.0;
        for (const auto& p : performanceValues) {
            variance += (p.second - mean) * (p.second - mean);
        }
        variance /= performanceValues.size();
        result.convergenceMetric = std::sqrt(variance);
    }
    
    return result;
}

const std::vector<SweepResult>& ParameterSweepManager::getSweepResults(const std::string& experimentName) const {
    auto it = sweepResults_.find(experimentName);
    if (it != sweepResults_.end()) {
        return it->second;
    }
    static const std::vector<SweepResult> emptyResults;
    return emptyResults;
}

bool ParameterSweepManager::saveSweepResults(const std::string& experimentName, const std::string& filepath) const {
    auto it = sweepResults_.find(experimentName);
    if (it == sweepResults_.end()) {
        return false;
    }
    
    try {
        json j;
        j["experiment_name"] = experimentName;
        j["timestamp"] = ExperimentUtils::getCurrentTimestamp();
        j["results"] = json::array();
        
        for (const auto& result : it->second) {
            json resultJson;
            resultJson["parameter_value"] = result.parameterValue;
            resultJson["metric_value"] = result.metricValue;
            resultJson["execution_time"] = result.executionTime.count();
            resultJson["success"] = result.success;
            resultJson["error_message"] = result.errorMessage;
            
            j["results"].push_back(resultJson);
        }
        
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << j.dump(2);
            file.close();
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error saving sweep results: " << e.what() << std::endl;
    }
    
    return false;
}

bool ParameterSweepManager::loadSweepResults(const std::string& experimentName, const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json j;
        file >> j;
        
        std::vector<SweepResult> results;
        
        for (const auto& resultJson : j["results"]) {
            SweepResult result;
            result.parameterName = experimentName;
            result.parameterValue = resultJson["parameter_value"].get<std::string>();
            result.metricValue = resultJson["metric_value"].get<double>();
            result.executionTime = std::chrono::duration<double>(resultJson["execution_time"].get<double>());
            result.success = resultJson["success"].get<bool>();
            result.errorMessage = resultJson["error_message"].get<std::string>();
            
            results.push_back(result);
        }
        
        sweepResults_[experimentName] = results;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading sweep results: " << e.what() << std::endl;
    }
    
    return false;
}

std::vector<SweepResult> ParameterSweepManager::runSingleSweep(const std::string& templateName,
                                                               const std::map<std::string, std::string>& parameters,
                                                               size_t iteration) {
    std::vector<SweepResult> results;
    ExperimentFactory factory(config_);
    
    SweepResult result;
    result.parameterName = templateName;
    std::stringstream paramStream;
    bool first = true;
    for (const auto& param : parameters) {
        if (!first) paramStream << ",";
        paramStream << param.first << "=" << param.second;
        first = false;
    }
    result.parameterValue = paramStream.str();
    
    result.experiment = factory.createExperimentFromTemplate(templateName, parameters);
    if (result.experiment) {
        // Set iteration number
        result.experiment->setParameter("iteration", std::to_string(iteration));
        
        // Initialize and run experiment
        if (result.experiment->initialize()) {
            auto startTime = std::chrono::high_resolution_clock::now();
            bool success = result.experiment->run();
            auto endTime = std::chrono::high_resolution_clock::now();
            
            result.executionTime = endTime - startTime;
            result.success = success;
            result.errorMessage = success ? "" : "Experiment failed";
            
            if (success) {
                result.experiment->finalize();
                result.metricValue = result.experiment->calculatePerformanceScore();
            }
        }
    }
    
    results.push_back(result);
    return results;
}

void ParameterSweepManager::generateParameterCombinations(
    const std::map<std::string, std::vector<std::string>>& parameterMatrix,
    std::vector<std::map<std::string, std::string>>& combinations,
    std::map<std::string, std::string> current,
    size_t index) {
    
    if (index == parameterMatrix.size()) {
        combinations.push_back(current);
        return;
    }
    
    auto it = parameterMatrix.begin();
    std::advance(it, index);
    
    for (const auto& value : it->second) {
        current[it->first] = value;
        generateParameterCombinations(parameterMatrix, combinations, current, index + 1);
    }
}

} // namespace nlm