#pragma once

#include "Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>

namespace nlm {

/**
 * @file ExperimentRunner.hpp
 * 
 * Experiment execution system for NLM
 * 
 * This file provides the experiment execution system for NLM, implementing:
 * - Experiment creation
 * - Experiment execution
 * - Batch experiment execution
 * - Experiment management
 * - Experiment results saving
 * 
 * The experiment execution system in NLM:
 * - **ExperimentRunner**: Runs experiments and collects data
 * - **Experiment creation**: Creates new experiments
 * - **Experiment execution**: Runs single experiments
 * - **Batch execution**: Runs batch of experiments
 * - **Experiment management**: Manages experiments
 * 
 * Key features:
 * - **Experiment creation**: Creates new experiments
 * - **Experiment execution**: Runs single experiments
 * - **Batch execution**: Runs batch of experiments
 * - **Experiment management**: Manages experiments
 * - **Results saving**: Saves experiment results
 * 
 * Experiment execution characteristics in NLM:
 * - **Experiment creation**: Creates new experiments
 * - **Experiment execution**: Runs single experiments
 * - **Batch execution**: Runs batch of experiments
 * - **Experiment management**: Manages experiments
 * - **Results saving**: Saves experiment results
 * 
 * @note Critical for experiment execution
 * @note Used extensively in experiment systems
 * @note Essential for experiment data collection
 * @see ExperimentRunner.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class ExperimentRunner
 * 
 * Neural implementation of experiment execution.
 * 
 * This class implements the experiment execution system, running
 * experiments and collecting data. It is essential for:
 * - **Experiment creation**: Creates new experiments
 * - **Experiment execution**: Runs single experiments
 * - **Batch execution**: Runs batch of experiments
 * - **Experiment management**: Manages experiments
 * - **Results saving**: Saves experiment results
 * 
 * Experiment execution characteristics in NLM:
 * - **Experiment creation**: Creates new experiments
 * - **Experiment execution**: Runs single experiments
 * - **Batch execution**: Runs batch of experiments
 * - **Experiment management**: Manages experiments
 * - **Results saving**: Saves experiment results
 * 
 * Key components:
 * - **Experiment creation**: Creates new experiments
 * - **Experiment execution**: Runs single experiments
 * - **Batch execution**: Runs batch of experiments
 * - **Experiment management**: Manages experiments
 * - **Results saving**: Saves experiment results
 * 
 * @note Critical for experiment execution
 * @see ExperimentRunner.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class ExperimentRunner {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default experiment runner instance with default parameters.
     * The experiment runner is initialized with default values for
     * all experiment runner fields.
     */
    ExperimentRunner();
    
    /**
     * @brief Destructor
     * 
     * Cleans up experiment runner resources and performs any necessary
     * cleanup operations. The experiment runner is left in a valid but
     * non-operational state after destruction.
     */
    ~ExperimentRunner();
    
    /**
     * @brief Create new experiment
     * 
     * Creates a new experiment. This is used for:
     * - Experiment creation
     * - Experiment management
     * - Experiment processing
     * - Experiment control
     */
    std::shared_ptr<Experiment> createExperiment(const std::string& name);
    
    /**
     * @brief Run single experiment
     * 
     * Runs a single experiment. This is used for:
     * - Experiment execution
     * - Experiment running
     * - Experiment processing
     * - Experiment control
     */
    bool runExperiment(std::shared_ptr<Experiment> experiment,
                      std::shared_ptr<Brain> brain,
                      std::shared_ptr<Environment> environment,
                      SimulationStep maxSteps);
    
    /**
     * @brief Run batch of experiments
     * 
     * Runs a batch of experiments. This is used for:
     * - Batch execution
     * - Experiment batch processing
     * - Experiment batch control
     * - Batch experiment management
     */
    std::vector<std::shared_ptr<Experiment>> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    /**
     * @brief Get all experiments
     * 
     * Returns all experiments. This is used for:
     * - Experiments access
     * - Experiments management
     * - Experiments processing
     * - Experiments control
     */
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    
    /**
     * @brief Get experiment by name
     * 
     * Returns an experiment by name. This is used for:
     * - Experiment access
     * - Experiment management
     * - Experiment processing
     * - Experiment control
     */
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    
    /**
     * @brief Clear all experiments
     * 
     * Clears all experiments. This is used for:
     * - Experiment clearing
     * - Experiment management
     * - Experiment processing
     * - Experiment control
     */
    void clearExperiments();
    
    /**
     * @brief Set random seed for reproducibility
     * 
     * Sets the random seed for reproducibility. This is used for:
     * - Seed setting
     * - Seed management
     * - Seed processing
     * - Seed control
     */
    void setGlobalSeed(uint64_t seed);
    
    /**
     * @brief Save experiment results
     * 
     * Saves experiment results. This is used for:
     * - Results saving
     * - Results management
     * - Results processing
     * - Results control
     */
    bool saveResults(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
