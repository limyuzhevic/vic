#pragma once

/**
 * @file NeuralPopulation.hpp
 * @brief Neural population interface - groups neurons with shared properties and functions
 * @author NLM Team
 * @date 2024
 * @version 1.0
 * @warning Phase 1 implementation. Full population-level dynamics implementation planned for Phase 2.
 */

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <memory>

namespace nlm {

/**
 * @namespace nlm
 * @brief Neural Machine Learning (NLM) - A spiking neural network framework for cognitive computing
 *
 * This namespace encompasses the entire NLM ecosystem, from low-level neuron dynamics to
 * high-level cognitive functions including memory, prediction, attention, and development.
 */

/**
 * @class NeuralPopulation
 * @brief A group of neurons with shared properties and collective dynamics
 * @details Neural populations organize neurons that process information within
 *          specific functional domains. All neurons in a population share the same
 *          type and often have similar connectivity patterns. Populations enable
 *          efficient computation by exploiting parallel processing across neuron groups.
 *
 * Key features:
 * - Neuron management: create, access, and manipulate individual neurons
 * - Population statistics: compute activity, firing rates, and membrane potential metrics
 * - Step processing: update all neurons in parallel
 * - Random initialization: set up neurons with biologically plausible parameters
 * - Identity tracking: maintain population and neuron IDs for connectivity management
 * - Activity recording: track population statistics for analysis and monitoring
 *
 * Populations are the intermediate level of organization between individual neurons
 * and brain regions. They contain multiple neurons that work together to perform
 * specific computational functions within the larger neural network.
 */

class NeuralPopulation {
public:
    /**
     * @brief Constructs a neural population with ID and size
     * @param id Unique identifier for the population
     * @param size Number of neurons in the population
     * @details Creates a new neural population instance with the specified number of neurons.
     *          All neurons in the population start with the default Internal type.
     *          The population ID is used for connectivity management and identification.
     * @pre The population ID should be unique within its region
     * @warning Population IDs are assigned sequentially starting from 1.
     */
    NeuralPopulation(PopulationId id, size_t size);
    
    /**
     * @brief Destructor cleans up population resources
     * @details Deletes the private implementation (pImpl) and cleans up
     *          any allocated resources. This ensures proper memory management.
     */
    ~NeuralPopulation();
    
    /**
     * @brief Disables copying (population is not copyable)
     */
    NeuralPopulation(const NeuralPopulation&) = delete;
    
    /**
     * @brief Disables copy assignment
     */
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;
    
    /**
     * @brief Enables move construction (efficient transfer of ownership)
     */
    NeuralPopulation(NeuralPopulation&&) noexcept;
    
    /**
     * @brief Enables move assignment (efficient transfer of ownership)
     */
    NeuralPopulation& operator=(NeuralPopulation&&) noexcept;
    
    /**
     * @brief Gets the population's unique identifier
     * @return PopulationId The population's unique ID
     */
    PopulationId getId() const;
    
    /**
     * @brief Gets the number of neurons in the population
     * @return size_t Number of neurons
     */
    size_t getSize() const;
    
    /**
     * @brief Checks if the population is empty
     * @return bool true if population contains no neurons, false otherwise
     */
    bool isEmpty() const;
    
    /**
     * @brief Accesses a neuron by index
     * @param index Index of the neuron to retrieve (0-based)
     * @return Neuron* Pointer to the neuron, or nullptr if index is invalid
     * @details Retrieves the neuron at the specified index. Returns nullptr if
     *          the index is out of bounds.
     * @warning Indices must be within [0, size()-1].
     */
    Neuron* getNeuron(NeuronIndex index);
    
    /**
     * @brief Accesses a neuron by index (const version)
     * @param index Index of the neuron to retrieve (0-based)
     * @return const Neuron* Const pointer to the neuron, or nullptr if index is invalid
     */
    const Neuron* getNeuron(NeuronIndex index) const;
    
    /**
     * @brief Adds a neuron to the population
     * @param neuron Pointer to the neuron to add
     * @details Adds the specified neuron to the population. The neuron is stored
     *          and becomes part of the population's collective dynamics.
     * @pre The neuron should not already be in a different population
     * @warning Neurons should be properly initialized before adding to population.
     */
    void addNeuron(Neuron* neuron);
    
