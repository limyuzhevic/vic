#pragma once

/**
 * @file Experiment.hpp
 * @brief Experiment class: Records data from experimental simulation runs.
 * 
 * This header defines the Experiment class, which serves as a data recording
 * system for capturing and storing experimental results. Each Experiment instance
 * represents a single simulation run and records comprehensive metrics, configuration
 * parameters, and step-by-step execution data for post-analysis and scientific validation.
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0.0
 * @see src/core/Types/Types.hpp
 * @see src/core/Config/Config.hpp
 * @see src/agent/AgentBrain.hpp
 * @see src/brain/Brain.hpp
 * @see src/plasticity/PlasticityRule.hpp
 * @see TODO Complete experiment recording (Phase 2)
 */

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>

namespace nlm {

/**
 * Experiment: Records data from a single experimental run.
 * 
 * The Experiment class provides comprehensive data recording and analysis
 * capabilities for neural network simulations. It captures:
 * 
 * - **Metadata**: Experiment name, identifier, and configuration parameters
 * - **Metrics**: Scalar values representing network properties and performance
 * - **Step Data**: Time-series data of network state at each simulation step
 * - **Notes**: Human-readable annotations and observations
 * 
 * This class uses the PImpl idiom for efficient implementation changes without
 * recompilation. It supports multiple metric value types (double, float, int)
 * for flexible recording of different types of numerical data.
 * 
 * @see Config for configuration management
 * @see AgentBrain for integration with simulated agents
 * @see Brain for neural network state recording
 * @see PlasticityRule for plasticity mechanism documentation
 * @see TODO Implement comprehensive experiment recording (Phase 2)
 */
class Experiment {
public:
    /**
     * Default constructor creates an unnamed experiment with default settings.
     */
    Experiment();
    
    /**
     * Construct experiment with specified name.
     * 
     * @param name Human-readable name for the experiment
     * @see getName()
     * @see setName()
     */
    Experiment(const std::string& name);
    
    /**
     * Destructor ensures proper cleanup of experiment data and implementation.
     */
    ~Experiment();
    
    /**
     * @name Experiment Metadata
     * 
     * Methods for managing experiment identity and basic parameters.
     */
    /// @{
    
    /**
     * @brief Get experiment name
     * 
     * @return const std::string& Name of the experiment
     * @see setName()
     */
    const std::string& getName() const;
    
    /**
     * @brief Set experiment name
     * 
     * @param name New name for the experiment
     * @see getName()
     */
    void setName(const std::string& name);
    
    /**
     * @brief Get random seed used for this experiment
     * 
     * @return uint64_t Random seed ensuring reproducibility
     * @see setSeed()
     */
    uint64_t getSeed() const;
    
    /**
     * @brief Set random seed for reproducibility
     * 
     * @param seed Random seed value
     * @see getSeed()
     */
    void setSeed(uint64_t seed);
    /// @}
    
    /**
     * @name Configuration Management
     * 
     * Methods for managing experiment configuration parameters.
     */
    /// @{
    
    /**
     * @brief Set configuration value as string
     * 
     * @param key Configuration key
     * @param value String configuration value
     * @see getConfigValue()
     */
    void setConfigValue(const std::string& key, const std::string& value);
    
    /**
     * @brief Get configuration value as string
     * 
     * @param key Configuration key
     * @return std::string Configuration value as string
     * @see setConfigValue()
     */
    std::string getConfigValue(const std::string& key) const;
    /// @}
    
    /**
     * @name Metrics Recording
     * 
     * Methods for recording numerical metrics and performance measures.
     */
    /// @{
    
    /**
     * @brief Record a metric value (double precision)
     * 
     * @param name Name/identifier of the metric
     * @param value Double precision value to record
     * @see recordMetric(const std::string&, float)
     * @see recordMetric(const std::string&, int)
     * @see getMetrics()
     */
    void recordMetric(const std::string& name, double value);
    
    /**
     * @brief Record a metric value (single precision)
     * 
     * @param name Name/identifier of the metric
     * @param value Single precision value to record
     * @see recordMetric(const std::string&, double)
     * @see recordMetric(const std::string&, int)
     */
    void recordMetric(const std::string& name, float value);
    
    /**
     * @brief Record a metric value (integer)
     * 
     * @param name Name/identifier of the metric
     * @param value Integer value to record
     * @see recordMetric(const std::string&, double)
     * @see recordMetric(const std::string&, float)
     */
    void recordMetric(const std::string& name, int value);
    
    /**
     * @brief Get all recorded metrics
     * 
     * @return const std::vector<std::pair<std::string, double>>& All metrics as string-name/double-value pairs
     * @see recordMetric()
     */
    const std::vector<std::pair<std::string, double>>& getMetrics() const;
    
    /**
     * @brief Get specific metric value
     * 
     * @param name Name of metric to retrieve
     * @return double Metric value (converted to double if necessary)
     * @see hasMetric()
     */
    double getMetric(const std::string& name) const;
    
    /**
     * @brief Check if metric exists
     * 
     * @param name Name of metric to check
     * @return true if metric exists, false otherwise
     * @see getMetric()
     */
    bool hasMetric(const std::string& name) const;
    /// @}
    
    /**
     * @name Step Data Recording
     * 
     * Methods for recording time-series simulation data.
     */
    /// @{
    
    /**
     * @brief Record step data for a simulation timestep
     * 
     * @param step Simulation step number
     * @param neuronCount Current number of neurons
     * @param synapseCount Current number of synapses
     * @param firingCount Number of firing neurons at this step
     * @param avgFiringRate Average firing rate at this step
     * @see StepData
     * @see getStepData()
     */
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                   size_t firingCount, float avgFiringRate);
    
    /**
     * Step data structure for recording network state at each simulation timestep.
     * 
     * Contains essential network metrics recorded at regular intervals for
     * analysis and visualization.
     */
    struct StepData {
        SimulationStep step;      /**< Simulation step number */
        size_t neuronCount;        /**< Current number of neurons */
        size_t synapseCount;       /**< Current number of synapses */
        size_t firingCount;        /**< Number of firing neurons */
        float avgFiringRate;       /**< Average firing rate */
    };
    
    /**
     * @brief Get all recorded step data
     * 
     * @return const std::vector<StepData>& All step data
     */
    const std::vector<StepData>& getStepData() const;
    /// @}
    
    /**
     * @name Simulation Duration
     * 
     * Methods for managing experiment start and end times.
     */
    /// @{
    
    /**
     * @brief Get experiment start step
     * 
     * @return SimulationStep Start step of the experiment
     * @see setStartStep()
     */
    SimulationStep getStartStep() const;
    
    /**
     * @brief Set experiment start step
     * 
     * @param step Start step value
     * @see getStartStep()
     */
    void setStartStep(SimulationStep step);
    
    /**
     * @brief Get experiment end step
     * 
     * @return SimulationStep End step of the experiment
     * @see setEndStep()
     */
    SimulationStep getEndStep() const;
    
    /**
     * @brief Set experiment end step
     * 
     * @param step End step value
     * @see getEndStep()
     */
    void setEndStep(SimulationStep step);
    /// @}
    
    /**
     * @name Notes and Annotations
     * 
     * Methods for adding human-readable notes and observations.
     */>
    /// @{
    
    /**
     * @brief Add a note to the experiment
     * 
     * @param note Human-readable annotation or observation
     * @note Notes can be used to document experimental conditions,
     *       observations, or important events during simulation.
     */
    void addNote(const std::string& note);
    
    /**
     * @brief Get all notes
     * 
     * @return const std::vector<std::string>& All experiment notes
     */
    const std::vector<std::string>& getNotes() const;
    /// @}
    
    /**
     * @name Related Documentation
     * 
     * The Experiment class integrates with multiple system components.
     */
    /// @{
    
    /**
     * @see Config for configuration management
     * @see AgentBrain for agent-specific experiment integration
     * @see Brain for neural network state
     * @see PlasticityRule for plasticity mechanism documentation
     * @see TODO Implement comprehensive experiment recording (Phase 2)
     */
    /// @}
    
private:
    /**
     * @name Implementation Details
     * 
     * PImpl idiom implementation details.
     */>
    /// @{
    
    /**
     * Forward declaration of private implementation.
     */
    struct Impl;
    
    std::unique_ptr<Impl> pImpl;  /**< Private implementation pointer */
    /// @}

}; // class Experiment

} // namespace nlm
