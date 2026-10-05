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
    
    // Enhanced experiment execution with monitoring integration
    bool runExperimentWithMonitoring(std::shared_ptr<Experiment> experiment,
                                     std::shared_ptr<Brain> brain,
                                     std::shared_ptr<Environment> environment,
                                     std::shared_ptr<VisualizationInterface> visualization,
                                     std::shared_ptr<MonitoringSystem> monitoring,
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
    
    // Save experiment results
    bool saveResults(const std::string& filepath) const;
    
    // Helper method for timestamp
    std::string getCurrentTimestamp() const;

} // namespace nlm
