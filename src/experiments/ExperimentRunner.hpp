#pragma once

#include "Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>

namespace nlm {

// ExperimentRunner class manages and executes scientific experiments
// Coordinates experiment setup, execution, and results collection
// Provides batch processing capabilities for systematic investigation
class ExperimentRunner {
public:
    // Default constructor
    ExperimentRunner();
    
    ~ExperimentRunner();
    
    // Disable copying for memory safety, enable moving semantics
    ExperimentRunner(const ExperimentRunner&) = delete;
    ExperimentRunner& operator=(const ExperimentRunner&) = delete;
    ExperimentRunner(ExperimentRunner&&) noexcept;
    ExperimentRunner& operator=(ExperimentRunner&&) noexcept;
    
    // Experiment creation and management
    // Creates a new experiment with specified name
    std::shared_ptr<Experiment> createExperiment(const std::string& name);
    
    // Single experiment execution
    // Runs one experiment with specified brain, environment, and step limit
    // Returns true if experiment completed successfully
    bool runExperiment(std::shared_ptr<Experiment> experiment,
                      std::shared_ptr<Brain> brain,
                      std::shared_ptr<Environment> environment,
                      SimulationStep maxSteps);
    
    // Batch experiment execution
    // Runs multiple experiments using template brain/environment for efficiency
    // Returns vector of completed experiments in same order as input
    std::vector<std::shared_ptr<Experiment>> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    // Experiment catalog management
    // Gets all registered experiments
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    
    // Experiment retrieval
    // Gets experiment by name (returns nullptr if not found)
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    
    // Experiment cleanup
    // Clears all registered experiments
    void clearExperiments();
    
    // Reproducibility control
    // Sets global random seed for all experiments in this runner
    void setGlobalSeed(uint64_t seed);
    
    // Results persistence
    // Saves experiment results to file (format determined by file extension)
    // Returns true if save operation succeeded
    bool saveResults(const std::string& filepath) const;
    
private:
    // Pimpl idiom for implementation hiding
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
