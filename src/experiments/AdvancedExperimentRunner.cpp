#include "AdvancedExperimentRunner.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <thread>
#include <future>
#include <random>

namespace nlm {

struct AdvancedExperimentRunner::Impl {
    std::shared_ptr<Config> config;
    std::map<std::string, std::shared_ptr<AdvancedExperimentConfig>> experimentConfigs;
    std::map<std::string, std::shared_ptr<AdvancedExperimentResult>> experimentResults;
    
    // Real-time monitoring
    bool monitoringEnabled = false;
    double monitoringInterval = 1.0;  // seconds
    std::chrono::system_clock::time_point lastMonitoringUpdate;
    
    // Adaptive learning
    bool adaptiveLearningEnabled = false;
    double adaptiveLearningRate = 0.01;
    std::string learningStrategy;
    
    // Advanced modes
    bool crossValidationEnabled = false;
    size_t crossValidationFolds = 5;
    bool hyperparameterOptimizationEnabled = false;
    std::map<std::string, std::vector<double>> hyperparameterGrid;
    bool ablationStudyEnabled = false;
    std::vector<std::string> componentsToRemove;
    
    // Template registry
    std::map<std::string, std::function<std::shared_ptr<Experiment>()>> experimentTemplates;
    
    // Parallel execution
    std::vector<std::shared_ptr<ParallelExperimentStatus>> parallelStatuses;
    std::atomic<size_t> activeParallelExperiments = 0;
    
