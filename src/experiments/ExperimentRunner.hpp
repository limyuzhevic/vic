#pragma once

#include "Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>

namespace nlm {

// ExperimentRunner: Runs experiments and collects data
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // Create new experiment
    std::shared_ptr<Experiment> createExperiment(const std::string& name);
    
    // Run single experiment
    bool runExperiment(std::shared_ptr<Experiment> experiment,
                      std::shared_ptr<Brain> brain,
                      std::shared_ptr<Environment> environment,
                      SimulationStep maxSteps);
    
    // Run batch of experiments
    std::vector<std::shared_ptr<Experiment>> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    // Get all experiments
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    
    // Get experiment by name
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    
    // Clear all experiments
    void clearExperiments();
    
    // Set random seed for reproducibility
    void setGlobalSeed(uint64_t seed);
    
// Get all experiments
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;

    // Get experiment by name
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;

    // Clear all experiments
    void clearExperiments();

    // Set random seed for reproducibility
    void setGlobalSeed(uint64_t seed);

    // Save experiment results
    bool saveResults(const std::string& filepath) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Helper functions for cloning and saving
    std::shared_ptr<Brain> cloneBrain(std::shared_ptr<Brain> brainTemplate);
    std::shared_ptr<Environment> cloneEnvironment(std::shared_ptr<Environment> environmentTemplate);
    bool saveExperimentResultsToFile(const std::string& filepath) const;

}; // class ExperimentRunner

} // namespace nlm
