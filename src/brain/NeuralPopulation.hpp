# NeuralPopulation.hpp - Comprehensive documentation for NeuralPopulation class

#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <memory>

namespace nlm {

/**
 * @class NeuralPopulation
 * @brief A group of neurons with shared properties and dynamics
 * 
 * NeuralPopulation represents a collection of neurons that share common characteristics
 * and behavior patterns. This abstraction allows for more efficient computation by
 * processing groups of neurons with similar properties together, which is particularly
 * useful for implementing population-based learning rules and large-scale neural
 * network simulations.
 * 
 * Populations are fundamental building blocks in the NLM architecture, representing
 * groups such as sensory neurons, motor neurons, or internal processing populations.
 * Each population contains neurons of the same type, and can be independently
 * initialized, stepped, and reset.
 * 
 * @note Currently implementing Phase 2: Real Neural Computation
 *       This class is under active development with additional features planned.
 */
class NeuralPopulation {
public:
    /**
     * @brief Create a new neural population with the specified ID and size
     * 
     * @param id Unique identifier for this population
     * @param size Number of neurons to create in this population
     * @throw std::invalid_argument if size is zero or negative
     */
    NeuralPopulation(PopulationId id, size_t size);
    
    /**
     * @brief Destructor
     * 
     * Properly cleans up internal resources and frees memory.
     */
    ~NeuralPopulation();
    
    // === Core Operations ===
    
    /**
     * @brief Disable copy constructor (neural populations cannot be copied)
     * 
     * Neural populations manage dynamic memory for neurons and cannot be safely
     * copied. Use move semantics instead.
     */
    NeuralPopulation(const NeuralPopulation&) = delete;
    
