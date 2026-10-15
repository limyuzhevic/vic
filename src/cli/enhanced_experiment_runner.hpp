#pragma once

#include "experiment_manager.hpp"
#include "enhanced_experiments.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <map>

namespace nlm {

// Enhanced experiment runner that integrates with the new experiment management framework
class EnhancedExperimentRunner {
public:
    EnhancedExperimentRunner(std::shared_ptr<Config> config);
    ~EnhancedExperimentRunner();

    // Run experiment using new framework
    CommandResult runEnhancedExperiment(const std::string& name, size_t steps,
                                        const std::map<std::string, std::string>& parameters = {});

    // Run experiment template
    CommandResult runExperimentTemplate(const std::string& templateName,
                                        const std::string& experimentName,
                                        size_t steps);

    // Run parameter sweep
    CommandResult runParameterSweep(const std::string& templateName,
                                   const std::map<std::string, std::vector<std::string>>& parameterMatrix,
                                   size_t numIterations = 1);

    // Analyze experiment results
    CommandResult analyzeExperiment(const std::string& experimentName);

    // Compare experiment results
    CommandResult compareExperiments(const std::vector<std::string>& experimentNames);

    // Get available experiment templates
    std::vector<std::string> getAvailableExperimentTemplates() const;

    // Get experiment metadata
    struct ExperimentTemplateInfo {
        std::string name;
        std::string type;
        std::string description;
        std::map<std::string, std::string> defaultParameters;
        std::vector<std::string> requiredParameters;
    };

    std::vector<ExperimentTemplateInfo> getExperimentTemplatesInfo() const;

    // Set experiment manager
    void setExperimentManager(std::unique_ptr<ExperimentManager> manager) {
        experimentManager_ = std::move(manager);
    }

    // Get experiment manager
    ExperimentManager* getExperimentManager() const {
        return experimentManager_.get();
    }

private:
    std::shared_ptr<Config> config_;
    std::unique_ptr<ExperimentManager> experimentManager_;

    // Create experiment templates
    void createExperimentTemplates();
    
    // Convert old experiment format to new format
    std::shared_ptr<BaseExperiment> convertLegacyExperiment(const std::string& name);
    
    // Generate experiment command results
    CommandResult generateExperimentResult(const std::shared_ptr<BaseExperiment>& experiment,
                                          size_t steps,
                                          const std::chrono::time_point<std::chrono::high_resolution_clock>& startTime);
};

} // namespace nlm