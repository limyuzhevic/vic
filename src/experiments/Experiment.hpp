// Experiment Framework for NLM (Neural Learning Machine)
//
// This file defines the base Experiment class for recording and managing
// experimental data in the NLM artificial brain system. The experiment framework
// provides a structured way to record, analyze, and report on brain simulation
// experiments, collecting metrics, step data, and observational notes.
//
// Experiments in NLM are used to:
// - Record brain simulation results for analysis
// - Collect performance metrics across different experimental conditions
// - Store temporal data for understanding brain dynamics
// - Organize experimental findings with metadata and notes
// - Support reproducibility and result validation
//
// The experiment framework is used by all phases of NLM:
// - Phase 3: World interaction experiments
// - Phase 4: Cognition experiments
// - Phase 5: Scaling experiments
// - Phase 6: Integration experiments
//
// Each experiment records:
// - Configuration parameters
// - Time-series data at each simulation step
// - Performance metrics
// - Observational notes
// - Random seed for reproducibility

#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>

namespace nlm {

/**
 * @brief Base experiment class for recording brain simulation data
 * 
 * The Experiment class provides a structured framework for recording, storing,
 * and analyzing experimental data from brain simulations. It serves as the
 * foundation for all NLM experiments across different phases.
 * 
 * Experiments in NLM track:
 * - Brain configuration parameters
 * - Neural activity over time (step data)
 * - Performance metrics and statistics
 * - Observational notes and comments
 * - Random seed for reproducibility
 * 
 * The experiment system is designed to be flexible and extensible,
 * supporting various types of experimental protocols and analysis.
 * 
 * @note This class follows the PIMPL pattern for implementation hiding.
 * All public interface is defined here, implementation details are
 * in the private Impl structure.
 */
class Experiment {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty experiment with default settings.
     */
    Experiment();
    
    /**
     * @brief Constructor with experiment name
     * 
     * @param name Name of the experiment
     */
    Experiment(const std::string& name);
    
    /**
     * @brief Destructor
     */
    ~Experiment();
    
    /**
     * @brief Disable copying (PIMPL pattern)
     */
    Experiment(const Experiment&) = delete;
    
    /**
     * @brief Disable assignment (PIMPL pattern)
     */
    Experiment& operator=(const Experiment&) = delete;
    
    /**
     * @brief Move constructor (enables ownership transfer)
     */
    Experiment(Experiment&&) noexcept;
    
    /**
     * @brief Move assignment operator (enables ownership transfer)
     */
    Experiment& operator=(Experiment&&) noexcept;
    
    /**
     * @brief Get experiment name
     * 
     * @return const std::string& Reference to the experiment name
     */
    const std::string& getName() const;
    
    /**
     * @brief Set experiment name
     * 
     * @param name New experiment name
     */
    void setName(const std::string& name);
    
    /**
     * @brief Get random seed
     * 
     * @return uint64_t Random seed used for the experiment
     */
    uint64_t getSeed() const;
    
    /**
     * @brief Set random seed
     * 
     * @param seed Random seed for the experiment
     */
    void setSeed(uint64_t seed);
    
    /**
     * @brief Set configuration value
     * 
     * @param key Configuration parameter name
     * @param value Configuration parameter value
     */
    void setConfigValue(const std::string& key, const std::string& value);
    
    /**
     * @brief Get configuration value
     * 
     * @param key Configuration parameter name
     * @return std::string Configuration parameter value
     */
    std::string getConfigValue(const std::string& key) const;
    
    /**
     * @brief Record metric with string name and double value
     * 
     * @param name Metric name
     * @param value Metric value
     */
    void recordMetric(const std::string& name, double value);
    
    /**
     * @brief Record metric with string name and float value
     * 
     * @param name Metric name
     * @param value Metric value
     */
    void recordMetric(const std::string& name, float value);
    
    /**
     * @brief Record metric with string name and int value
     * 
     * @param name Metric name
     * @param value Metric value
     */
    void recordMetric(const std::string& name, int value);
    
    /**
     * @brief Get all metrics
     * 
     * @return const std::vector<std::pair<std::string, double>>& Reference to metrics vector
     */
    const std::vector<std::pair<std::string, double>>& getMetrics() const;
    
    /**
     * @brief Get metric value
     * 
     * @param name Metric name
     * @return double Metric value
     */
    double getMetric(const std::string& name) const;
    
    /**
     * @brief Check if metric exists
     * 
     * @param name Metric name
     * @return bool true if metric exists, false otherwise
     */
    bool hasMetric(const std::string& name) const;
    
    /**
     * @brief Record step data
     * 
     * Records neural activity and simulation statistics at a specific step.
     * 
     * @param step Simulation step number
     * @param neuronCount Number of neurons at this step
     * @param synapseCount Number of synapses at this step
     * @param firingCount Number of firing neurons at this step
     * @param avgFiringRate Average firing rate at this step
     */
    void recordStep(SimulationStep step, size_t neuronCount, size_t synapseCount,
                   size_t firingCount, float avgFiringRate);
    
    /**
     * @brief Get step data structure
     * 
     * @note This structure contains detailed information about a single
     * simulation step for analysis and visualization.
     */
    struct StepData {
        /** @brief Simulation step number */
        SimulationStep step;
        /** @brief Number of neurons at this step */
        size_t neuronCount;
        /** @brief Number of synapses at this step */
        size_t synapseCount;
        /** @brief Number of firing neurons at this step */
        size_t firingCount;
        /** @brief Average firing rate at this step */
        float avgFiringRate;
    };
    
    /**
     * @brief Get step data
     * 
     * @return const std::vector<StepData>& Reference to step data vector
     */
    const std::vector<StepData>& getStepData() const;
    
    /**
     * @brief Get start step
     * 
     * @return SimulationStep First step number in the experiment
     */
    SimulationStep getStartStep() const;
    
    /**
     * @brief Set start step
     * 
     * @param step First step number in the experiment
     */
    void setStartStep(SimulationStep step);
    
    /**
     * @brief Get end step
     * 
     * @return SimulationStep Last step number in the experiment
     */
    SimulationStep getEndStep() const;
    
    /**
     * @brief Set end step
     * 
     * @param step Last step number in the experiment
     */
    void setEndStep(SimulationStep step);
    
    /**
     * @brief Add observational note
     * 
     * @param note Note to add to the experiment
     */
    void addNote(const std::string& note);
    
    /**
     * @brief Get notes
     * 
     * @return const std::vector<std::string>& Reference to notes vector
     */
    const std::vector<std::string>& getNotes() const;
    
private:
    /** @brief Forward declaration of implementation (PIMPL pattern) */
    struct Impl;
    /** @brief Pointer to implementation */
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm