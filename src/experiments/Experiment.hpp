#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>

namespace nlm {

/**
 * @file Experiment.hpp
 * 
 * Experimental recording system for NLM
 * 
 * This file provides the experimental recording system for NLM, implementing:
 * - Experimental run recording
 * - Metric recording and management
 * - Step data recording
 * - Experiment metadata
 * - Experiment configuration
 * 
 * The experiment system in NLM:
 * - **Experiment**: Records data from a single experimental run
 * - **Experiment recording**: Records data from experimental runs
 * - **Metric recording**: Records metrics
 * - **Step recording**: Records step data
 * - **Metadata management**: Manages experiment metadata
 * 
 * Key features:
 * - **Experiment recording**: Records data from experimental runs
 * - **Metric recording**: Records metrics
 * - **Step recording**: Records step data
 * - **Metadata management**: Manages experiment metadata
 * - **Configuration management**: Manages experiment configuration
 * 
 * Experiment characteristics in NLM:
 * - **Experiment recording**: Records data from experimental runs
 * - **Metric recording**: Records metrics
 * - **Step recording**: Records step data
 * - **Metadata management**: Manages experiment metadata
 * - **Configuration management**: Manages experiment configuration
 * 
 * @note Critical for experimental recording
 * @note Used extensively in experimental systems
 * @note Essential for experimental data collection
 * @see Experiment.cpp - For implementation details
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class Experiment
 * 
 * Neural implementation of experimental recording.
 * 
 * This class implements the experimental recording system, recording
 * data from experimental runs. It is essential for:
 * - **Experiment recording**: Records data from experimental runs
 * - **Metric recording**: Records metrics
 * - **Step recording**: Records step data
 * - **Metadata management**: Manages experiment metadata
 * - **Configuration management**: Manages experiment configuration
 * 
 * Experiment characteristics in NLM:
 * - **Experiment recording**: Records data from experimental runs
 * - **Metric recording**: Records metrics
 * - **Step recording**: Records step data
 * - **Metadata management**: Manages experiment metadata
 * - **Configuration management**: Manages experiment configuration
 * 
 * Key components:
 * - **Experiment name**: Returns experiment name
 * - **Experiment seed**: Returns experiment seed
 * - **Experiment configuration**: Manages experiment configuration
 * - **Metric recording**: Records metrics
 * - **Step recording**: Records step data
 * - **Note management**: Manages experiment notes
 * 
 * @note Critical for experimental recording
 * @see Experiment.cpp - For implementation details
 * 
 * @author NLM Development Team
 */
class Experiment {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a default experiment instance with default parameters.
     * The experiment is initialized with default values for
     * all experiment fields.
     */
    Experiment();
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates an experiment instance with specified name.
     * The experiment is initialized with specified name.
     */
    Experiment(const std::string& name);
    
    /**
     * @brief Destructor
     * 
     * Cleans up experiment resources and performs any necessary
     * cleanup operations. The experiment is left in a valid but
     * non-operational state after destruction.
     */
    ~Experiment();
    
    /**
     * @brief Get experiment name
     * 
     * Returns the experiment name. This is used for:
     * - Experiment name access
     * - Experiment name management
     * - Experiment name processing
     * - Experiment name control
     */
    const std::string& getName() const;
    
    /**
     * @brief Set experiment name
     * 
     * Sets the experiment name. This is used for:
     * - Experiment name setting
     * - Experiment name management
     * - Experiment name processing
     * - Experiment name control
     */
    void setName(const std::string& name);
    
    /**
     * @brief Get experiment seed
     * 
     * Returns the experiment seed. This is used for:
     * - Seed access
     * - Seed management
     * - Seed processing
     * - Seed control
     */
    uint64_t getSeed() const;
    
    /**
     * @brief Set experiment seed
     * 
     * Sets the experiment seed. This is used for:
     * - Seed setting
     * - Seed management
     * - Seed processing
     * - Seed control
     */
    void setSeed(uint64_t seed);
    
    /**
     * @brief Set configuration value
     * 
     * Sets a configuration value. This is used for:
     * - Configuration setting
     * - Configuration management
     * - Configuration processing
     * - Configuration control
     */
    void setConfigValue(const std::string& key, const std::string& value);
    
    /**
     * @brief Get configuration value
     * 
     * Returns a configuration value. This is used for:
     * - Configuration access
     * - Configuration management
     * - Configuration processing
     * - Configuration control
     */
    std::string getConfigValue(const std::string& key) const;
    
    /**
     * @brief Record metric
     * 
     * Records a metric. This is used for:
     * - Metric recording
     * - Metric management
     * - Metric processing
     * - Metric control
     */
    void recordMetric(const std::string& name, double value);
    
    /**
     * @brief Record metric (float)
     * 
     * Records a metric (float). This is used for:
     * - Metric recording
     * - Metric management
     * - Metric processing
     * - Metric control
     */
    void recordMetric(const std::string& name, float value);
    
    /**
     * @brief Record metric (int)
     * 
     * Records a metric (int). This is used for:
     * - Metric recording
     * - Metric management
     * - Metric processing
     * - Metric control
     */
    void recordMetric(const std::string& name, int value);
    
    /**
     * @brief Get all metrics
     * 
     * Returns all metrics. This is used for:
     * - Metrics access
     * - Metrics management
     * - Metrics processing
     * - Metrics control
     */
    const std::vector<std::pair<std::string, double>>& getMetrics() const;
    
    /**
     * @brief Get metric value
     * 
     * Returns a metric value. This is used for:
     * - Metric value access
     * - Metric value management
     * - Metric value processing
     * - Metric value control
     */
    double getMetric(const std::string& name) const;
    
    /**
     * @brief Check if metric exists
     * 
     * Checks if a metric exists. This is used for:
     * - Metric existence checking
     * - Metric checking
     * - Metric processing
     * - Metric control
     */
    bool hasMetric(const std::string& name) const;
    
    /**
     * @brief Record step data
     * 
     * Records step data. This is used for:
     * - Step recording
     * - Step data management
     * - Step data processing
     * - Step data control
     */
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                   size_t firingCount, float avgFiringRate);
    
    /**
     * @brief Get step data
     * 
     * Returns step data. This is used for:
     * - Step data access
     * - Step data management
     * - Step data processing
     * - Step data control
     */
    const std::vector<StepData>& getStepData() const;
    
    /**
     * @brief Get start step
     * 
     * Returns the start step. This is used for:
     * - Start step access
     * - Start step management
     * - Start step processing
     * - Start step control
     */
    SimulationStep getStartStep() const;
    
    /**
     * @brief Set start step
     * 
     * Sets the start step. This is used for:
     * - Start step setting
     * - Start step management
     * - Start step processing
     * - Start step control
     */
    void setStartStep(SimulationStep step);
    
    /**
     * @brief Get end step
     * 
     * Returns the end step. This is used for:
     * - End step access
     * - End step management
     * - End step processing
     * - End step control
     */
    SimulationStep getEndStep() const;
    
    /**
     * @brief Set end step
     * 
     * Sets the end step. This is used for:
     * - End step setting
     * - End step management
     * - End step processing
     * - End step control
     */
    void setEndStep(SimulationStep step);
    
    /**
     * @brief Add note
     * 
     * Adds a note. This is used for:
     * - Note addition
     * - Note management
     * - Note processing
     * - Note control
     */
    void addNote(const std::string& note);
    
    /**
     * @brief Get notes
     * 
     * Returns notes. This is used for:
     * - Note access
     * - Note management
     * - Note processing
     * - Note control
     */
    const std::vector<std::string>& getNotes() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