    /**
     * @brief Disable copy assignment operator
     * 
     * See copy constructor note above.
     */
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Transfers ownership of internal resources from another population.
     * The source population is left in a valid but empty state.
     * 
     * @param other The population to move from
     */
    NeuralPopulation(NeuralPopulation&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Transfers ownership of internal resources from another population.
     * The source population is left in a valid but empty state.
     * 
     * @param other The population to move from
     * @return Reference to this population (for chaining)
     */
    NeuralPopulation& operator=(NeuralPopulation&&) noexcept;
    
    // === Identity and Basic Properties ===
    
    /**
     * @brief Get the unique identifier of this population
     * 
     * @return PopulationId Unique identifier for this population
     */
    PopulationId getId() const;
    
    /**
     * @brief Get the number of neurons in this population
     * 
     * @return size_t Number of neurons currently in this population
     */
    size_t getSize() const;
    
    /**
     * @brief Check if this population contains any neurons
     * 
     * @return bool true if population is empty, false otherwise
     */
    bool isEmpty() const;
    
    // === Neuron Access and Management ===
    
    /**
     * @brief Get a mutable reference to a neuron by its index
     * 
     * @param index Zero-based index of the neuron to retrieve
     * @return Neuron* Pointer to the neuron, or nullptr if index is out of bounds
     */
    Neuron* getNeuron(NeuronIndex index);
    
    /**
     * @brief Get a const reference to a neuron by its index
     * 
     * @param index Zero-based index of the neuron to retrieve
     * @return const Neuron* Const pointer to the neuron, or nullptr if index is out of bounds
     */
    const Neuron* getNeuron(NeuronIndex index) const;
    
    /**
     * @brief Add an existing neuron to this population
     * 
     * This allows for dynamic population management by adding neurons after
     * population creation. The neuron takes ownership of the population reference.
     * 
     * @param neuron Pointer to the neuron to add (must not be nullptr)
     */
    void addNeuron(Neuron* neuron);
    
    /**
     * @brief Get all neurons in this population
     * 
     * @return const std::vector<Neuron*>& Vector of all neuron pointers in this population
     */
    const std::vector<Neuron*>& getNeurons() const;
    
    // === Population Type Management ===
    
    /**
     * @brief Get the neuron type for this population
     * 
     * All neurons in a population share the same type. This type determines
     * their computational properties and behavior.
     * 
     * @return NeuronType The type of neurons in this population
     */
    NeuronType getNeuronType() const;
    
    /**
     * @brief Set the neuron type for this population
     * 
     * Changes the type of all neurons in this population to the specified type.
     * This affects their computational properties and behavior.
     * 
     * @param type The new neuron type to assign to all neurons in this population
     */
    void setNeuronType(NeuronType type);
    
    // === Statistics and Monitoring ===
    
    /**
     * @brief Get the number of active neurons in this population
     * 
     * Active neurons are those that are currently firing or have a firing rate > 0.
     * 
     * @return size_t Number of active neurons
     */
    size_t getActiveCount() const;
    
    /**
     * @brief Get the number of currently firing neurons in this population
     * 
     * Firing neurons are those that have just crossed their firing threshold
     * during the current simulation step.
     * 
     * @return size_t Number of firing neurons
     */
    size_t getFiringCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons in this population
     * 
     * @return float Average firing rate in Hz, or 0.0 if no neurons present
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get the activity level as a fraction of active neurons
     * 
     * This provides a normalized measure of population activity, useful for
     * monitoring and control algorithms.
     * 
     * @return float Fraction of active neurons (0.0 to 1.0), or 0.0 if empty
     */
    float getActivityLevel() const;  // fraction of active neurons
    
    /**
     * @brief Get the average membrane potential across all neurons
     * 
     * @return float Average membrane potential in mV, or 0.0 if no neurons
     */
    float getMeanMembranePotential() const;
    
    /**
     * @brief Get the variance of membrane potentials across neurons
     * 
     * This measures the heterogeneity of membrane potentials within the population,
     * which can indicate different states of activation or synchronization.
     * 
     * @return float Variance of membrane potentials, or 0.0 if insufficient data
     */
    float getVarianceMembranePotential() const;
    
    // === Simulation Operations ===
    
    /**
     * @brief Initialize all neurons in this population with random parameters
     * 
     * This method sets up each neuron with biologically plausible random values
     * for membrane potential, threshold, refractory period, and other parameters.
     * This provides diversity within the population while maintaining consistency
     * with neural dynamics.
     * 
     * @param rng Reference to random number generator for reproducible initialization
     */
    void initializeRandom(class RandomGenerator& rng);
    
    /**
     * @brief Advance all neurons in this population by one simulation step
     * 
     * This method propagates the LIF (Leaky Integrate-and-Fire) dynamics for each
     * neuron, updating their membrane potentials, detecting spikes, and applying
     * any synaptic inputs or modulatory effects.
     * 
     * @param currentTime Current simulation time in seconds
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset all neurons in this population to their initial state
     * 
     * This clears all spiking activity, resets membrane potentials, and prepares
     * the neurons for a new simulation cycle or experiment.
     */
    void reset();
    
    // === Neuron Identity Management ===
    
    /**
     * @brief Get the IDs of all neurons in this population
     * 
     * @return std::vector<NeuronId> Vector of neuron IDs in this population
     */
    std::vector<NeuronId> getNeuronIds() const;
    
    // === Statistics and Recording ===
    
    /**
     * @brief Record current activity statistics for this population
     * 
     * This method captures the current state of the population for analysis,
     * metrics collection, or later replay operations. It stores snapshots of
     * key variables for statistical analysis.
     */
    void recordActivity();  // Record current state for statistics
    
private:
    /**
     * @brief Internal implementation structure (pImpl pattern)
     * 
     * This uses the pImpl idiom to hide implementation details from the header file,
     * reducing compilation dependencies and improving encapsulation.
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     * 
     * Managed pointer to the internal implementation structure that contains
     * the actual data and method implementations.
     */
    Impl* pImpl;
};

} // namespace nlm