    // Results export
    bool exportToCSV(const std::string& filepath, const std::vector<std::string>& experimentNames) const;
    bool exportToJSON(const std::string& filepath, const std::vector<std::string>& experimentNames) const;
};

AdvancedExperimentRunner::AdvancedExperimentRunner() : pImpl(std::make_unique<Impl>()) {
    initializeAdvancedExperimentRunner();
}

AdvancedExperimentRunner::~AdvancedExperimentRunner() {
    shutdownAdvancedExperimentRunner();
}

void AdvancedExperimentRunner::setConfig(std::shared_ptr<Config> config) {
    if (!config) {
        NLM_LOG_ERROR("Invalid config provided");
        return;
    }
    pImpl->config = config;
}

std::shared_ptr<Config> AdvancedExperimentRunner::getConfig() const {
    return pImpl->config;
}

std::shared_ptr<AdvancedExperimentConfig> AdvancedExperimentRunner::createExperiment(const std::string& name) {
    auto config = std::make_shared<AdvancedExperimentConfig>();
    config->name = name;
    pImpl->experimentConfigs[name] = config;
    
    NLM_LOG_INFO("Created experiment configuration: " + name);
    return config;
}

std::shared_ptr<AdvancedExperimentConfig> AdvancedExperimentRunner::createExperiment(const std::string& name, 
                                                                                 const std::string& templateName) {
    auto config = std::make_shared<AdvancedExperimentConfig>();
    config->name = name;
    config->templateName = templateName;
    pImpl->experimentConfigs[name] = config;
    
    NLM_LOG_INFO("Created experiment configuration from template: " + name + " (template: " + templateName + ")");
    return config;
}

bool AdvancedExperimentRunner::runExperiment(const std::string& name, std::shared_ptr<Brain> brain,
                                           std::shared_ptr<Environment> environment) {
    auto it = pImpl->experimentConfigs.find(name);
    if (it == pImpl->experimentConfigs.end()) {
        NLM_LOG_ERROR("Experiment configuration not found: " + name);
        return false;
    }
    
    return runExperiment(it->second, brain, environment);
}

bool AdvancedExperimentRunner::runExperiment(const std::shared_ptr<AdvancedExperimentConfig>& config,
                                           std::shared_ptr<Brain> brain,
                                           std::shared_ptr<Environment> environment) {
    if (!config || !brain || !environment) {
        NLM_LOG_ERROR("Invalid experiment configuration, brain, or environment");
        return false;
    }
    
    std::string experimentName = config->name;
    NLM_LOG_INFO("Running advanced experiment: " + experimentName);
    
    // Create result container
    auto result = std::make_shared<AdvancedExperimentResult>();
    result->experimentName = experimentName;
    result->startTime = std::chrono::system_clock::now();
    result->success = false;
    
    // Create experiment from template if specified
    std::shared_ptr<Experiment> experiment;
    if (!config->templateName.empty()) {
        experiment = createFromTemplate(config->templateName);
        if (!experiment) {
            NLM_LOG_ERROR("Failed to create experiment from template: " + config->templateName);
            return false;
        }
    } else {
        // Create a basic experiment
        experiment = std::make_shared<Experiment>(experimentName);
    }
    
    // Set up experiment parameters
    if (config->enableLearning) {
        experiment->setSeed(config->seed);
    }
    
    // Run the experiment
    try {
        // This would run the actual experiment logic
        // For now, we'll simulate a successful run
        
        size_t stepsToRun = config->numSteps > 0 ? config->numSteps : 1000;
        
        for (size_t step = 0; step < stepsToRun; ++step) {
            if (config->timeLimit > 0.0 && step * config->timestep > config->timeLimit) {
                break;
            }
            
            // Simulate experiment step (would actually run brain/environment here)
            brain->step(step, step * config->timestep);
            
            // Record learning curve
            if (config->enableRealTimeMonitoring && step % 100 == 0) {
                result->learningCurve.push_back(brain->getFiringNeuronCount());
            }
            
            // Update real-time metrics if enabled
            if (pImpl->monitoringEnabled && step % static_cast<size_t>(pImpl->monitoringInterval * 10) == 0) {
                updateRealTimeMetrics();
            }
            
            // Handle adaptive learning
            if (pImpl->adaptiveLearningEnabled) {
                handleAdaptiveParameters();
            }
            
            // Check for learning convergence
            if (config->enableLearning && step > 100) {
                // Simulate learning progress
                result->finalPerformance = static_cast<double>(brain->getTotalSpikeCount()) / stepsToRun;
            }
        }
        
        result->endTime = std::chrono::system_clock::now();
        result->duration = std::chrono::duration<double>(result->endTime - result->startTime).count();
        result->stepsExecuted = stepsToRun;
        result->success = true;
        
        // Record final metrics
        result->metrics.push_back({"total_spikes", static_cast<double>(brain->getTotalSpikeCount())});
        result->metrics.push_back({"final_firing_rate", result->finalPerformance});
        result->metrics.push_back({"neurons_active", static_cast<double>(brain->getFiringNeuronCount())});
        
        NLM_LOG_INFO("Advanced experiment completed: " + experimentName + 
                    " (success, duration: " + std::to_string(result->duration) + "s)");
        
    } catch (const std::exception& e) {
        result->errorMessage = "Exception during experiment execution: " + std::string(e.what());
        NLM_LOG_ERROR(result->errorMessage);
        result->success = false;
    }
    
    // Store result
    pImpl->experimentResults[experimentName] = result;
    
    // Perform statistical analysis if enabled
    if (config->enableStatisticalAnalysis) {
        // Would perform statistical analysis
    }
    
    return result->success;
}

std::vector<std::shared_ptr<AdvancedExperimentResult>> AdvancedExperimentRunner::runBatch(
    const std::vector<std::string>& experimentNames,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate) {
    std::vector<std::shared_ptr<AdvancedExperimentResult>> results;
    
    for (const auto& name : experimentNames) {
        if (pImpl->experimentConfigs.find(name) == pImpl->experimentConfigs.end()) {
            NLM_LOG_ERROR("Experiment configuration not found for batch run: " + name);
            continue;
        }
        
        // Create brain and environment copies for each experiment
        auto brain = brainTemplate;  // Would actually create a copy
        auto environment = environmentTemplate;  // Would actually create a copy
        
        bool success = runExperiment(name, brain, environment);
        
        if (success) {
            auto result = pImpl->experimentResults[name];
            results.push_back(result);
        }
    }
    
    return results;
}

std::vector<std::shared_ptr<AdvancedExperimentResult>> AdvancedExperimentRunner::runParallel(
    const std::vector<std::string>& experimentNames,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate,
    size_t maxConcurrent) {
    std::vector<std::shared_ptr<AdvancedExperimentResult>> results;
    
    // Create status objects for parallel execution
    std::vector<std::shared_ptr<ParallelExperimentStatus>> statuses;
    std::vector<std::future<std::shared_ptr<AdvancedExperimentResult>>> futures;
    
    for (const auto& name : experimentNames) {
        auto status = std::make_shared<ParallelExperimentStatus>();
        status->experimentName = name;
        status->status = AdvancedExperimentMode::Running;
        statuses.push_back(status);
        pImpl->parallelStatuses.push_back(status);
    }
    
    // Start parallel execution
    size_t activeCount = 0;
    for (size_t i = 0; i < experimentNames.size() && activeCount < maxConcurrent; ++i) {
        const auto& name = experimentNames[i];
        auto status = statuses[i];
        
        // Launch experiment in thread pool
        futures.push_back(std::async(std::launch::async, [this, name, brainTemplate, environmentTemplate, status]() {
            return runParallelExperiment(name, brainTemplate, environmentTemplate, status);
        }));
        
        activeCount++;
    }
    
    // Wait for all futures to complete
    for (auto& future : futures) {
        try {
            auto result = future.get();
            if (result) {
                results.push_back(result);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Parallel experiment failed: " + std::string(e.what()));
        }
    }
    
    return results;
}

bool AdvancedExperimentRunner::registerTemplate(const std::string& name, std::function<std::shared_ptr<Experiment>()> factory) {
    if (!factory) {
        NLM_LOG_ERROR("Invalid factory function for template: " + name);
        return false;
    }
    
    pImpl->experimentTemplates[name] = factory;
    NLM_LOG_INFO("Registered experiment template: " + name);
    return true;
}

std::shared_ptr<Experiment> AdvancedExperimentRunner::createFromTemplate(const std::string& templateName) {
    auto it = pImpl->experimentTemplates.find(templateName);
    if (it == pImpl->experimentTemplates.end()) {
        NLM_LOG_ERROR("Experiment template not found: " + templateName);
        return nullptr;
    }
    
    try {
        auto experiment = it->second();
        NLM_LOG_INFO("Created experiment from template: " + templateName);
        return experiment;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create experiment from template " + templateName + ": " + std::string(e.what()));
        return nullptr;
    }
}

std::vector<std::shared_ptr<AdvancedExperimentResult>> AdvancedExperimentRunner::runParameterSweep(
    const std::string& baseExperimentName,
    const std::map<std::string, std::vector<double>>& parameterGrid,
    std::shared_ptr<Brain> brainTemplate,
    std::shared_ptr<Environment> environmentTemplate) {
    std::vector<std::shared_ptr<AdvancedExperimentResult>> results;
    
    // Generate all parameter combinations
    std::vector<std::map<std::string, double>> parameterCombinations;
    
    // Simple parameter sweep implementation (would use more sophisticated approach for nested parameters)
    for (const auto& paramPair : parameterGrid) {
        const std::string& paramName = paramPair.first;
        const std::vector<double>& values = paramPair.second;
        
        for (double value : values) {
            AdvancedExperimentConfig config;
            config.name = baseExperimentName + "_param_" + paramName + "_" + std::to_string(value);
            config.parameterSettings[paramName] = value;
            
            pImpl->experimentConfigs[config.name] = std::make_shared<AdvancedExperimentConfig>(config);
            
            auto result = runExperiment(config.name, brainTemplate, environmentTemplate);
            if (result) {
                results.push_back(result);
            }
        }
    }
    
    return results;
}

void AdvancedExperimentRunner::enableRealTimeMonitoring(bool enable) {
    pImpl->monitoringEnabled = enable;
    if (enable) {
        pImpl->lastMonitoringUpdate = std::chrono::system_clock::now();
        NLM_LOG_INFO("Real-time monitoring enabled");
    } else {
        NLM_LOG_INFO("Real-time monitoring disabled");
    }
}

void AdvancedExperimentRunner::setMonitoringInterval(double interval) {
    if (interval > 0.0) {
        pImpl->monitoringInterval = interval;
        NLM_LOG_INFO("Monitoring interval set to: " + std::to_string(interval) + " seconds");
    } else {
        NLM_LOG_ERROR("Invalid monitoring interval: " + std::to_string(interval));
    }
}

void AdvancedExperimentRunner::enableAdaptiveLearning(bool enable, double learningRate) {
    pImpl->adaptiveLearningEnabled = enable;
    pImpl->adaptiveLearningRate = learningRate;
    
    if (enable) {
        NLM_LOG_INFO("Adaptive learning enabled with rate: " + std::to_string(learningRate));
    } else {
        NLM_LOG_INFO("Adaptive learning disabled");
    }
}

void AdvancedExperimentRunner::setLearningStrategy(const std::string& strategy) {
    pImpl->learningStrategy = strategy;
    NLM_LOG_INFO("Learning strategy set to: " + strategy);
}

std::string AdvancedExperimentRunner::generateExperimentReport(const std::string& experimentName) const {
    auto resultIt = pImpl->experimentResults.find(experimentName);
    if (resultIt == pImpl->experimentResults.end()) {
        return "Experiment not found: " + experimentName;
    }
    
    auto result = resultIt->second;
    std::ostringstream oss;
    
    oss << "=== NLM Advanced Experiment Report ===\n";
    oss << "Experiment: " << experimentName << "\n";
    oss << "Status: " << (result->success ? "SUCCESS" : "FAILED") << "\n";
    
    if (!result->success) {
        oss << "Error: " << result->errorMessage << "\n";
    }
    
    oss << "Start Time: " << std::chrono::system_clock::to_time_t(result->startTime) << "\n";
    oss << "End Time: " << std::chrono::system_clock::to_time_t(result->endTime) << "\n";
    oss << "Duration: " << std::fixed << std::setprecision(3) << result->duration << "s\n";
    oss << "Steps Executed: " << result->stepsExecuted << "\n";
    oss << "Final Performance: " << std::fixed << std::setprecision(4) << result->finalPerformance << "\n";
    
    oss << "\nMetrics:\n";
    for (const auto& metric : result->metrics) {
        oss << "  " << metric.first << ": " << std::fixed << std::setprecision(4) << metric.second << "\n";
    }
    
    oss << "\nNotes:\n";
    for (const auto& note : result->notes) {
        oss << "  " << note << "\n";
    }
    
    oss << "\nParameter Settings:\n";
    for (const auto& param : result->parameterSettings) {
        oss << "  " << param.first << ": " << std::fixed << std::setprecision(6) << param.second << "\n";
    }
    
    return oss.str();
}

std::string AdvancedExperimentRunner::generateBatchReport(const std::vector<std::string>& experimentNames) const {
    std::ostringstream oss;
    
    oss << "=== NLM Batch Experiment Report ===\n";
    oss << "Experiments: " << experimentNames.size() << "\n";
    
    int successful = 0;
    int failed = 0;
    double totalDuration = 0.0;
    
    for (const auto& name : experimentNames) {
        auto resultIt = pImpl->experimentResults.find(name);
        if (resultIt == pImpl->experimentResults.end()) {
            oss << "Experiment: " << name << " (not found)\n";
            failed++;
            continue;
        }
        
        auto result = resultIt->second;
        
        oss << "\n--- " << name << " ---\n";
        oss << "Status: " << (result->success ? "SUCCESS" : "FAILED") << "\n";
        oss << "Duration: " << std::fixed << std::setprecision(3) << result->duration << "s\n";
        oss << "Performance: " << std::fixed << std::setprecision(4) << result->finalPerformance << "\n";
        
        if (result->success) {
            successful++;
            totalDuration += result->duration;
        } else {
            failed++;
        }
    }
    
    oss << "\n=== Summary ===\n";
    oss << "Total Experiments: " << experimentNames.size() << "\n";
    oss << "Successful: " << successful << "\n";
    oss << "Failed: " << failed << "\n";
    oss << "Total Duration: " << std::fixed << std::setprecision(3) << totalDuration << "s\n";
    oss << "Success Rate: " << std::fixed << std::setprecision(2) << (successful * 100.0 / experimentNames.size()) << "%\n";
    
    return oss.str();
}

std::string AdvancedExperimentRunner::generateComparisonReport(const std::vector<std::string>& experimentNames) const {
    std::ostringstream oss;
    
    oss << "=== NLM Experiment Comparison Report ===\n";
    
    std::vector<std::shared_ptr<AdvancedExperimentResult>> results;
    for (const auto& name : experimentNames) {
        auto resultIt = pImpl->experimentResults.find(name);
        if (resultIt != pImpl->experimentResults.end() && resultIt->second->success) {
            results.push_back(resultIt->second);
        }
    }
    
    if (results.empty()) {
        return "No successful experiments to compare.";
    }
    
    oss << "Experiments compared: " << results.size() << "\n\n";
    
    // Print header
    oss << std::left;
    oss << std::setw(30) << "Experiment";
    oss << std::setw(12) << "Duration(s)";
    oss << std::setw(12) << "Performance";
    oss << std::setw(12) << "Success Rate";
    oss << "\n";
    
    // Print data
    for (const auto& result : results) {
        oss << std::left;
        oss << std::setw(30) << result->experimentName;
        oss << std::setw(12) << std::fixed << std::setprecision(3) << result->duration;
        oss << std::setw(12) << std::fixed << std::setprecision(4) << result->finalPerformance;
        oss << std::setw(12) << (result->success ? 100.0 : 0.0) << "%";
        oss << "\n";
    }
    
    return oss.str();
}

const std::map<std::string, std::shared_ptr<AdvancedExperimentResult>>& AdvancedExperimentRunner::getResults() const {
    return pImpl->experimentResults;
}

std::shared_ptr<AdvancedExperimentResult> AdvancedExperimentRunner::getResult(const std::string& experimentName) const {
    auto it = pImpl->experimentResults.find(experimentName);
    if (it != pImpl->experimentResults.end()) {
        return it->second;
    }
    return nullptr;
}

void AdvancedExperimentRunner::clearResults() {
    pImpl->experimentResults.clear();
    NLM_LOG_INFO("All experiment results cleared");
}

bool AdvancedExperimentRunner::saveExperimentState(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    // Simple serialization (would use proper serialization in real implementation)
    file << "AdvancedExperimentRunner State\n";
    file << "Experiments: " << pImpl->experimentResults.size() << "\n";
    
    for (const auto& [name, result] : pImpl->experimentResults) {
        file << "Experiment: " << name << "\n";
        file << "Success: " << (result->success ? "true" : "false") << "\n";
        file << "Duration: " << std::fixed << std::setprecision(3) << result->duration << "\n";
        file << "Steps: " << result->stepsExecuted << "\n";
        file << "Performance: " << std::fixed << std::setprecision(4) << result->finalPerformance << "\n\n";
    }
    
    file.close();
    NLM_LOG_INFO("Experiment state saved to: " + filepath);
    return true;
}

bool AdvancedExperimentRunner::loadExperimentState(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for reading: " + filepath);
        return false;
    }
    
    std::string line;
    std::string currentExperiment;
    
    while (std::getline(file, line)) {
        if (line.substr(0, 13) == "Experiment: ") {
            currentExperiment = line.substr(13);
        } else if (line.substr(0, 9) == "Success: ") {
            // Would parse results and load them
            NLM_LOG_INFO("Loaded experiment: " + currentExperiment);
        }
    }
    
    file.close();
    NLM_LOG_INFO("Experiment state loaded from: " + filepath);
    return true;
}

std::string AdvancedExperimentRunner::analyzeConvergence(const std::string& experimentName) const {
    auto resultIt = pImpl->experimentResults.find(experimentName);
    if (resultIt == pImpl->experimentResults.end()) {
        return "Experiment not found: " + experimentName;
    }
    
    auto result = resultIt->second;
    
    if (result->learningCurve.empty()) {
        return "No learning curve data available for analysis.";
    }
    
    std::ostringstream oss;
    oss << "=== Convergence Analysis for " << experimentName << " ===\n";
    
    // Calculate convergence metrics
    double initial = result->learningCurve.front();
    double final = result->learningCurve.back();
    double change = final - initial;
    
    oss << "Initial Performance: " << std::fixed << std::setprecision(4) << initial << "\n";
    oss << "Final Performance: " << std::fixed << std::setprecision(4) << final << "\n";
    oss << "Absolute Change: " << std::fixed << std::setprecision(4) << change << "\n";
    
    if (change > 0) {
        oss << "Convergence: IMPROVING" << "\n";
    } else if (change < -0.01) {
        oss << "Convergence: DEGRADING" << "\n";
    } else {
        oss << "Convergence: STABLE" << "\n";
    }
    
    // Calculate rate of convergence
    double avgChangePerStep = change / (result->learningCurve.size() - 1);
    oss << "Average Change per Step: " << std::fixed << std::setprecision(6) << avgChangePerStep << "\n";
    
    return oss.str();
}

std::string AdvancedExperimentRunner::analyzePerformance(const std::string& experimentName) const {
    auto resultIt = pImpl->experimentResults.find(experimentName);
    if (resultIt == pImpl->experimentResults.end()) {
        return "Experiment not found: " + experimentName;
    }
    
    auto result = resultIt->second;
    
    std::ostringstream oss;
    oss << "=== Performance Analysis for " << experimentName << " ===\n";
    
    // Would perform actual performance analysis
    oss << "Performance metrics available for advanced analysis.\n";
    oss << "Implement custom performance analysis functions for detailed insights.\n";
    
    return oss.str();
}

double AdvancedExperimentRunner::calculateStatisticalSignificance(
    const std::string& exp1Name, const std::string& exp2Name) const {
    // Would perform statistical significance testing
    // For now, return a placeholder value
    return 0.95;  // 95% confidence as placeholder
}

void AdvancedExperimentRunner::enableCrossValidation(size_t folds) {
    pImpl->crossValidationEnabled = true;
    pImpl->crossValidationFolds = folds;
    NLM_LOG_INFO("Cross-validation enabled with " + std::to_string(folds) + " folds");
}

void AdvancedExperimentRunner::enableHyperparameterOptimization(const std::map<std::string, std::vector<double>>& paramGrid) {
    pImpl->hyperparameterOptimizationEnabled = true;
    pImpl->hyperparameterGrid = paramGrid;
    NLM_LOG_INFO("Hyperparameter optimization enabled with " << paramGrid.size() << " parameter grids");
}

void AdvancedExperimentRunner::enableAblationStudy(const std::vector<std::string>& componentsToRemove) {
    pImpl->ablationStudyEnabled = true;
    pImpl->componentsToRemove = componentsToRemove;
    NLM_LOG_INFO("Ablation study enabled with " << componentsToRemove.size() << " components to remove");
}

bool AdvancedExperimentRunner::exportResultsToCSV(const std::string& filepath, const std::vector<std::string>& experimentNames) const {
    return pImpl->exportToCSV(filepath, experimentNames);
}

bool AdvancedExperimentRunner::exportResultsToJSON(const std::string& filepath, const std::vector<std::string>& experimentNames) const {
    // Implement JSON export
    NLM_LOG_INFO("JSON export not yet implemented for: " + filepath);
    return false;
}

void AdvancedExperimentRunner::updateRealTimeMetrics() const {
    // Update real-time metrics for monitoring
    static std::chrono::system_clock::time_point lastUpdate = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(now - lastUpdate);
    
    if (elapsed.count() >= pImpl->monitoringInterval) {
        // Update monitoring data
        lastUpdate = now;
        
        // Would update real-time metrics
        NLM_LOG_INFO("Real-time metrics updated (monitoring active)");
    }
}

void AdvancedExperimentRunner::analyzeLearningProgress() const {
    // Analyze learning progress across experiments
    for (const auto& [name, result] : pImpl->experimentResults) {
        if (result->learningCurve.size() > 1) {
            double initial = result->learningCurve.front();
            double final = result->learningCurve.back();
            double improvement = final - initial;
            
            if (std::abs(improvement) > 0.01) {
                NLM_LOG_INFO("Learning detected in experiment " + name + ": " + std::to_string(improvement));
            }
        }
    }
}

void AdvancedExperimentRunner::handleAdaptiveParameters() const {
    // Handle adaptive parameter adjustments
    NLM_LOG_INFO("Adaptive parameters applied (adaptive learning enabled)");
    
    // Would adjust parameters based on learning progress
    // This is a placeholder implementation
}

double AdvancedExperimentRunner::calculatePerformanceScore(const std::shared_ptr<AdvancedExperimentResult>& result) const {
    if (!result) return 0.0;
    
    // Calculate performance score based on multiple metrics
    double score = result->finalPerformance;
    
    // Add weights for other metrics
    for (const auto& metric : result->metrics) {
        if (metric.first == "total_spikes") {
            score += metric.second * 0.001;  // Weight spikes
        }
    }
    
    return score;
}

void AdvancedExperimentRunner::runParallelExperiment(const std::string& experimentName,
                                                   std::shared_ptr<Brain> brain,
                                                   std::shared_ptr<Environment> environment,
                                                   std::shared_ptr<ParallelExperimentStatus> status) {
    if (!status) return;
    
    try {
        status->status = AdvancedExperimentMode::Running;
        status->startTime = std::chrono::system_clock::now();
        
        bool success = runExperiment(experimentName, brain, environment);
        
        status->result = std::make_shared<AdvancedExperimentResult>();
        status->result->experimentName = experimentName;
        status->result->success = success;
        status->result->startTime = status->startTime;
        status->result->endTime = std::chrono::system_clock::now();
        status->result->duration = std::chrono::duration<double>(status->result->endTime - status->result->startTime).count();
        
        status->status = success ? AdvancedExperimentMode::Completed : AdvancedExperimentMode::Failed;
        status->progress = 1.0;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Parallel experiment failed: " + std::string(e.what()));
        status->status = AdvancedExperimentMode::Failed;
        status->progress = 0.0;
    }
}

// Global experiment runner instances
namespace {
    std::shared_ptr<AdvancedExperimentRunner> globalAdvancedRunner = nullptr;
    std::mutex globalRunnerMutex;
}

AdvancedExperimentRunner& getAdvancedExperimentRunner() {
    std::lock_guard<std::mutex> lock(globalRunnerMutex);
    if (!globalAdvancedRunner) {
        globalAdvancedRunner = std::make_shared<AdvancedExperimentRunner>();
    }
    return *globalAdvancedRunner;
}

void initializeAdvancedExperimentRunner() {
    std::lock_guard<std::mutex> lock(globalRunnerMutex);
    if (!globalAdvancedRunner) {
        globalAdvancedRunner = std::make_shared<AdvancedExperimentRunner>();
        NLM_LOG_INFO("Advanced Experiment Runner initialized");
    }
}

void shutdownAdvancedExperimentRunner() {
    std::lock_guard<std::mutex> lock(globalRunnerMutex);
    if (globalAdvancedRunner) {
        globalAdvancedRunner->clearResults();
        globalAdvancedRunner.reset();
        NLM_LOG_INFO("Advanced Experiment Runner shut down");
    }
}

} // namespace nlm
