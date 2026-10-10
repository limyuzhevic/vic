#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <memory>

namespace nlm {

/**
 * @file NeuralPopulation.hpp
 * 
 * Neural population implementation for NLM
 * 
 * This file provides the neural population implementation, which is a
 * specialized group of neurons with shared properties and characteristics.
 * Neural populations are the fundamental organizational unit within neural
 * regions, bringing together neurons that perform similar computational
 * functions or have similar biophysical properties.
 * 
 * Neural populations in NLM serve as the bridge between regional organization
 * and individual neuron computation, providing:
 * - Homogeneous computational units within regions
 * - Efficient parallel processing capabilities
 * - Population-level dynamics and interactions
 * - Shared synaptic and plasticity properties
 * - Optimized for specific computational tasks
 * 
 * Population organization in NLM:
 * - **Brain** organizes regions
 * - **Regions** organize populations
 * - **Populations** organize neurons
 * - **Neurons** perform individual computations
 * - **Synapses** enable communication
 * 
 * Key features:
 * - Uniform neuron properties within population
 * - Efficient population-level operations
 * - Population statistics and monitoring
 * - Random initialization and configuration
 * - Population-level dynamics management
 * - Activity tracking and analysis
 * - Computational efficiency through grouping
 * 
 * @note Essential for modular brain architecture
 * @note Used by NeuralRegion for regional organization
 * @note Critical for population-level computation
 * @see NeuralRegion.hpp - For region-level management
 * @see Neuron.hpp - For neuron-level implementation
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class NeuralPopulation
 * 
 * Neural population implementing a group of neurons with shared properties.
 * 
 * The NeuralPopulation class represents a specialized group of neurons that
 * perform similar computational functions or have similar biophysical
 * properties. This is the third level of organization in NLM (after the
 * brain and regions), providing homogeneous computational units that
 * work together to perform specific tasks within the brain.
 * 
 * Neural populations in NLM are characterized by:
 * - **Homogeneity**: All neurons share similar properties and types
 * - **Efficiency**: Enables efficient parallel processing
 * - **Scalability**: Can contain large numbers of neurons
 * - **Management**: Simplifies neuron management and control
 * - **Specialisation**: Optimized for specific computational tasks
 * 
 * Population-level organization in NLM:
 * 1. **Brain** coordinates all regions
 * 2. **Regions** organize populations by function
 * 3. **Populations** provide homogeneous computational units
 * 4. **Neurons** perform individual computations within populations
 * 5. **Synapses** enable communication between neurons
 * 
 * Population functions include:
 * - Providing homogeneous computational resources
 * - Enabling efficient parallel processing
 * - Managing neuron populations with shared properties
 * - Supporting population-level dynamics
 * - Facilitating computational specialization
 * 
 * @note Essential for modular brain architecture
 * @note Used by NeuralRegion for regional organization
 * @note Critical for population-level computation
 * @see NeuralRegion.hpp - For region-level management
 * @see Neuron.hpp - For neuron-level implementation
 * 
 * @author NLM Development Team
 */
class NeuralPopulation {
public:
    /**
     * @brief Constructor with ID and size
     * 
     * Creates a new neural population with the specified ID and size.
     * The population is initialized with empty neurons that can be
     * individually configured and initialized later.
     * 
     * @param id Unique identifier for the population
     * @param size Number of neurons in the population
     * 
     * @pre id must be valid (not zero or invalid)
     * @pre size must be positive
     * @post Population created with specified ID and size
     * @post Population ready for neuron initialization
     * @post Neuron IDs will be assigned automatically
     * 
     * @note Neurons in population share common properties
     * @see NeuralPopulation(PopulationId) - Alternative constructor
     * @see getId() - For retrieving population ID
     * @see getSize() - For retrieving population size
     */
    NeuralPopulation(PopulationId id, size_t size);
    
    /**
     * @brief Destructor
     * 
     * Cleans up population resources and performs any necessary
     * cleanup operations. The population is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Population cannot be used after destruction
     */
    ~NeuralPopulation();
    
    // Disable copying - PImpl idiom
    NeuralPopulation(const NeuralPopulation&) = delete;
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;
    NeuralPopulation(NeuralPopulation&&) noexcept;
    NeuralPopulation& operator=(NeuralPopulation&&) noexcept;
    
    /**
     * @brief Get population identifier
     * 
     * Returns the unique identifier assigned to this neural population.
     * This ID is used for addressing populations in the brain and is
     * essential for population management and organization.
     * 
     * @return PopulationId Unique identifier
     * 
     * @post Return value is constant for population lifetime
     * @complexity O(1)
     * 
     * @see NeuralPopulation(PopulationId, size_t) - Constructor with ID
     */
    PopulationId getId() const;
    
