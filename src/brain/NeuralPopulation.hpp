#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace nlm {

/**
 * @class NeuralPopulation
 * @brief A group of neurons with shared properties and population-level dynamics
 * 
 * NeuralPopulation encapsulates a collection of neurons that share common
 * characteristics such as type, initial conditions, and learning parameters.
 * It provides population-level operations like statistical analysis,
 * coordinated activation, and synchronized updates that are not possible
 * at the individual neuron level.
 * 
 * The population maintains its own state and behavior while providing
 * access to individual neurons when needed. This abstraction enables
 * more efficient implementation of population-based neural dynamics,
 * learning rules, and computational neuroscience models.
 * 
 * @note Currently in Phase 2 implementation - population-level dynamics
 *       are placeholder functionality to be enhanced in future phases.
 * 
 * @see Neuron For individual neuron implementation
 * @see NeuralRegion Neurons are organized into populations within regions
 */
class NeuralPopulation {
public:
    /**
     * @brief Construct a new NeuralPopulation with specified ID and size
     * 
     * Initializes an empty population with the given identifier and capacity.
     * Neurons are added to the population via the addNeuron method.
     * 
     * @param id Unique identifier for this population
     * @param size Maximum number of neurons this population can hold
     * @throws std::invalid_argument if size is zero or negative
     */
    explicit NeuralPopulation(PopulationId id, size_t size);

    /**
     * @brief Destructor
     * 
     * Properly cleans up all neuron pointers and statistics data
     * maintained by the population.
     */
    ~NeuralPopulation();

    // Rule of five: delete copy operations, enable move operations
    /**
     * @brief Deleted copy constructor
     * 
     * Populations cannot be copied due to the pImpl pattern and neuron ownership.
     * Use move operations instead for transferring population ownership.
     */
    NeuralPopulation(const NeuralPopulation&) = delete;
    
    /**
     * @brief Deleted copy assignment operator
     * 
     * See copy constructor for details.
     */
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;

    /**
     * @brief Move constructor
     * 
     * Transfers ownership of the internal implementation and neuron pointers
     * from another population to this one.
     * 
     * @param other The population to move from
     */
    NeuralPopulation(NeuralPopulation&&) noexcept;

    /**
     * @brief Move assignment operator
     * 
     * Transfers ownership from another population, properly cleaning up
     * any existing resources.
     * 
     * @param other The population to move from
     * @return Reference to this population after the move
     */
    NeuralPopulation& operator=(NeuralPopulation&&) noexcept;

    /**
     * @brief Get the unique identifier of this population
     * 
     * @return PopulationId The unique identifier
     */
    PopulationId getId() const;

    /**
     * @brief Get the current number of neurons in the population
     * 
     * @return size_t Number of neurons currently stored
     */
    size_t getSize() const;

    /**
     * @brief Check if the population contains any neurons
     * 
     * @return bool True if population is empty, false otherwise
     */
    bool isEmpty() const;

    /**
     * @brief Get a reference to a specific neuron by index
     * 
     * @param index Index of the neuron to retrieve (0-based)
     * @return Neuron* Pointer to the neuron, or nullptr if index out of range
     */
    Neuron* getNeuron(NeuronIndex index);

    /**
     * @brief Get a const reference to a specific neuron by index
     * 
     * @param index Index of the neuron to retrieve (0-based)
     * @return const Neuron* Const pointer to the neuron, or nullptr if index out of range
     */
    const Neuron* getNeuron(NeuronIndex index) const;

    /**
     * @brief Add a neuron to the population
     * 
     * Takes ownership of the neuron pointer. The population assumes responsibility
     * for managing the neuron's lifetime.
     * 
     * @param neuron Pointer to the neuron to add
     * @throws std::runtime_error if neuron pointer is null
     */
    void addNeuron(Neuron* neuron);

    /**
     * @brief Get all neurons in the population
     * 
     * @return const std::vector<Neuron*>& Vector of neuron pointers (non-owning)
     */
    const std::vector<Neuron*>& getNeurons() const;

    /**
     * @brief Get the neuron type of all neurons in this population
     * 
     * All neurons in a population share the same type after initialization.
     * This can be changed to modify the type of all neurons in the population.
     * 
     * @return NeuronType The neuron type
     */
    NeuronType getNeuronType() const;

    /**
     * @brief Set the neuron type for all neurons in the population
     * 
     * Changes the type of all neurons in the population. This affects
     * synaptic dynamics, plasticity rules, and other type-dependent behaviors.
     * 
     * @param type New neuron type for all neurons
     */
    void setNeuronType(NeuronType type);

    /**
     * @brief Get the number of active neurons in the population
     * 
     * @return size_t Count of neurons that are currently firing or have recent activity
     */
    size_t getActiveCount() const;

    /**
     * @brief Get the number of firing neurons in the population
     * 
     * @return size_t Count of neurons that are currently in the firing state
     */
    size_t getFiringCount() const;

    /**
     * @brief Calculate the average firing rate across all neurons
     * 
     * @return float Average firing rate (spikes per unit time)
     */
    float getAverageFiringRate() const;

    /**
     * @brief Calculate the activity level of the population
     * 
     * @return float Fraction of active neurons (0.0 to 1.0)
     */
    float getActivityLevel() const;

    /**
     * @brief Initialize all neurons with random parameters
     * 
     * Sets up initial conditions for all neurons in the population using
     * the provided random number generator. This typically includes setting
     * initial membrane potentials, synaptic weights, and other parameters.
     * 
     * @param rng Random number generator for stochastic initialization
     */
    void initializeRandom(class RandomGenerator& rng);

    /**
     * @brief Perform a simulation step for all neurons in the population
     * 
     * Advances the state of all neurons by one time step, updating membrane
     * potentials, detecting spikes, and managing neuronal dynamics according
     * to the population's properties.
     * 
     * @param currentTime Current simulation time
     */
    void step(Timestamp currentTime);

    /**
     * @brief Reset all neurons to their initial state
     * 
     * Clears any accumulated state, statistics, and activity from all neurons
     * in the population, preparing them for a fresh initialization.
     */
    void reset();

    /**
     * @brief Get the IDs of all neurons in the population
     * 
     * @return std::vector<NeuronId> Vector of neuron identifiers
     */
    std::vector<NeuronId> getNeuronIds() const;

    /**
     * @brief Record current neural activity for statistical analysis
     * 
     * Captures the current membrane potentials of all neurons for later
     * statistical analysis, such as calculating mean and variance.
     */
    void recordActivity();

    /**
     * @brief Calculate the mean membrane potential across all neurons
     * 
     * Uses recorded activity data from the most recent recordActivity() call.
     * 
     * @return float Mean membrane potential
     */
    float getMeanMembranePotential() const;

    /**
     * @brief Calculate the variance of membrane potentials
     * 
     * Uses recorded activity data from the most recent recordActivity() call.
     * 
     * @return float Variance of membrane potentials
     */
    float getVarianceMembranePotential() const;

private:
    /**
     * @brief Internal implementation structure using pImpl pattern
     * 
     * This struct contains the actual implementation details of the NeuralPopulation,
     * including neuron storage, statistics, and other member variables. This follows
     * the pImpl idiom to hide implementation details and reduce compilation dependencies.
     */
    struct Impl;

    /**
     * @brief Pointer to the internal implementation
     * 
     * Manages the lifetime of the implementation struct and all contained data.
     */
    Impl* pImpl;
};

} // namespace nlm