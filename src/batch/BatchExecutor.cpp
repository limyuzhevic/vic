// NLM Batch Executor
// Implementation of batch execution and experiment workflow management

#include "BatchExecutor.hpp"
#include "experiments/Phase6Experiment.hpp"
#include "core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <future>
#include <iomanip>

namespace nlm {

struct BatchExecutor::Impl {
    std::map<std::string, std::thread::id> activeThreads;
    std::mutex threadsMutex;
    bool shuttingDown = false;
    size_t totalExperimentsProcessed = 0;
};

BatchExecutor::BatchExecutor() : pImpl(std::make_unique<Impl>()) {}
BatchExecutor::~BatchExecutor() = default;

std::vector<ExperimentExecutionResult> BatchExecutor::executeBatch(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    const BatchExecutionConfig& config) {
    
    std::vector<ExperimentExecutionResult> results;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    stats.startTime = "";
    stats.totalExperiments = experiments.size();
    
    NLM_LOG_INFO("Starting batch execution of " + std::to_string(experiments.size()) + 
                " experiments (mode: " + config.mode + ")");
    
    if (config.mode == "parallel" && experiments.size() > 1) {
        // Parallel execution
        results = executeBatchInParallel(experiments, config);
    } else {
        // Sequential execution
        results = executeBatchInSequence(experiments, config);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    stats.endTime = "";
    stats.totalExecutionTime = std::chrono::duration<double>(endTime - startTime).count();
    if (stats.totalExperiments > 0) {
        stats.averageExecutionTime = stats.totalExecutionTime / stats.totalExperiments;
    }
    
    NLM_LOG_INFO("Batch execution completed: " + std::to_string(stats.successfulExperiments) + "/" +
                std::to_string(stats.totalExperiments) + " successful");
    
    return results;
}

std::vector<ExperimentExecutionResult> BatchExecutor::executeBatchInParallel(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    const BatchExecutionConfig& config) {
    
    std::vector<ExperimentExecutionResult> results;
    std::vector<std::future<ExperimentExecutionResult>> futures;
    
    // Submit experiments to thread pool
    for (const auto& experiment : experiments) {
        futures.push_back(std::async(std::launch::async, [this, experiment, config]() {
            return executeExperimentInternal(experiment, config, {});
        }));
    }
    
    // Collect results
    for (auto& future : futures) {
        try {
            auto result = future.get();
            results.push_back(result);
            
            std::lock_guard<std::mutex> lock(statsMutex);
            stats.totalExperimentsProcessed++;
            if (result.success) {
                stats.successfulExperiments++;
            } else {
                stats.failedExperiments++;
            }
            
        } catch (const std::exception& e) {
            ExperimentExecutionResult errorResult;
            errorResult.errorMessage = std::string("Exception: ") + e.what();
            errorResult.success = false;
            results.push_back(errorResult);
            
            std::lock_guard<std::mutex> lock(statsMutex);
            stats.totalExperimentsProcessed++;
            stats.failedExperiments++;
        }
    }
    
    return results;
}

std::vector<ExperimentExecutionResult> BatchExecutor::executeBatchInSequence(
    const std::vector<std::shared_ptr<Experiment>>& experiments,
    const BatchExecutionConfig& config) {
    
    std::vector<ExperimentExecutionResult> results;
    
    for (size_t i = 0; i < experiments.size(); ++i) {
        const auto& experiment = experiments[i];
        
        NLM_LOG_INFO("Executing experiment " + std::to_string(i + 1) + "/" +
                    std::to_string(experiments.size()) + ": " + experiment->getName());
        
        ExperimentExecutionResult result = executeExperimentInternal(experiment, config, {});
        results.push_back(result);
        
        // Check for early termination
        if (!result.success && !config.continueOnError) {
            NLM_LOG_WARNING("Execution stopped due to error: " + result.errorMessage);
            break;
        }
        
        std::lock_guard<std::mutex> lock(statsMutex);
        stats.totalExperimentsProcessed++;
        if (result.success) {
            stats.successfulExperiments++;
        } else {
            stats.failedExperiments++;
        }
    }
    
    return results;
}

std::vector<ExperimentExecutionResult> BatchExecutor::executeWithParameterSweep(
    const std::shared_ptr<Experiment>& experimentTemplate,
    const ScriptOptions& sweepOptions,
    const BatchExecutionConfig& config) {
    
    // Generate parameter combinations
    ScriptProcessor scriptProcessor;
    auto combinations = scriptProcessor.generateSweepCombinations(sweepOptions.sweepParameters);
    
    NLM_LOG_INFO("Parameter sweep: Executing " + std::to_string(combinations.size()) +
                " parameter combinations");
    
    return executeBatchWithParameters(experimentTemplate, combinations, config);
}

std::vector<ExperimentExecutionResult> BatchExecutor::executeGridSearch(
    const std::shared_ptr<Experiment>& experimentTemplate,
    const ScriptOptions& gridOptions,
    const BatchExecutionConfig& config) {
    
    // Generate parameter combinations for grid search
    ScriptProcessor scriptProcessor;
    auto combinations = scriptProcessor.generateGridCombinations(gridOptions.gridSearch);
    
    NLM_LOG_INFO("Grid search: Executing " + std::to_string(combinations.size()) +
                " parameter combinations");
    
    return executeBatchWithParameters(experimentTemplate, combinations, config);
}

std::vector<ExperimentExecutionResult> BatchExecutor::executeChain(
    const ExperimentChain& chain,
    const std::map<std::string, std::shared_ptr<Experiment>>& experimentMap,
    const BatchExecutionConfig& config) {
    
    std::vector<ExperimentExecutionResult> results;
    
    for (const auto& experimentName : chain.experimentSequence) {
        auto it = experimentMap.find(experimentName);
        if (it == experimentMap.end()) {
            NLM_LOG_ERROR("Experiment not found in map: " + experimentName);
            continue;
        }
        
        const auto& experiment = it->second;
        
        // Check dependencies
        if (!chain.dependencies.empty()) {
            // In real implementation, check dependencies before execution
        }
        
        // Check conditions
        if (!chain.conditions.empty()) {
            if (!checkConditions(chain.conditions, {})) {
                NLM_LOG_INFO("Skipping experiment " + experimentName + " due to conditions");
                continue;
            }
        }
        
        NLM_LOG_INFO("Executing chained experiment: " + experimentName);
        
        ExperimentExecutionResult result = executeExperimentInternal(experiment, config, {});
        results.push_back(result);
        
        if (!config.continueOnError && !result.success) {
            NLM_LOG_WARNING("Execution stopped due to error in chained experiment: " + experimentName);
            break;
        }
    }
    
    return results;
}

ExperimentExecutionResult BatchExecutor::executeExperiment(
    const std::shared_ptr<Experiment>& experiment,
    const BatchExecutionConfig& config) {
    
    ExperimentExecutionResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        NLM_LOG_INFO("Executing single experiment: " + experiment->getName());
        
        result = executeExperimentInternal(experiment, config, {});
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Exception: ") + e.what();
        result.success = false;
        NLM_LOG_ERROR("Exception during experiment execution: " + result.errorMessage);
    }
    
    return result;
}

ExperimentExecutionResult BatchExecutor::executeExperimentInternal(
    const std::shared_ptr<Experiment>& experiment,
    const BatchExecutionConfig& config,
    const std::map<std::string, std::string>& parameters) {
    
    ExperimentExecutionResult result;
    result.experimentName = experiment->getName();
    
    try {
        // In real implementation, this would run the actual experiment
        // For now, simulate experiment execution
        
        // Simulate work
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Simulate experiment processing
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Generate some metrics
        result.metrics["execution_time"] = std::to_string(
            std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count());
        result.metrics["parameters"] = "simulated";
        
        // Simulate success or failure
        result.success = true;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Experiment error: ") + e.what();
        result.success = false;
    }
    
    return result;
}

ExperimentExecutionResult BatchExecutor::executeExperimentInThread(
    const std::shared_ptr<Experiment>& experiment,
    const std::map<std::string, std::string>& parameters) {
    
    // Similar to executeExperimentInternal but for parallel execution
    ExperimentExecutionResult result;
    result.experimentName = experiment->getName();
    
    try {
        // Simulate parallel execution
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        result.success = true;
        result.executionTime = 0.1;  // Simulated
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Thread error: ") + e.what();
        result.success = false;
    }
    
    return result;
}

std::shared_ptr<Experiment> BatchExecutor::createExperimentFromTemplate(
    const std::shared_ptr<Experiment>& templateExperiment,
    const std::map<std::string, std::string>& parameters) {
    
    // Create a copy of the experiment template
    // In real implementation, this would apply parameters to the experiment
    return templateExperiment;  // Simplified
}

bool BatchExecutor::checkConditions(
    const std::map<std::string, std::string>& conditions,
    const std::map<std::string, std::string>& experimentData) {
    
    // Simplified condition checking
    // In real implementation, evaluate conditions based on experiment data
    return true;
}

std::vector<ExperimentExecutionResult> BatchExecutor::executeBatchWithParameters(
    const std::shared_ptr<Experiment>& experimentTemplate,
    const std::vector<std::map<std::string, std::string>>& parameterCombinations,
    const BatchExecutionConfig& config) {
    
    std::vector<ExperimentExecutionResult> results;
    
    for (size_t i = 0; i < parameterCombinations.size(); ++i) {
        const auto& params = parameterCombinations[i];
        
        // Create experiment with parameters
        auto experiment = createExperimentFromTemplate(experimentTemplate, params);
        
        // Execute experiment
        ExperimentExecutionResult result = executeExperimentInternal(experiment, config, params);
        results.push_back(result);
        
        // Save checkpoint if requested
        if (config.saveCheckpoints && !config.checkpointFile.empty()) {
            std::string checkpointFile = config.checkpointFile + "_" + std::to_string(i) + ".nlm";
            saveBatchCheckpoint({result}, checkpointFile);
        }
    }
    
    return results;
}

bool BatchExecutor::saveBatchCheckpoint(const std::vector<ExperimentExecutionResult>& results,
                                        const std::string& checkpointFile) {
    
    std::ofstream file(checkpointFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot save checkpoint to: " + checkpointFile);
        return false;
    }
    
    // Simple checkpoint format
    file << "[batch_checkpoint]\n";
    file << "version = 1.0\n";
    file << "timestamp = " << std::time(nullptr) << "\n";
    file << "experiments_count = " << results.size() << "\n";
    file << "successful_count = " << std::count_if(results.begin(), results.end(),
                     [](const ExperimentExecutionResult& r) { return r.success; }) << "\n";
    
    // Write detailed results
    file << "\n[results]\n";
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        file << "experiment_" << i << " = {\n";
        file << "  name = " << result.experimentName << "\n";
        file << "  success = " << (result.success ? "true" : "false") << "\n";
        file << "  execution_time = " << result.executionTime << "\n";
        file << "}\n";
    }
    
