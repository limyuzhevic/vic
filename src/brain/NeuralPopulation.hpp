#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <memory>

namespace nlm {

/**
 * @brief NeuralPopulation: A group of neurons with shared properties
 * 
 * Neural populations are fundamental computational units in the NLM brain architecture.
 * They organize neurons based on their functional properties, developmental origin,
 * or computational role. All neurons within a population share common characteristics
 * such as type, learning rules, and developmental trajectory.
 * 
 * Key features:
 * - Neurons share the same neuron type (Excitatory, Inhibitory, etc.)
 * - Enables population-level computations and statistics
 * - Supports efficient synaptic transmission patterns
 * - Facilitates population-specific plasticity rules
 * - Allows for population-level development and adaptation
 * - Reduces computational complexity through neuron grouping
 * 
 * Populations are essential for several architectural reasons:
 * - Biological plausibility: Real brains have functionally organized neuron groups
 * - Computational efficiency: Shared properties enable optimized operations
 * - Developmental realism: Regions develop distinct population types
 * - Learning efficiency: Population-level learning rules scale better
 * - Monitoring: Population statistics provide system-level insights
 */
class NeuralPopulation {
public:
    /**
     * @brief Construct a new NeuralPopulation with specified identifier and size
     * 
     * @param id Unique identifier for this population
     * @param size Number of neurons in the population
     * 
     * @note The population is initialized with neurons having the default type
     *       (Internal). Individual neuron types can be set through neuron access methods.
     *       All neurons within a population typically share similar properties,
     *       though individual neurons may have variations.
     */
    NeuralPopulation(PopulationId id, size_t size);
    
    /**
     * @brief Destroy the NeuralPopulation instance
     * 
     * @note Ensures proper cleanup of all contained neurons and allocated memory.
     *       The population should be in a clean state (typically reset) before
     *       destruction to avoid memory leaks.
     */
    ~NeuralPopulation();
    
    // Disable copying, enable moving for efficient resource management
    NeuralPopulation(const NeuralPopulation&) = delete;
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;
    NeuralPopulation(NeuralPopulation&&) noexcept;
    NeuralPopulation& operator=(NeuralPopulation&&) noexcept;
    
    /**
     * @brief Get the population's unique identifier
     * 
     * @return PopulationId The population's identifier
     * 
     * @note Used for system-wide operations, population management,
     *       and population-level statistics.
     */
    PopulationId getId() const;
    
    /**
     * @brief Get the number of neurons in this population
     * 
     * @return size_t Total neuron count
     * 
     * @note This represents the current number of neurons in the population.
     *       The count may change if neurons are added or removed through
     *       population management operations.
     */
    size_t getSize() const;
    
    /**
     * @brief Check if the population is empty
     * 
     * @return bool true if population has no neurons, false otherwise
     * 
     * @note Useful for checking population state before operations that
     *       require neurons to be present.
     */
    bool isEmpty() const;
    
    // -------------------------------------------------------------------------
    // NEURON ACCESS AND MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get a reference to a specific neuron by index
     * 
     * @param index Index of the neuron within the population (0..size-1)
     * @return NeuralPopulation* Pointer to the requested neuron
     * 
     * @note Neurons are owned by the population, so this returns pointers
     *       rather than creating new neurons. Use with caution to avoid
     *       creating ownership conflicts.
     */
    NeuralPopulation* getNeuron(NeuronIndex index);
    
    /**
     * @brief Get constant reference to a specific neuron by index
     * 
     * @param index Index of the neuron within the population (0..size-1)
     * @return const NeuralPopulation* Const pointer to the requested neuron
     * 
     * @note Thread-safe read access. Useful for read-only operations that need
     *       to access neuron state without modification.
     */
    const NeuralPopulation* getNeuron(NeuronIndex index) const;
    
    /**
     * @brief Add an existing neuron to the population
     * 
     * @param neuron Pointer to the neuron to add
     * 
     * @note This method allows dynamic population modification. The neuron
     *       must not already belong to another population. Adding neurons
     *       changes the population size and requires updating neuron indices.
     */
    void addNeuron(Neuron* neuron);
    
