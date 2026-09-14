#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <memory>

namespace nlm {

// NeuralPopulation: A group of neurons with shared properties
// PLACEHOLDER - Phase 2 will implement population-level dynamics

class NeuralPopulation {
public:
    /**
     * @brief Construct a neural population with identifier and size
     * 
     * Creates a new neural population with the specified number of neurons.
     * The population starts empty and neurons must be added before it can
     * participate in neural computation.
     * 
     * @param id Unique identifier for this population within the brain
     * @param size Initial number of neurons to allocate for the population
     * @pre id must be valid and not represent a reserved population ID
     * @post Population object is created with specified size
     * @post Population ID is set
     * @post Population type is set to Internal by default
     * @post Population is initialized with empty neuron vector
     * 
     * @note Population IDs should be unique within a brain region
     * @see PopulationId for identifier structure
     */
    NeuralPopulation(PopulationId id, size_t size);
    
    /**
     * @brief Destroy the neural population and clean up all resources
     * 
     * This destructor ensures proper cleanup of all population resources,
     * including deletion of all neurons and removal from parent region.
     * The population is removed from the region's collection of populations.
     * 
     * @post All neurons are destroyed
     * @post Population is removed from parent region
     * @post Memory is properly deallocated
     */
    ~NeuralPopulation();
    
