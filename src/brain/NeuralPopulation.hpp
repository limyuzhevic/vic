#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <memory>

namespace nlm {

/**
 * @brief A group of neurons with shared properties and behaviors
 * 
 * NeuralPopulation represents a collection of neurons that share common
 * characteristics, such as neuron type, connectivity patterns, and functional
 * roles. Populations enable efficient management of groups of neurons with
 * similar properties and allow for population-level dynamics and statistics.
 * 
 * Key features:
 * - Neurons of the same type within a population
 * - Efficient management of neuron groups
 * - Population-level statistics and metrics
 * - Shared initialization and control
 * - Random initialization capabilities
 * 
 * @note This is a placeholder implementation. Phase 2 will implement
 *       population-level dynamics, synaptic connectivity between neurons
 *       within the population, and specialized population behaviors.
 * 
 * @see NeuralRegion Manages groups of neural populations
 * @see Neuron Individual neuron units
 * @see Brain Central coordinator that manages populations
 * 
 * Example usage:
 * @code
 * NeuralPopulation population(0, 100); // Population ID 0 with 100 neurons
 * population.setNeuronType(NeuronType::Excitatory);
 * population.initializeRandom(rng);
 * population.step(currentTime);
 * float activity = population.getActivityLevel(); // 0.0 to 1.0
 * @endcode
 */
class NeuralPopulation {
public:
    /**
     * @brief Constructor creating a population with specified size
     * 
     * Creates a neural population with a unique identifier and the specified
     * number of neurons. Neurons are not automatically created; they must be
     * added individually using addNeuron(). This allows for flexible population
     * composition and controlled neuron creation.
     * 
     * @param id Unique identifier for this population
     * @param size Initial number of neurons in the population (0 for empty)
     * @throws std::invalid_argument if size is greater than maximum allowed neurons
     * 
     * @note The population ID is used for identification and statistics tracking.
     *       Common ID conventions: 0 for sensory, 1-10 for cortical layers, etc.
     */
    NeuralPopulation(PopulationId id, size_t size);
    
