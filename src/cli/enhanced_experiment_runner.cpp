#include "enhanced_experiment_runner.hpp"
#include "experiment_manager.hpp"
#include "enhanced_experiments.hpp"
#include "experiment_factory.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

EnhancedExperimentRunner::EnhancedExperimentRunner(std::shared_ptr<Config> config) : config_(config) {
    experimentManager_ = std::make_unique<ExperimentManager>(config);
    createExperimentTemplates();
    
    Logger::getInstance().log("Enhanced Experiment Runner initialized", LogLevel::Info);
}

EnhancedExperimentRunner::~EnhancedExperimentRunner() {
    Logger::getInstance().log("Enhanced Experiment Runner shutdown", LogLevel::Info);
}

CommandResult EnhancedExperimentRunner::runEnhancedExperiment(const std::string& name, size_t steps,
                                                             const std::map<std::string, std::string>& parameters) {
    try {
        // Create experiment
        auto experiment = experimentManager_->createExperimentFromTemplate("auto", parameters);
        if (!experiment) {
            return CommandResult(1, "Failed to create experiment: " + name, false,
                                "Experiment type not found or invalid parameters");
        }
        
        // Override name
        experiment->setName(name);
        
        // Set total steps
        experiment->setTotalSteps(steps);
        
        // Run experiment
        bool success = experiment->initialize();
        if (!success) {
            return CommandResult(1, "Failed to initialize experiment: " + name, false,
                                "Experiment initialization failed");
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        success = experiment->run();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        if (success) {
            experiment->finalize();
        }
        
        return generateExperimentResult(experiment, steps, startTime);
    } catch (const std::exception& e) {
        return CommandResult(1, std::string("Error running enhanced experiment: ") + e.what(), false, e.what());
    }
}

CommandResult EnhancedExperimentRunner::runExperimentTemplate(const std::string& templateName,
                                                              const std::string& experimentName,
                                                              size_t steps) {
    try {
        // Get available templates
        auto templates = experimentManager_->getExperimentTemplates();
        
        if (std::find(templates.begin(), templates.end(), templateName) == templates.end()) {
            return CommandResult(1, "Unknown experiment template: " + templateName, false,
                                "Available templates: " + experimentUtilsToString(templates));
        }
        
        // Create experiment from template
        auto experiment = experimentManager_->createExperimentFromTemplate(templateName, experimentName);
        if (!experiment) {
            return CommandResult(1, "Failed to create experiment from template: " + templateName, false,
                                "Template creation failed");
        }
        
        // Override name and steps
        experiment->setName(experimentName);
        experiment->setTotalSteps(steps);
        
        // Run experiment
        if (experiment->initialize() && experiment->run()) {
            experiment->finalize();
            return generateExperimentResult(experiment, steps, std::chrono::high_resolution_clock::now());
        } else {
            return CommandResult(1, "Experiment failed: " + experimentName, false,
                                "Experiment execution failed");
        }
    } catch (const std::exception& e) {
        return CommandResult(1, std::string("Error running experiment template: ") + e.what(), false, e.what());
    }
}

CommandResult EnhancedExperimentRunner::runParameterSweep(const std::string& templateName,
                                                         const std::map<std::string, std::vector<std::string>>& parameterMatrix,
                                                         size_t numIterations) {
    try {
        auto sweepResults = experimentManager_->runParameterSweep(templateName, parameterMatrix, numIterations);
        
        std::stringstream output;
        output << "Parameter Sweep Results: " << templateName << std::endl;
        output << std::endl;
        
        double bestMetric = -std::numeric_limits<double>::max();
        std::string bestParams;
        
        for (const auto& result : sweepResults) {
            output << "  Parameters: " << result.parameterValue << std::endl;
            output << "    Metric Value: " << result.metricValue << std::endl;
            output << "    Success: " << (result.success ? "Yes" : "No") << std::endl;
            output << "    Execution Time: " << result.executionTime.count() << " seconds" << std::endl;
            output << std::endl;
            
            if (result.success && result.metricValue > bestMetric) {
                bestMetric = result.metricValue;
                bestParams = result.parameterValue;
            }
        }
        
        if (!bestParams.empty()) {
            output << "Best parameters: " << bestParams << std::endl;
            output << "Best metric value: " << bestMetric << std::endl;
        }
        
        return CommandResult(0, output.str(), true, "");
    } catch (const std::exception& e) {
        return CommandResult(1, std::string("Error running parameter sweep: ") + e.what(), false, e.what());
    }
}

CommandResult EnhancedExperimentRunner::analyzeExperiment(const std::string& experimentName) {
    try {
        auto analysis = experimentManager_->analyzeExperiment(experimentName);
        
        std::stringstream output;
        output << "Experiment Analysis: " << experimentName << std::endl;
        output << std::endl;
        
        output << "Statistics:" << std::endl;
        output << "  Mean: " << analysis.mean << std::endl;
        output << "  Median: " << analysis.median << std::endl;
        output << "  Standard Deviation: " << analysis.stddev << std::endl;
        output << "  Minimum: " << analysis.min << std::endl;
        output << "  Maximum: " << analysis.max << std::endl;
        output << "  25th Percentile: " << analysis.percentile25 << std::endl;
        output << "  75th Percentile: " << analysis.percentile75 << std::endl;
        output << "  Count: " << analysis.count << std::endl;
        
        return CommandResult(0, output.str(), true, "");
    } catch (const std::exception& e) {
        return CommandResult(1, std::string("Error analyzing experiment: ") + e.what(), false, e.what());
    }
}

CommandResult EnhancedExperimentRunner::compareExperiments(const std::vector<std::string>& experimentNames) {
    try {
        auto comparison = experimentManager_->compareExperiments(experimentNames);
        
        std::stringstream output;
        output << "Experiment Comparison" << std::endl;
        output << std::endl;
        
        output << "Experiment 1: " << comparison.experiment1Name << std::endl;
        output << "Experiment 2: " << comparison.experiment2Name << std::endl;
        output << std::endl;
        output << "Performance Difference: " << comparison.performanceDifference << std::endl;
        output << "Confidence Level: " << comparison.confidenceLevel << std::endl;
        output << "Significant: " << (comparison.isSignificant ? "Yes" : "No") << std::endl;
        output << std::endl;
        output << "Interpretation: " << comparison.interpretation << std::endl;
        
        return CommandResult(0, output.str(), true, "");
    } catch (const std::exception& e) {
        return CommandResult(1, std::string("Error comparing experiments: ") + e.what(), false, e.what());
    }
}

std::vector<std::string> EnhancedExperimentRunner::getAvailableExperimentTemplates() const {
    return experimentManager_->getExperimentTemplates();
}

std::vector<EnhancedExperimentRunner::ExperimentTemplateInfo> EnhancedExperimentRunner::getExperimentTemplatesInfo() const {
    std::vector<ExperimentTemplateInfo> templates;
    
    // Get available experiment types from factory
    auto experimentTypes = experimentManager_->getAvailableExperimentTypes();
    
    for (const auto& type : experimentTypes) {
        ExperimentTemplateInfo info;
        info.name = type;
        info.type = type;
        
        // Get description based on type
        if (type == "plasticity") {
            info.description = "Studies synaptic plasticity and learning mechanisms";
        } else if (type == "learning_curve") {
            info.description = "Measures learning performance across training curves";
        } else if (type == "network_connectivity") {
            info.description = "Tests information propagation through neural networks";
        } else if (type == "developmental") {
            info.description = "Studies brain development and maturation processes";
        } else if (type == "neuromodulation") {
            info.description = "Studies reward, motivation, and neuromodulation systems";
        } else {
            info.description = "Generic experiment";
        }
        
        info.defaultParameters = {{"max_steps", "1000"}}; // Default value
        info.requiredParameters = {"max_steps"};
        
        templates.push_back(info);
    }
    
    return templates;
}

void EnhancedExperimentRunner::createExperimentTemplates() {
    // Register experiment templates with the experiment manager
    // In a real implementation, this would create predefined experiment configurations
    Logger::getInstance().log("Created experiment templates", LogLevel::Info);
}

std::shared_ptr<BaseExperiment> EnhancedExperimentRunner::convertLegacyExperiment(const std::string& name) {
    // Convert legacy experiment to new format
    return nullptr; // Placeholder - would need full implementation
}

CommandResult EnhancedExperimentRunner::generateExperimentResult(const std::shared_ptr<BaseExperiment>& experiment,
                                                                size_t steps,
                                                                const std::chrono::time_point<std::chrono::high_resolution_clock>& startTime) {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto executionTime = std::chrono::duration<double>(endTime - startTime);
    
    std::stringstream output;
    output << "Experiment completed: " << experiment->getName() << std::endl;
    output << std::endl;
    output << "Experiment Details:" << std::endl;
    output << "  Name: " << experiment->getName() << std::endl;
    output << "  Type: " << experiment->getExperimentType() << std::endl;
    output << "  Description: " << experiment->getDescription() << std::endl;
    output << "  Progress: " << (experiment->getProgress() * 100) << "%" << std::endl;
    output << "  Steps: " << experiment->getCurrentStep() << " / " << experiment->getTotalSteps() << std::endl;
    output << "  Execution Time: " << executionTime.count() << " seconds" << std::endl;
    output << "  Performance Score: " << experiment->calculatePerformanceScore() << std::endl;
    output << std::endl;
    
    auto metrics = experiment->getMetrics();
    if (!metrics.empty()) {
        output << "Metrics:" << std::endl;
        for (const auto& metric : metrics) {
            output << "  " << metric.first << ": " << metric.second << std::endl;
        }
    }
    
    output << std::endl;
    output << experiment->getResultsSummary() << std::endl;
    
    return CommandResult(0, output.str(), true, "");
}

std::string EnhancedExperimentRunner::experimentUtilsToString(const std::vector<std::string>& vec) {
    std::stringstream ss;
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << vec[i];
    }
    return ss.str();
}

} // namespace nlm