    /**
     * @brief Get population size
     * 
     * Returns the number of neurons in this population. Population size
     * is a key statistic for understanding population complexity and
     * computational capacity.
     * 
     * @return Number of neurons in population
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see isEmpty() - For checking if population is empty
     * @see getId() - For retrieving population ID
     */
    size_t getSize() const;
    
    /**
     * @brief Check if population is empty
     * 
     * Determines whether this population contains any neurons. This is
     * useful for checking population state and handling edge cases.
     * 
     * @return true if population contains no neurons, false otherwise
     * 
     * @post Return value reflects current population state
     * @complexity O(1)
     * 
     * @see getSize() - For getting population size
     */
    bool isEmpty() const;
    
    /**
     * @brief Get neuron (non-const version)
     * 
     * Returns a pointer to the neuron at the specified index within this
     * population. This allows access to and modification of individual
     * neuron state.
     * 
     * @param index Index of neuron to retrieve (0 to size-1)
     * @return Pointer to neuron
     * 
     * @pre index must be within valid range [0, size-1]
     * @post Return value is valid as long as population is initialized
     * @complexity O(1)
     * 
     * @note Neurons are accessed by index for efficiency
     * @see getNeuron(NeuronIndex) const - Const version
     */
    Neuron* getNeuron(NeuronIndex index);
    
    /**
     * @brief Get neuron (const version)
     * 
     * Returns a const pointer to the neuron at the specified index within
     * this population. This provides read-only access to individual
     * neuron state, useful for inspection and debugging.
     * 
     * @param index Index of neuron to retrieve (0 to size-1)
     * @return Const pointer to neuron
     * 
     * @pre index must be within valid range [0, size-1]
     * @post Return value is valid as long as population is initialized
     * @complexity O(1)
     * 
     * @note Neurons are accessed by index for efficiency
     * @see getNeuron(NeuronIndex) - Non-const version
     */
    const Neuron* getNeuron(NeuronIndex index) const;
    
    /**
     * @brief Add neuron to population
     * 
     * Adds an existing neuron to this population. This is used for:
     * - Integrating neurons into populations
     * - Building populations from pre-initialized neurons
     * - Dynamic population management
     * - Experimental control
     * 
     * @param neuron Pointer to neuron to add
     * 
     * @pre neuron must not already be in another population
     * @post Neuron added to population
     * @post Population size increased
     * 
     * @note Each neuron belongs to exactly one population
     * @see getNeurons() - For accessing all neurons
     */
    void addNeuron(Neuron* neuron);
    
    /**
     * @brief Get all neurons (const version)
     * 
     * Returns a const reference to the vector containing all neurons in this
     * population. This provides access to the complete population structure.
     * 
     * @return Const reference to vector of neuron pointers
     * 
     * @post Return value stable during simulation
     * @complexity O(1) for reference, O(n) for copying
     * 
     * @note Returns neurons in index order
     * @see addNeuron() - For adding neurons
     */
    const std::vector<Neuron*>& getNeurons() const;
    
    /**
     * @brief Get neuron type
     * 
     * Returns the neuron type for this population. All neurons in a
     * population share the same type, which determines their computational
     * properties and behavior.
     * 
     * @return NeuronType Neuron type for all neurons in population
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see setNeuronType() - For setting neuron type
     * @see NeuronType - Available neuron types
     */
    NeuronType getNeuronType() const;
    
    /**
     * @brief Set neuron type
     * 
     * Changes the neuron type for this population. This affects all neurons
     * in the population, changing their computational properties and
     * behavior. This is useful for:
     * - Creating specialized populations
     * - Adjusting population properties
     * - Dynamic type changes
     * - Experimental studies
     * 
     * @param type New neuron type for all neurons in population
     * 
     * @post Neuron type updated for all neurons
     * @post Computational properties changed
     * 
     * @note All neurons in population share type
     * @see getNeuronType() - For getting neuron type
     */
    void setNeuronType(NeuronType type);
    
    /**
     * @brief Get active neuron count
     * 
     * Returns the number of neurons currently firing or active in this
     * population. This is an important statistic for understanding
     * population-level activity.
     * 
     * @return Number of active neurons
     * 
     * @post Return value fluctuates during simulation
     * @complexity O(n) where n is population size
     * 
     * @note Used for population activity monitoring
     * @note Critical for resource allocation
     * @see getFiringCount() - For firing neurons
     */
    size_t getActiveCount() const;
    