    // Disable copying, enable moving
    NeuralPopulation(const NeuralPopulation&) = delete;
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;
    NeuralPopulation(NeuralPopulation&&) noexcept;
    NeuralPopulation& operator=(NeuralPopulation&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this population
     * 
     * @return The population's identifier
     * @pre Population object must be valid
     * @post Identifier is returned unchanged
     */
    PopulationId getId() const;
    
    /**
     * @brief Get the current size of this population
     * 
     * Returns the number of neurons currently in the population.
     * This may be less than the initial allocation if neurons have been
     * removed, or greater if new neurons have been added.
     * 
     * @return Current number of neurons in the population
     * @pre Population object must be valid
     * @post Returns current population size
     */
    size_t getSize() const;
    
    /**
     * @brief Check if the population is empty
     * 
     * @return true if population has no neurons, false otherwise
     * @pre Population object must be valid
     * @post Returns emptiness status
     */
    bool isEmpty() const;
    
    /**
     * @brief Get a reference to a specific neuron by index
     * 
     * Provides access to a neuron by its index position within the population's
     * internal vector. This allows direct manipulation of individual neurons
     * for performance-critical operations.
     * 
     * @param index Index of the neuron to retrieve (0 <= index < size)
     * @return Pointer to the Neuron instance, or nullptr if index is invalid
     * @pre Population object must be valid
     * @post Returns reference to requested neuron
     * 
     * @note Index is population-local, not a global neuron ID
     * @see getNeuron() for const version
     */
    Neuron* getNeuron(NeuronIndex index);
    
    /**
     * @brief Get a const reference to a specific neuron by index
     * 
     * Const version of getNeuron() for read-only access to neuron data.
     * 
     * @param index Index of the neuron to retrieve
     * @return Const pointer to the Neuron instance, or nullptr if index is invalid
     * @pre Population object must be valid
     * @post Returns const reference to requested neuron
     */
    const Neuron* getNeuron(NeuronIndex index) const;
    
    /**
     * @brief Add a neuron to this population
     * 
     * Registers a neuron as belonging to this population. The neuron is
     * added to the population's internal vector and marked as a member
     * of this population. This is used for population-level organization
     * and management.
     * 
     * @param neuron Pointer to the neuron to add to the population
     * @pre neuron must be a valid neuron pointer
     * @pre neuron must not already belong to another population
     * @post Neuron is added to population's neuron vector
     * @post Population size is incremented
     * @post Neuron maintains its neural properties and state
     * 
     * @note Neurons can belong to only one population at a time
     * @see getNeuron() for accessing neurons
     */
    void addNeuron(Neuron* neuron);
    
    /**
     * @brief Get all neurons in this population
     * 
     * Returns a const reference to the vector of all neurons in the population.
     * The neurons are owned by this population and should not be deleted externally.
     * 
     * @return Const reference to vector of all neurons
     * @pre Population object must be valid
     * @post Returns reference to all neurons
     */
    const std::vector<Neuron*>& getNeurons() const;
    
    /**
     * @brief Get the neuron type for this population
     * 
     * All neurons in a population share the same type classification.
     * Population type determines the functional role and characteristics
     * of all neurons within it.
     * 
     * @return Neuron type for this population
     * @pre Population object must be valid
     * @post Returns population type
     */
    NeuronType getNeuronType() const;
    
    /**
     * @brief Set the neuron type for this population
     * 
     * Changes the type classification for all neurons in this population.
     * This affects the functional role and behavior of all neurons in the
     * population. All neurons are updated to the new type.
     * 
     * @param type The new neuron type to assign to all neurons
     * @pre Population object must be valid
     * @post All neurons are updated to the new type
     */
    void setNeuronType(NeuronType type);
    
    /**
     * @brief Get the number of active neurons in this population
     * 
     * @return Count of neurons in Active or Refractory firing state
     * @pre Population object must be valid
     * @post Returns active neuron count
     */
    size_t getActiveCount() const;
    
    /**
     * @brief Get the number of neurons currently firing spikes in this population
     * 
     * @return Count of neurons in Refractory state (just fired)
     * @pre Population object must be valid
     * @post Returns firing neuron count
     */
    size_t getFiringCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons in this population
     * 
     * Computes the mean firing rate (in Hz) of all neurons in the population,
     * providing a population-level measure of neural activity.
     * 
     * @return Average firing rate in Hz
     * @pre Population object must be valid
     * @post Returns computed average
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get the activity level of this population
     * 
     * Computes the fraction of neurons in the population that are currently
     * active (in firing or refractory state). This provides a normalized
     * measure of population-level activity.
     * 
     * @return Activity level (0.0 to 1.0)
     * @pre Population object must be valid
     * @post Returns computed activity level
     */
    float getActivityLevel() const;  // fraction of active neurons
    
    /**
     * @brief Initialize all neurons in this population randomly
     * 
     * This method sets random parameters for all neurons in the population,
     * creating heterogeneity within the population while maintaining the
     * same neuron type for all members.
     * 
     * @param rng Reference to the random number generator
     * @pre Population object must be valid
     * @post All neurons in population are initialized with random parameters
     * @post Membrane potentials are set to resting values
     * @post Thresholds, conductances, and periods are randomized
     * @post All neurons receive unique random seeds
     */
    void initializeRandom(class RandomGenerator& rng);
    
    /**
     * @brief Perform one simulation step for all neurons in this population
     * 
     * This method advances all neurons in the population by one simulation
     * step, coordinating their individual computations and interactions.
     * This is the core computational method for population-level processing.
     * 
     * @param currentTime Current simulation time in seconds
     * @pre Population object must be valid
     * @post All neurons in population are updated
     * @post Population-level dynamics are computed
     * @post Activity metrics are updated
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset all neurons in this population to their initial states
     * 
     * This method returns all neurons in the population to their default
     * states as specified in their constructors. This clears all activity,
     * resets potentials, and clears synaptic modifications.
     * 
     * @pre Population object must be valid
     * @post All neurons are reset to initial state
     * @post Activity is cleared
     * @post Synaptic states are reset
     */
    void reset();
    
    /**
     * @brief Get the list of neuron IDs in this population
     * 
     * Returns a vector of neuron identifiers for all neurons in the population.
     * This provides global identification of neurons within the population
     * without exposing the internal implementation details.
     * 
     * @return Vector of neuron identifiers
     * @pre Population object must be valid
     * @post Returns list of neuron IDs
     */
    std::vector<NeuronId> getNeuronIds() const;
    
    /**
     * @brief Record current activity for statistics
     * 
     * This method captures the current state of the population for
     * statistical analysis and monitoring. It records relevant metrics
     * such as membrane potentials, firing rates, and activity patterns.
     * 
     * @pre Population object must be valid
     * @post Current activity is recorded for statistics
     */
    void recordActivity();  // Record current state for statistics
    
    /**
     * @brief Get the mean membrane potential across all neurons
     * 
     * Computes the average membrane potential of all neurons in the population,
     * providing a population-level measure of neural excitation/inhibition.
     * 
     * @return Mean membrane potential in millivolts
     * @pre Population object must be valid
     * @post Returns computed mean
     */
    float getMeanMembranePotential() const;
    
    /**
     * @brief Get the variance of membrane potentials across all neurons
     * 
     * Computes the variance of membrane potentials in the population,
     * providing a measure of population-level neural diversity and synchronization.
     * 
     * @return Variance of membrane potentials
     * @pre Population object must be valid
     * @post Returns computed variance
     */
    float getVarianceMembranePotential() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