    /**
     * @brief Get all neurons in the population
     * 
     * @return const std::vector<Neuron*>& Reference to neuron collection
     * 
     * @note Provides access to all neurons for population-level operations.
     *       Neurons are owned by the population, so this returns references
     *       to the actual stored neurons.
     */
    const std::vector<Neuron*>& getNeurons() const;
    
    // -------------------------------------------------------------------------
    // POPULATION TYPE AND PROPERTIES
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the neuron type shared by all neurons in this population
     * 
     * @return NeuronType The neuron type for all neurons in the population
     * 
     * @note All neurons in a population share the same type, which determines
     *       their computational properties and behavior. This enables
     *       population-level optimizations and computations.
     */
    NeuronType getNeuronType() const;
    
    /**
     * @brief Set the neuron type for all neurons in the population
     * 
     * @param type New neuron type for all neurons
     * 
     * @note This method changes the type of all neurons in the population.
     *       This operation may have significant effects on network dynamics
     *       and should be used with caution.
     */
    void setNeuronType(NeuronType type);
    
    // -------------------------------------------------------------------------
    // STATISTICS AND MONITORING
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the number of active neurons in the population
     * 
     * @return size_t Count of neurons with firing state other than Resting
     */
    size_t getActiveCount() const;
    
    /**
     * @brief Get the number of neurons currently firing
     * 
     * @return size_t Count of neurons in Active firing state
     */
    size_t getFiringCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons in the population
     * 
     * @return float Average firing rate in Hertz (spikes per second)
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get the activity level of the population
     * 
     * @return float Fraction of active neurons (0.0 to 1.0)
     * 
     * @note Activity level provides a normalized measure of population engagement,
     *       useful for comparing populations of different sizes or monitoring
     *       overall brain activity levels.
     */
    float getActivityLevel() const;  // fraction of active neurons
    
    // -------------------------------------------------------------------------
    // SIMULATION OPERATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Initialize all neurons with random parameters
     * 
     * @param rng Reference to random number generator
     * 
     * @note This method sets neuron-specific parameters (membrane potential,
     *       threshold, conductances, etc.) using random values from the provided
     *       generator. This is typically called during brain initialization
     *       to create variability across neurons.
     */
    void initializeRandom(class RandomGenerator& rng);
    
    /**
     * @brief Execute one simulation step for all neurons in the population
     * 
     * @param currentTime Current simulation time (seconds)
     * 
     * @note This performs the core neural computation cycle for all neurons
     *       in the population, including:
     *       - LIF neuron state updates and spike generation
     *       - Synaptic input processing
     *       - Population-level statistics computation
     *       
     * @note Population stepping is typically called by the Brain class
     *       as part of the overall simulation pipeline.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset all neurons in the population
     * 
     * @note Clears all neural activity, neuron state, and internal counters.
     *       Resets neurons to resting state with default parameters.
     *       Used to prepare populations for new simulation runs.
     */
    void reset();
    
    // -------------------------------------------------------------------------
    // DATA ACCESS AND ANALYSIS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the neuron identifiers for all neurons in this population
     * 
     * @return std::vector<NeuronId> Vector of neuron identifiers
     * 
     * @note Useful for system-wide operations that need to identify neurons
     *       belonging to specific populations. The order of IDs corresponds
     *       to neuron indices within the population.
     */
    std::vector<NeuronId> getNeuronIds() const;
    
    /**
     * @brief Record current activity statistics for the population
     * 
     * @note This method captures the current state of the population for
     *       later analysis, debugging, or recording. It stores metrics such as
     *       activity levels, firing rates, and other statistical measures.
     */
    void recordActivity();  // Record current state for statistics
    
    /**
     * @brief Get the mean membrane potential across all neurons
     * 
     * @return float Mean membrane potential (mV)
     * 
     * @note Provides a population-level measure of overall membrane potential
     *       state, useful for monitoring brain dynamics and detecting population-level
     *       shifts in excitability.
     */
    float getMeanMembranePotential() const;
    
    /**
     * @brief Get the variance of membrane potential across all neurons
     * 
     * @return float Variance of membrane potential (mV²)
     * 
     * @note Measures the variability in membrane potential across the population,
     *       which reflects the diversity of neuronal states and network dynamics.
     */
    float getVarianceMembranePotential() const;
    
private:
    struct Impl;
    Impl* pImpl;  ///< Private implementation for PIMPL pattern
};

} // namespace nlm