    /**
     * @brief Get firing neuron count
     * 
     * Returns the number of neurons that have fired in the current timestep
     * in this population. This differs from active neurons, which includes
     * neurons that are currently firing.
     * 
     * @return Number of neurons that fired this timestep
     * 
     * @post Return value resets each timestep
     * @complexity O(n) where n is population size
     * 
     * @see getActiveCount() - For currently active neurons
     */
    size_t getFiringCount() const;
    
    /**
     * @brief Get average firing rate
     * 
     * Calculates and returns the average firing rate across all neurons
     * in this population. This is an important metric for understanding
     * population dynamics and performance.
     * 
     * @return Average firing rate in Hz (spikes per second)
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is population size
     * 
     * @note Weighted by number of spikes per neuron
     * @note Critical for population analysis
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get activity level
     * 
     * Calculates and returns the activity level of this population,
     * defined as the fraction of active neurons relative to the total
     * population size. This is a normalized measure of population activity.
     * 
     * @return Activity level (0.0 to 1.0)
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is population size
     * 
     * @note 0.0 = no active neurons, 1.0 = all neurons active
     * @see getActiveCount() - For active neuron count
     * @see getSize() - For population size
     */
    float getActivityLevel() const;  // fraction of active neurons
    
    /**
     * @brief Initialize all neurons randomly
     * 
     * Initializes all neurons in this population with random biophysical
     * parameters, typically used for generating biologically realistic
     * neural networks. This function randomizes various parameters to create
     * diversity across neurons while maintaining population homogeneity.
     * 
     * @param rng Random number generator for reproducible initialization
     * 
     * @pre rng must be valid and seeded
     * @post All neurons initialized with random parameters
     * @post Population state ready for simulation
     * 
     * @note Uses the population's neuron type for initialization
     * @see addNeuron() - For adding pre-initialized neurons
     * @see reset() - For state reset without parameter changes
     */
    void initializeRandom(class RandomGenerator& rng);
    
    /**
     * @brief Step all neurons
     * 
     * Advances the population by one timestep, updating all neuron
     * computations. This is the primary computation function for the
     * population that calls step() on each individual neuron.
     * 
     * @param currentTime Current simulation time (ms)
     * 
     * @pre currentTime must be non-negative
     * @post All neuronal computations updated
     * @post Population state advanced
     * 
     * @note Calls step() on each neuron
     * @note Primary population computation function
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset all neurons
     * 
     * Resets all neurons in this population to their initial state,
     * undoing all changes accumulated during simulation. This includes:
     * - All neurons reset to initial state
     * - Population statistics reset
     * - Activity counters reset
     * 
     * @post Population returned to initial state
     * @post Ready for new simulation
     * @post All neuronal variables reset
     * 
     * @see initializeRandom() - For parameter initialization
     * @see step() - For simulation stepping
     */
    void reset();
    
    /**
     * @brief Get neuron IDs in this population
     * 
     * Returns a vector of neuron IDs for all neurons in this population.
     * This is useful for:
     * - Identifying neurons within population
     * - Synaptic connection management
     * - Debugging and analysis
     * - Population statistics
     * 
     * @return Vector of neuron IDs
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is population size
     * 
     * @note Includes IDs for all neurons in population
     */
    std::vector<NeuronId> getNeuronIds() const;
    
    /**
     * @brief Record current activity
     * 
     * Records the current state of neurons in this population for
     * statistics and analysis. This function is called automatically
     * during simulation steps to maintain population statistics.
     * 
     * @post Population statistics updated
     * @post Statistical tracking enabled
     * 
     * @note Called automatically by step()
     * @see getMeanMembranePotential() - For membrane potential statistics
     * @see getVarianceMembranePotential() - For variance statistics
     */
    void recordActivity();  // Record current state for statistics
    
    /**
     * @brief Get mean membrane potential
     * 
     * Returns the mean membrane potential across all neurons in this
     * population. This is a key statistic for understanding population
     * dynamics and state.
     * 
     * @return Mean membrane potential across population
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is population size
     * 
     * @note Used for population dynamics analysis
     * @see getVarianceMembranePotential() - For variance statistics
     */
    float getMeanMembranePotential() const;
    
    /**
     * @brief Get variance of membrane potential
     * 
     * Returns the variance of membrane potential across all neurons in this
     * population. This measures the spread of membrane potentials and is
     * an important statistic for understanding population dynamics.
     * 
     * @return Variance of membrane potential across population
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is population size
     * 
     * @note Higher variance indicates greater diversity
     * @see getMeanMembranePotential() - For mean statistics
     */
    float getVarianceMembranePotential() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
