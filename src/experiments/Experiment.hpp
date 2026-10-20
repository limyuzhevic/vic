#pragma once

/**
 * NLM Experiment Framework
 * 
 * Core experiment management and data recording system for NLM (Neural Learning Machine).
 * Provides a framework for recording neural activity, behavioral responses, and
 * experimental data during cognitive and learning experiments.
 * 
 * The Experiment class serves as the base for all NLM experiments, providing
 * a standardized way to:
 * - Record neural metrics (neuron counts, firing rates, etc.)
 * - Store experimental configuration and parameters
 * - Track experimental steps and progression
 * - Collect and analyze experimental data
 * 
 * This framework supports both simple experimental runs and complex lifetime
 * learning experiments with multiple phases and tasks.
 */

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace nlm {

/**
 * Experiment class - Core experiment management and data recording system
 * 
 * This class provides a comprehensive framework for managing experimental data
 * in NLM (Neural Learning Machine) experiments. It supports recording neural
 * activity, behavioral responses, and experimental configuration in a
 * standardized format.
 * 
 * Key features:
 * - Metadata tracking (name, seed, configuration)
 * - Metric collection (key-value pairs for flexible data recording)
 * - Step-by-step data recording (neural and behavioral metrics)
 * - Experimental progression tracking (start/end steps)
 * - Note system for experimental observations and annotations
 * - Memory-efficient pImpl pattern for performance
 * 
 * Usage example:
 * @code
 * Experiment exp("LearningExperiment");
 * exp.recordMetric("firing_rate", 0.45);
 * exp.recordMetric("spike_count", 128);
 * exp.recordStep(100, 500, 120, 0.67);
 * @endcode
 */
class Experiment {
public:
    /**
     * Default constructor
     * 
     * Creates an unnamed experiment with default settings.
     */
    Experiment();
    
    /**
     * Constructor with name
     * 
     * @param name Name of the experiment
     */
    Experiment(const std::string& name);
    
    /**
     * Destructor
     */
    ~Experiment();
    
    /**
     * Get experiment name
     * 
     * @return Reference to the experiment name
     */
    const std::string& getName() const;
    
    /**
     * Set experiment name
     * 
     * @param name New experiment name
     */
    void setName(const std::string& name);
    
    /**
     * Get experiment random seed
     * 
     * @return Random seed used for reproducibility
     */
    uint64_t getSeed() const;
    
    /**
     * Set experiment random seed
     * 
     * @param seed Random seed for reproducibility
     */
    void setSeed(uint64_t seed);
    
    /**
     * Set configuration value
     * 
     * Stores a key-value pair in the experiment configuration.
     * 
     * @param key Configuration key
     * @param value Configuration value
     */
    void setConfigValue(const std::string& key, const std::string& value);
    
    /**
     * Get configuration value
     * 
     * Retrieves a configuration value by key.
     * 
     * @param key Configuration key
     * @return Configuration value (empty string if not found)
     */
    std::string getConfigValue(const std::string& key) const;
    
    /**
     * Record a metric value
     * 
     * Records a metric with a given name and numeric value. Supports
     * double, float, and int values for flexibility.
     * 
     * @param name Metric name
     * @param value Metric value (double)
     */
    void recordMetric(const std::string& name, double value);
    
    /**
     * Record a metric value (float overload)
     * 
     * Convenience wrapper for recording float values.
     * 
     * @param name Metric name
     * @param value Metric value (float)
     */
    void recordMetric(const std::string& name, float value);
    
    /**
     * Record a metric value (int overload)
     * 
     * Convenience wrapper for recording int values.
     * 
     * @param name Metric name
     * @param value Metric value (int)
     */
    void recordMetric(const std::string& name, int value);
    
    /**
     * Get all recorded metrics
     * 
     * @return Vector of all metric name-value pairs
     */
    const std::vector<std::pair<std::string, double>>& getMetrics() const;
    
    /**
     * Get a specific metric value
     * 
     * Retrieves the value of a specific metric by name.
     * 
     * @param name Metric name
     * @return Metric value (0.0 if not found)
     */
    double getMetric(const std::string& name) const;
    
    /**
     * Check if metric exists
     * 
     * @param name Metric name to check
     * @return true if metric exists, false otherwise
     */
    bool hasMetric(const std::string& name) const;
    
    /**
     * Record step data
     * 
     * Records neural and behavioral data for a specific simulation step.
     * This is the core data recording function for experimental runs.
     * 
     * @param step Simulation step number
     * @param neuronCount Total number of neurons in network
     * @param synapseCount Total number of synapses in network
     * @param firingCount Number of neurons currently firing
     * @param avgFiringRate Average firing rate across network
     */
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                   size_t firingCount, float avgFiringRate);
    
    /**
     * Get step data
     * 
     * Retrieves all recorded step data for analysis.
     * 
     * @return Vector of step data records
     */
    const std::vector<StepData>& getStepData() const;
    
    /**
     * Get start step
     * 
     * @return First step of the experiment
     */
    SimulationStep getStartStep() const;
    
    /**
     * Set start step
     * 
     * @param step First step of the experiment
     */
    void setStartStep(SimulationStep step);
    
    /**
     * Get end step
     * 
     * @return Last step of the experiment
     */
    SimulationStep getEndStep() const;
    
    /**
     * Set end step
     * 
     * @param step Last step of the experiment
     */
    void setEndStep(SimulationStep step);
    
    /**
     * Add a note to the experiment
     * 
     * Adds an observation or annotation to the experiment record.
     * Useful for documenting experimental conditions or observations.
     * 
     * @param note Note text to add
     */
    void addNote(const std::string& note);
    
    /**
     * Get all notes
     * 
     * @return Vector of all notes added to the experiment
     */
    const std::vector<std::string>& getNotes() const;
    
private:
    /**
     * Private implementation pointer for pImpl pattern
     * 
     * Implements the pImpl pattern for better encapsulation and reduced
     * compilation dependencies. The actual implementation details are
     * hidden in this private struct.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * Step data structure for recording neural and behavioral metrics
     * 
     * Contains all the data recorded for a single simulation step,
     * including neural network statistics and behavioral measures.
     */
    struct StepData {
        SimulationStep step;           ///< Simulation step number
        size_t neuronCount;             ///< Total neurons in network
        size_t synapseCount;            ///< Total synapses in network
        size_t firingCount;             ///< Number of firing neurons
        float avgFiringRate;            ///< Average firing rate (Hz)
    };
};

} // namespace nlm
