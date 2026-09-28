#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <memory>

namespace nlm {

// NeuralPopulation: A group of neurons with shared properties
// PLACEHOLDER - Phase 2 will implement population-level dynamics

/**
 * @class NeuralPopulation
 * @brief Represents a functional group of neurons with shared characteristics and behavior
 * 
 * The NeuralPopulation class implements population-level neural computation by grouping
 * individual neurons that share common properties, connectivity patterns, and computational
 * roles. Populations enable efficient implementation of large-scale neural networks by
 * enabling batch operations and sharing of population-level resources.
 * 
 * **Key Features:**
 * - Batch management of neuron groups
 * - Shared neuron type across all population members
 * - Efficient population statistics computation
 * - Coordinated initialization and reset
 * - Population-level activity monitoring
 * 
 * **Computational Advantages:**
 * - Memory locality: neurons in same population are likely in same cache lines
 * - Vectorized operations possible for population-level computations
 * - Reduced parameter storage (shared across population members)
 * - Efficient synchronization for population-wide operations
 * 
 * **Population Types Supported:**
 * - EXCITATORY: Positive-to-positive processing
 * - INHIBITORY: Negative-to-positive suppression  
 * - MODULATORY: Regulatory and gating functions
 * - SENSORY: External signal reception
 * - MOTOR: Output generation
 * - INTERNAL: Internal processing and integration
 * 
 * The population abstraction is fundamental for implementing large-scale neural systems
 * with hierarchical organization and specialized functional groups.
 * 
 * @note Uses Pimpl idiom for efficient implementation and memory management
 * @warning Population operations are not thread-safe
 * @tparam PopulationId Type for unique population identification
 * @tparam NeuronId Type for neuron identification
 * @tparam NeuronType Type for neuron categorization
 * 
 * @see NeuralRegion for population management at brain region level
 * @see NeuralRegion::addPopulation() for population creation
 * @see Brain for system-level population access
 */

class NeuralPopulation {
public:
    /**
     * @brief Construct a new NeuralPopulation object
     * 
     * Creates a population of neurons with specified size and characteristics. This
     * constructor establishes the fundamental population parameters including size,
     * type, and initial state.
     * 
     * @param id Unique identifier for this population
     * @param size Number of neurons in this population
     * @post Population initialized with specified number of neurons
     * @pre Size must be greater than 0
     * 
     * @note Neurons in population are not yet created - they are added later via
     *       addNeuron() or initializeRandom(). Memory is allocated lazily for
     *       efficiency when population size is large.
     * 
     * @see addNeuron() for adding individual neurons
     * @see initializeRandom() for creating neurons with random parameters
     * @see getSize() for retrieving population size
     */
    NeuralPopulation(PopulationId id, size_t size);
    
    ~NeuralPopulation();
    
