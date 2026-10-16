// src/batch/BatchProcessor.hpp
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>

namespace nlm {

// Batch processor for running multiple simulations
class BatchProcessor {
public:
    BatchProcessor();
    ~BatchProcessor();
    
    // Add a configuration to the batch
    void addConfiguration(const std::shared_ptr<Config>& config, const std::string& name = "");
    
    // Get all configurations
    const std::vector<std::shared_ptr<Config>>& getConfigurations() const;
    
    // Clear all configurations
    void clearConfigurations();
    
    // Run batch simulation
    std::vector<std::shared_ptr<Experiment>> runBatchSimulation(
        const std::shared_ptr<Environment>& environmentTemplate,
        SimulationStep maxStepsPerSimulation,
        const std::function<void(std::shared_ptr<Brain>, SimulationStep)>& progressCallback = nullptr);
    
    // Run hyperparameter search
    std::shared_ptr<Config> runHyperparameterSearch(
        const std::vector<std::string>& hyperparameterNames,
        const std::vector<std::vector<ConfigValue>>& hyperparameterValues,
        const std::shared_ptr<Environment>& environmentTemplate,
        SimulationStep maxSteps,
        const std::function<void(double)>& progressCallback = nullptr);
    
    // Run cross-validation
    std::vector<std::shared_ptr<Experiment>> runCrossValidation(
        const std::vector<std::shared_ptr<Config>>& configs,
        const std::shared_ptr<Environment>& environmentTemplate,
        SimulationStep maxSteps,
        double trainTestRatio = 0.8,
        const std::function<void(double)>& progressCallback = nullptr);
    
    // Save batch results
    bool saveResults(const std::string& filepath) const;
    
    // Load batch results
    bool loadResults(const std::string& filepath);
    
    // Get batch statistics
    std::string getStatistics() const;
    
    // Enable/disable verbose output
    void setVerbose(bool enable) { verbose_ = enable; }
    
    // Set maximum concurrent simulations
    void setMaxConcurrent(size_t maxConcurrent) { maxConcurrent_ = maxConcurrent; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool verbose_;
    size_t maxConcurrent_;
};

} // namespace nlm