    file.close();
    NLM_LOG_INFO("Batch checkpoint saved to: " + checkpointFile);
    return true;
}

bool BatchExecutor::loadBatchCheckpoint(std::vector<ExperimentExecutionResult>& results,
                                        const std::string& checkpointFile) {
    
    std::ifstream file(checkpointFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot load checkpoint from: " + checkpointFile);
        return false;
    }
    
    std::string line;
    std::string currentSection;
    ExperimentExecutionResult currentResult;
    
    while (std::getline(file, line)) {
        // Remove whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#') continue;
        
        if (line[0] == '[' && line.back() == ']') {
            // Section header
            currentSection = line.substr(1, line.length() - 2);
            if (currentSection == "experiment_0" || currentSection == "experiment_1" ||
                currentSection == "experiment_2" || currentSection == "experiment_3") {
                if (!currentResult.experimentName.empty()) {
                    results.push_back(currentResult);
                }
                currentResult = ExperimentExecutionResult();
                // Extract experiment number
                size_t underscore = currentSection.find('_');
                if (underscore != std::string::npos) {
                    currentResult.experimentName = "Experiment_" + currentSection.substr(underscore + 1);
                }
            }
        } else if (currentSection == "results" && line.find('=') != std::string::npos) {
            size_t equalsPos = line.find('=');
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            key.erase(0, key.find_first_not_of(" \t\r\n"));
            key.erase(key.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            
            if (key == "name") {
                currentResult.experimentName = value;
            } else if (key == "success") {
                currentResult.success = (value == "true" || value == "1");
            } else if (key == "execution_time") {
                currentResult.executionTime = std::stod(value);
            }
        }
    }
    
    if (!currentResult.experimentName.empty()) {
        results.push_back(currentResult);
    }
    
    file.close();
    NLM_LOG_INFO("Batch checkpoint loaded from: " + checkpointFile + " (" +
                std::to_string(results.size()) + " experiments)");
    return true;
}

} // namespace nlm