    // Disable copying, enable moving
    /**
     * @brief Copy constructor is deleted to prevent copying
     * 
     * Populations are managed objects with unique identifiers. Copying would violate
     * the one-population-per-function model and lead to duplicate neuron management.
     * 
     * @param other Another population object (unused)
     * 
     * @throws std::runtime_error if attempted copy
     * @note Use move semantics for transferring population ownership
     */
    NeuralPopulation(const NeuralPopulation&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted to prevent copying
     * 
     * Prevents copy assignment to maintain population uniqueness and ownership semantics.
     * 
     * @param other Another population object (unused)
     * 
     * @return Reference to this population (never returns, throws on attempt)
     * @note Use move assignment for transferring ownership
     */
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership of implementation
     * 
     * Efficiently transfers population ownership from source to destination without copying.
     * This is the preferred way to transfer population objects.
     * 
     * @param other Source population object to transfer ownership from
     * @post Source population becomes invalid (null implementation pointer)
     * @throws None - noexcept ensures no exceptions
     * 
     * @note After move, the source population cannot be used
     * @see operator=() for move assignment
     */
    NeuralPopulation(NeuralPopulation&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership of implementation
     * 
     * Efficiently transfers population ownership from source to destination without copying.
     * Self-assignment is handled safely.
     * 
     * @param other Source population object to transfer ownership from
     * @return Reference to this population (for chaining)
     * @post Source population becomes invalid (null implementation pointer)
     * @throws None - noexcept ensures no exceptions
     * 
     * @note After move, the source population cannot be used
     * @see NeuralPopulation(NeuralPopulation&&) for move constructor
     */
    NeuralPopulation& operator=(NeuralPopulation&&) noexcept;
    
    // Identity
    /**
     * @brief Get the unique identifier of this population
     * 
     * @return PopulationId The unique identifier for this population
     * @note This ID remains constant throughout the population's lifetime
     * @see getNeuronType() to get the neuron type of all population members
     * @see getSize() to get the number of neurons in this population
     */
    PopulationId getId() const;
    
    // Size
    /**
     * @brief Get the number of neurons in this population
     * 
     * @return size_t Number of neurons currently in this population
     * @see isEmpty() to check if population has any neurons
     * @see addNeuron() to add neurons to population
     * @note Population size can change over time via addNeuron()
     */
    size_t getSize() const;
    
    /**
     * @brief Check if population contains any neurons
     * 
     * @return bool true if population has zero neurons, false otherwise
     * @see getSize() to get exact neuron count
     * @see addNeuron() to add neurons
     * @see initializeRandom() to create neurons
     */
    bool isEmpty() const;
    
    // Neuron access
    /**
     * @brief Get neuron at specified index in this population
     * 
     * @param index Index of neuron to retrieve (0-based)
     * @return Neuron* Pointer to neuron at specified index
     * @throws std::out_of_range if index >= getSize()
     * @pre Population must contain neurons (not empty)
     * @post Returned pointer is valid for population lifetime
     * 
     * @note Index-based access is efficient for batch operations
     * @see getNeurons() to get all neurons as a vector
     * @see addNeuron() to add neurons to population
     */
    Neuron* getNeuron(NeuronIndex index);
    
    /**
     * @brief Get const neuron at specified index in this population
     * 
     * @param index Index of neuron to retrieve (0-based)
     * @return const Neuron* Const pointer to neuron at specified index
     * @throws std::out_of_range if index >= getSize()
     * @pre Population must contain neurons (not empty)
     * 
     * @note Index-based access is efficient for batch operations
     * @see getNeurons() const to get all neurons as a const vector
     * @see addNeuron() to add neurons to population
     */
    const Neuron* getNeuron(NeuronIndex index) const;
    
    /**
     * @brief Add a neuron to this population
     * 
     * @param neuron Pointer to neuron to add to this population
     * @post Neuron added to population's internal list
     * @pre Neuron must not already belong to another population
     * @pre Neuron pointer must be valid
     * @note Population takes ownership of neuron pointer
     * 
     * @see getNeuron() to retrieve neurons by index
     * @see getNeurons() to get all neurons
     * @see removeNeuron() to remove neurons from population
     */
    void addNeuron(Neuron* neuron);
    
    /**
     * @brief Get all neurons in this population as a const vector
     * 
     * @return const std::vector<Neuron*>& Const reference to vector of neuron pointers
     * @see getNeuron() for index-based access
     * @see addNeuron() to add neurons
     * @note Vector reference may be invalidated by future operations
     */
    const std::vector<Neuron*>& getNeurons() const;
    
    // Population type (all neurons in population share same type)
    /**
     * @brief Get the neuron type of all neurons in this population
     * 
     * @return NeuronType Type shared by all neurons in this population
     * @see setNeuronType() to change neuron type
     * @note All neurons in population share the same type
     */
    NeuronType getNeuronType() const;
    
    /**
     * @brief Set the neuron type for all neurons in this population
     * 
     * @param type New neuron type for all population members
     * @post All neurons in population have the specified type
     * @pre Population must contain neurons
     * @pre All existing neurons are updated to new type
     * 
     * @note This affects neuron computation characteristics
     * @see getNeuronType() to get current neuron type
     * @see Neuron::setType() for individual neuron type setting
     */
    void setNeuronType(NeuronType type);
    
    // Statistics
    /**
     * @brief Get the number of active neurons in this population
     * 
     * @return size_t Number of neurons currently in active state (firing or processing)
     * @see getFiringCount() to get neurons that are actually firing
     * @see getSize() to get total neuron count
     * @see recordActivity() to update activity statistics
     */
    size_t getActiveCount() const;
    
    /**
     * @brief Get the number of neurons currently firing in this population
     * 
     * @return size_t Number of neurons that generated spikes in current timestep
     * @see getActiveCount() to get broader activity measure
     * @see getSize() to get total neuron count
     * @note Firing differs from active in that active includes neurons receiving input
     */
    size_t getFiringCount() const;
    
    /**
     * @brief Get the average firing rate of neurons in this population
     * 
     * @return float Average firing rate (Hz) across all neurons
     * @see getActiveCount() to get activity count
     * @see getFiringCount() to get firing count
     * @see getActivityLevel() to get fraction of active neurons
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get the fraction of active neurons in this population
     * 
     * @return float Activity level (0.0 to 1.0) - fraction of neurons active
     * @see getActiveCount() to get exact count of active neurons
     * @see getAverageFiringRate() to get firing rate
     * @note This is a normalized measure of population activity
     */
    float getActivityLevel() const;  // fraction of active neurons
    
    // Initialize all neurons randomly
    /**
     * @brief Initialize all neurons in this population with random parameters
     * 
     * @param rng Random number generator for parameter sampling
     * @post All neurons in population initialized with random biologically plausible parameters:
     *       - Membrane potential: Uniform(-70, -50) mV
     *       - Threshold: Uniform(-60, -40) mV
     *       - Weight: Uniform(-1.0, 1.0)
     *       - Type: Uniform across NeuronType enum
     *       - State: Reset to resting state
     * 
     * @note This creates neurons and adds them to the population
     * @see addNeuron() for manual neuron addition
     * @see reset() for returning to initial state
     * @warning Clears existing neurons and creates new ones
     */
    void initializeRandom(class RandomGenerator& rng);
    
    // Step all neurons
    /**
     * @brief Update all neurons in this population for one simulation timestep
     * 
     * @param currentTime Current simulation time (seconds)
     * @post All neurons in population updated according to their dynamics
     * @see Neuron::step() for individual neuron updates
     * @see recordActivity() to record current state for statistics
     * @note This performs the core neural computation for the population
     */
    void step(Timestamp currentTime);
    
    // Reset all neurons
    /**
     * @brief Reset all neurons in this population to initial state
     * 
     * @post All neurons reset to resting state with default parameters:
     *       - Membrane potential: -70 mV
     *       - Firing state: Resting
     *       - Refractory period: 0
     *       - Spike history: Cleared
     *       - Plasticity state: Reset
     * 
     * @see step() for simulation updates
     * @see initializeRandom() for re-initialization with random parameters
     * @note This is typically called at the start of each simulation epoch
     */
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
