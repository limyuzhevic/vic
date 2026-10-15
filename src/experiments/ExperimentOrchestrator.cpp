#include "ExperimentOrchestrator.hpp"
#include "brain/Brain.hpp"
#include <algorithm>

namespace nlm {

ExperimentOrchestrator::ExperimentOrchestrator() {
    // Default experiments are added via configuration
    // This can be extended with factory methods for experiment creation
}

void ExperimentOrchestrator::addExperiment(std::unique_ptr<BaseExperiment> experiment) {
    if (experiment) {
        experiments_.push_back(std::move(experiment));
        NLM_LOG_INFO("Added experiment: " + experiment->getName());
    }
}

void ExperimentOrchestrator::runExperiments(std::shared_ptr<Brain> brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot run experiments - brain is null");
        return;
    }
    
    NLM_LOG_INFO("=== Starting Experiment Suite ===");
    NLM_LOG_INFO("Running " + std::to_string(experiments_.size()) + " experiments");
    
    for (size_t i = 0; i < experiments_.size(); ++i) {
        auto& experiment = experiments_[i];
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Experiment " + std::to_string(i + 1) + ": " + experiment->getName() + " ===");
        
        // Reset brain before experiment if configured
        if (reset_brain_between_experiments_) {
            NLM_LOG_INFO("Resetting brain for experiment...");
            brain->reset();
            if (!brain->initialize()) {
                NLM_LOG_ERROR("Failed to initialize brain for experiment " + experiment->getName());
                experimentResults_.push_back({false, "Brain initialization failed", {}, {"Brain initialization failed"}});
                continue;
            }
        }
        
        // Run the experiment
        runSingleExperiment(brain, *experiment);
        
        NLM_LOG_INFO("Experiment " + std::to_string(i + 1) + " completed");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Experiment Suite Complete ===");
    NLM_LOG_INFO("Results: " + getResultsSummary());
}

void ExperimentOrchestrator::runSingleExperiment(std::shared_ptr<Brain> brain, BaseExperiment& experiment) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        ExperimentResult result = experiment.run(brain);
        experimentResults_.push_back(result);
        
        // Log experiment result
        NLM_LOG_INFO("Experiment Result: " + result.message);
        if (!result.logs.empty()) {
            NLM_LOG_INFO("Detailed logs:");
            for (const auto& log : result.logs) {
                NLM_LOG_INFO("  " + log);
            }
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception during experiment " + experiment.getName() + ": " + std::string(e.what()));
        experimentResults_.push_back({false, "Exception: " + std::string(e.what()), {}, {"Exception: " + std::string(e.what())}});
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto executionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
}

bool ExperimentOrchestrator::runExperiment(std::shared_ptr<Brain> brain, const std::string& experimentName) {
    for (auto& experiment : experiments_) {
        if (experiment->getName() == experimentName) {
            runSingleExperiment(brain, *experiment);
            return true;
        }
    }
    
    NLM_LOG_ERROR("Experiment not found: " + experimentName);
    return false;
}

std::string ExperimentOrchestrator::getResultsSummary() const {
    if (experimentResults_.empty()) {
        return "No experiments run";
    }
    
    size_t successful = 0;
    for (const auto& result : experimentResults_) {
        if (result.success) successful++;
    }
    
    return std::to_string(successful) + "/" + std::to_string(experimentResults_.size()) + 
           " experiments successful";
}

void ExperimentOrchestrator::clearExperiments() {
    experiments_.clear();
    experimentResults_.clear();
    NLM_LOG_INFO("All experiments cleared");
}

} // namespace nlm