    /**
     * @brief Gets all neurons in the population
     * @return const std::vector<Neuron*>& Reference to internal neuron vector
     * @note Returns a reference to the internal vector of neurons. For read-only
     *       access, const version is preferred.
     */
    const std::vector<Neuron*>& getNeurons() const;
    
    /**
     * @brief Gets the neuron type for all neurons in the population
     * @return NeuronType The neuron type (all neurons share the same type)
     * @details Returns the type that all neurons in the population have.
     *          This determines how the population processes information.
     * @note All neurons in a population must have the same type.
     */
    NeuronType getNeuronType() const;
    
    /**
     * @brief Sets the neuron type for all neurons in the population
     * @param type Neuron type to set for all neurons
     * @details Updates the type of all neurons in the population. This affects
     *          how the population processes inputs and generates outputs.
     * @note All neurons in a population must be of the same type.
     * @see NeuronType enum for available types
     */
    void setNeuronType(NeuronType type);
    
    /**
     * @brief Gets the number of active neurons in the population
     * @return size_t Number of neurons currently firing or active
     * @details Active neurons are those that are currently firing (spiking)
     *          or in the active state but not yet refractory.
     */
    size_t getActiveCount() const;
    
    /**
     * @brief Gets the number of firing neurons in the population
     * @return size_t Number of neurons in firing state
     * @details Firing neurons are those that have crossed threshold and are
     *          currently in the firing state.
     */
    size_t getFiringCount() const;
    
    /**
     * @brief Calculates the average firing rate across all neurons
     * @return float Average firing rate (Hz)
     * @details Computes the mean firing rate of all neurons in the population,
     *          not a global average of individual neuron firing rates.
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Gets the activity level of the population
     * @return float Activity level as fraction of active neurons (0.0 to 1.0)
     * @details Computes the ratio of active neurons to total neurons in the population.
     *          This is a normalized measure of population activity.
     * @warning Returns 0.0 if population is empty.
     */
    float getActivityLevel() const;  // fraction of active neurons
    
    /**
     * @brief Initializes all neurons with random parameters
     * @param rng Reference to random number generator
     * @details Sets up all neurons in the population with biologically plausible parameters:
     *          - Membrane potential near resting potential
     *          - Firing threshold at typical values (-55 mV)
     *          - Resting potential at typical values (-70 mV)
     *          - Leak conductance and refractory period within biological ranges
     *          - Initial firing state and spike history
     * @note This is used during network initialization to create realistic neurons.
     */
    void initializeRandom(class RandomGenerator& rng);
    
    /**
     * @brief Steps all neurons in the population
     * @param currentTime The current simulation time (seconds)
     * @details Updates all neurons in the population for one simulation step:
     *          - Processes neuron LIF dynamics
     *          - Handles spike events
     *          - Updates synaptic transmission
     *          - Manages short-term plasticity
     * @note This is called by NeuralRegion::step() for all populations.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets all neurons to initial state
     * @details Resets all neurons in the population to their initial states:
     *          - Membrane potential to resting potential
     *          - Firing state to resting
     *          - Spike history cleared
     *          - Refractory counters reset
     * @note Useful for episode termination and state management.
     */
    void reset();
    
    /**
     * @brief Gets all neuron IDs in the population
     * @return std::vector<NeuronId> Vector of neuron IDs
     * @details Collects the IDs of all neurons in the population.
     *          Used for connectivity queries and debugging.
     */
    std::vector<NeuronId> getNeuronIds() const;
    
    /**
     * @brief Records current activity for statistics
     * @details Records the current state of all neurons for later statistical analysis.
     *          This is used for computing population-level metrics like mean and variance
     *          of membrane potentials.
     * @note Called during step processing to maintain activity statistics.
     */
    void recordActivity();  // Record current state for statistics
    
    /**
     * @brief Gets the mean membrane potential of all neurons
     * @return float Mean membrane potential in millivolts (mV)
     * @details Computes the average membrane potential across all neurons in the population.
     *          Useful for monitoring population-level excitability.
     */
    float getMeanMembranePotential() const;
    
    /**
     * @brief Gets the variance of membrane potential across neurons
     * @return float Variance of membrane potentials in mV²
     * @details Measures the spread of membrane potentials across neurons in the population.
     *          Higher variance indicates greater diversity in neuronal states.
     */
    float getVarianceMembranePotential() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
