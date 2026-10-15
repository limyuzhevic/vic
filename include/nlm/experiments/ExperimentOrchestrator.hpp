#pragma once

#include "ExperimentParams.hpp"
#include "BaseExperiment.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include <iostream>

namespace nlm {

class Brain;

class ExperimentOrchestrator {
public:
    ExperimentOrchestrator();
    ~ExperimentOrchestrator() = default;
    
    // Add an experiment to the orchestrator
    void addExperiment(std::unique_ptr<BaseExperiment> experiment);
    
    // Run all experiments
    void runExperiments(std::shared_ptr<Brain> brain);
    
    // Get the list of added experiments
    const std::vector<std::unique_ptr<BaseExperiment>>& getExperiments() const { return experiments_; }
    
    // Reset brain between experiments
    void setResetBrainBetweenExperiments(bool reset) { reset_brain_between_experiments_ = reset; }
    bool getResetBrainBetweenExperiments() const { return reset_brain_between_experiments_; }
    
    // Run a specific experiment by name
    bool runExperiment(std::shared_ptr<Brain> brain, const std::string& experimentName);
    
    // Get experiment results summary
    std::string getResultsSummary() const;
    
    // Clear all experiments
    void clearExperiments();
    
private:
    std::vector<std::unique_ptr<BaseExperiment>> experiments_;
    bool reset_brain_between_experiments_ = true;
    std::vector<ExperimentResult> experimentResults_;
    
    void runSingleExperiment(std::shared_ptr<Brain> brain, BaseExperiment& experiment);
};

} // namespace nlm