    /**
     * @brief Destructor
     */
    ~NeuralPopulation();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * NeuralPopulation objects cannot be copied to prevent resource management
     * issues and maintain population integrity. Use move semantics instead.
     */
    NeuralPopulation(const NeuralPopulation&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     */
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Efficiently transfers ownership of internal resources from another NeuralPopulation
     * object. The source object becomes unusable after the move operation.
     * 
     * @param other The NeuralPopulation object to move from
     */
    NeuralPopulation(NeuralPopulation&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Efficiently transfers ownership of internal resources from another NeuralPopulation
     * object. The source object becomes unusable after the move operation.
     * 
     * @param other The NeuralPopulation object to move from
     * @return Reference to this NeuralPopulation object
     */
    NeuralPopulation& operator=(NeuralPopulation&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this population
     * 
     * Returns the population ID that was specified during construction.
     * This identifier is used for referencing the population and maintaining
     * statistics and logs.
     * 
     * @return PopulationId The unique identifier for this population
     * 
     * @see addRegion() in Brain for region population management
     */
    PopulationId getId() const;
    
    /**
     * @brief Get the number of neurons in this population
     * 
     * Returns the current count of neurons in the population. This can change
     * over time as neurons are added or removed.
     * 
     * @return size_t Current number of neurons
     * 
     * @note Time complexity: O(1) - returns cached count
     */
    size_t getSize() const;
    
    /**
     * @brief Check if the population is empty
     * 
     * Determines whether the population contains any neurons. Useful for checking
     * population validity before operations that require neurons.
     * 
     * @return bool true if population has no neurons, false otherwise
     */
    bool isEmpty() const;
    
    /**
     * @brief Get a specific neuron by index
     * 
     * Returns a pointer to the neuron at the specified index within the population.
     * Neurons are typically stored in the order they were added. This provides
     * direct access to individual neurons for targeted manipulation.
     * 
     * @param index Index of the neuron to retrieve (0 <= index < getSize())
     * @return Neuron* Pointer to the neuron at the specified index
     * @return const Neuron* Const pointer to the neuron at the specified index
     * @throws std::out_of_range if index is invalid
     * 
     * @note The returned pointer remains valid for the lifetime of the population.
     */
    Neuron* getNeuron(NeuronIndex index);
    const Neuron* getNeuron(NeuronIndex index) const;
    
    /**
     * @brief Add a neuron to this population
     * 
     * Adds the specified neuron to the population. The neuron must not already
     * belong to another population. This allows for dynamic population composition
     * and flexible network architecture.
     * 
     * @param neuron Pointer to the neuron to add
     * @throws std::invalid_argument if neuron is null or already in another population
     * 
     * @note The neuron takes ownership of the neuron object (no copy is made).
     *       The population does not take ownership of the neuron pointer.
     */
    void addNeuron(Neuron* neuron);
    
    /**
     * @brief Get all neurons in this population
     * 
     * Returns a reference to the vector containing all neurons in the population.
     * This provides read-only access to all neurons for iteration or statistics.
     * The vector is stored internally for efficiency.
     * 
     * @return const std::vector<Neuron*>& Reference to the neuron container
     * 
     * @note This returns a reference for efficiency, not a copy. Do not modify
     *       the returned vector directly. Use addNeuron() or getNeuron() for
     *       individual neuron operations.
     */
    const std::vector<Neuron*>& getNeurons() const;
    
    /**
     * @brief Get the neuron type for all neurons in this population
     * 
     * Returns the neuron type that characterizes all neurons in this population.
     * All neurons in a population share the same type, which defines their
     * basic properties and behaviors.
     * 
     * @return NeuronType The neuron type for all neurons in this population
     * 
     * @see NeuronType Enum of neuron types (Excitatory, Inhibitory, etc.)
     */
    NeuronType getNeuronType() const;
    
    /**
     * @brief Set the neuron type for all neurons in this population
     * 
     * Changes the neuron type for all neurons in this population. This can be
     * useful for creating different functional populations or for testing
     * the effects of neuron type on network dynamics.
     * 
     * @param type The new neuron type for all neurons in the population
     */
    void setNeuronType(NeuronType type);
    
    /**
     * @brief Get the number of active neurons in this population
     * 
     * Returns the count of neurons that are currently active (firing or exhibiting
     * activity). This is a key metric for understanding population-level dynamics
     * and network activity patterns.
     * 
     * @return size_t Number of active neurons
     */
    size_t getActiveCount() const;
    
    /**
     * @brief Get the number of firing neurons in this population
     * 
     * Returns the count of neurons that are currently in the firing state,
     * generating action potentials or spikes. This differs from active neurons
     * in that firing neurons are actually producing spikes at that moment.
     * 
     * @return size_t Number of firing neurons
     */
    size_t getFiringCount() const;
    
    /**
     * @brief Get the average firing rate of neurons in this population
     * 
     * Returns the average firing rate across all neurons in the population,
     * providing a measure of overall population activity. This is calculated
     * as the sum of individual firing rates divided by the total number of neurons.
     * 
     * @return float Average firing rate in Hz (0.0 for inactive neurons)
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get the activity level of this population
     * 
     * Returns the fraction of active neurons in the population, providing a
     * normalized measure of population activity between 0.0 (completely inactive)
     * and 1.0 (all neurons active). This is useful for comparing activity levels
     * across populations of different sizes.
     * 
     * @return float Activity level (0.0 to 1.0)
     */
    float getActivityLevel() const;  // fraction of active neurons
    
    /**
     * @brief Initialize all neurons with random parameters
     * 
     * Sets up each neuron in the population with random parameters such as
     * membrane potential, threshold, and other biophysical properties. This
     * is typically called once at the beginning of the simulation to establish
     * a diverse initial state.
     * 
     * @param rng Reference to the random number generator to use for initialization
     * 
     * @note Uses the RandomGenerator's normal distribution for parameters
     *       that should vary around typical values.
     */
    void initializeRandom(class RandomGenerator& rng);
    
    /**
     * @brief Step all neurons in this population
     * 
     * Advances each neuron in the population by one timestep according to
     * their specific dynamics and the current simulation time. This is the
     * primary operation for driving neural activity within the population.
     * 
     * @param currentTime The current simulation time in seconds
     * 
     * @note All neurons are updated sequentially. For performance, neurons
     *       are typically updated in parallel using SIMD or GPU acceleration.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset all neurons to their initial state
     * 
     * Resets all neurons in the population to their initial conditions as defined
     * during construction or initialization. This typically includes resetting
     * membrane potentials, firing states, and other internal state variables.
     * 
     * @note Useful for running multiple simulation trials with the same initial
     *       conditions or for testing network reset behavior.
     */
    void reset();
    
    /**
     * @brief Get the neuron identifiers in this population
     * 
     * Returns a vector containing the IDs of all neurons in the population.
     * This is useful for creating adjacency lists, tracking neurons, or
     * maintaining connectivity information.
     * 
     * @return std::vector<NeuronId> List of neuron IDs
     */
    std::vector<NeuronId> getNeuronIds() const;
    
    /**
     * @brief Record current activity for statistics
     * 
     * Records the current state of the population for later statistical analysis.
     * This is used to compute population-level statistics such as mean membrane
     * potential and variance over time.
     * 
     * @note Statistics are accumulated internally and can be retrieved using
     *       getMeanMembranePotential() and getVarianceMembranePotential().
     */
    void recordActivity();  // Record current state for statistics
    
    /**
     * @brief Get the mean membrane potential of neurons in this population
     * 
     * Returns the average membrane potential across all neurons in the population,
     * providing a measure of the overall depolarization/hyperpolarization state
     * of the population. This is computed from recorded activity statistics.
     * 
     * @return float Mean membrane potential in mV
     */
    float getMeanMembranePotential() const;
    
    /**
     * @brief Get the variance of membrane potentials in this population
     * 
     * Returns the variance of membrane potentials across neurons in the population,
     * providing a measure of the diversity of membrane potentials and the overall
     * synchronization level of the population.
     * 
     * @return float Variance of membrane potentials (mV²)
     */
    float getVarianceMembranePotential() const;
    
private:
    /**
     * @brief Internal implementation details (Pimpl idiom)
     * 
     * Encapsulates the internal implementation of the NeuralPopulation class,
     * allowing for better compilation times and hiding implementation details
     * from the header file.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm