#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

/**
 * @file NeuralRegion.hpp
 * @brief NeuralRegion: A brain region containing multiple populations and local connectivity
 * 
 * @details A NeuralRegion represents an anatomical or functional area of the brain containing
 *          multiple neural populations with local connectivity patterns. Regions coordinate
 *          population-level activity and maintain internal synaptic connections.
 * 
 * @note PLACEHOLDER - Phase 2 will implement region-level dynamics
 * @author NLM Development Team
 * @date 2026
 * @version 1.0.0
 * 
 * @ingroup Neuronal Dynamics
 */

class NeuralRegion {
/**
     * @brief Constructor - Create region with ID
     * 
     * @details Creates a new neural region with the specified ID and initializes
     *          it as an empty region ready for population and connection creation.
     * 
     * @param id Unique identifier for this region. Must be unique within
     *           the brain system.
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @note Region ID should be valid and unique before region is added to brain
     * @warning Regions should be added to brain before neuron creation
     * @thread_safety Thread-unsafe initialization
     * @post Region is created and ready for population/synapse creation
     * @ingroup Neuronal Dynamics
     */
    explicit NeuralRegion(RegionId id);

    /**
     * @brief Constructor - Create region with ID and name
     * 
     * @details Creates a new neural region with the specified ID and name,
     *          initializing it with a descriptive identifier for debugging and
     *          visualization purposes.
     * 
     * @param id Unique identifier for this region
     * @param name Human-readable name for the region
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @note Name is used for logging and debugging
     * @thread_safety Thread-unsafe initialization
     * @post Region is created with specified name
     * @ingroup Neuronal Dynamics
     */
    NeuralRegion(RegionId id, const std::string& name);

    /**
     * @brief Destructor - Clean up region resources
     * 
     * @details Releases all allocated memory and cleans up region state,
     *          including all contained populations and synapses.
     * 
     * @note All populations and connections should be properly handled before
     *       region destruction
     * @thread_safety Thread-safe if no concurrent operations
     * @post Region resources are freed
     */
    ~NeuralRegion();

    /**
     * @brief Copy constructor (deleted)
     * 
     * @details Copy construction is not supported to prevent accidental copying
     *          and maintain object ownership semantics.
     * 
     * @note Region contains non-copyable resources (unique_ptr)
     * @warning Never use copy constructor - explicitly deleted
     */
    NeuralRegion(const NeuralRegion&) = delete;

    /**
     * @brief Copy assignment operator (deleted)
     * 
     * @details Copy assignment is not supported for the same reasons as copy constructor.
     * 
     * @note Prevents shallow copying of internal resources
     * @warning Never use copy assignment - explicitly deleted
     */
    NeuralRegion& operator=(const NeuralRegion&) = delete;

    /**
     * @brief Move constructor (noexcept)
     * 
     * @details Move constructor allows efficient transfer of region ownership
     *          by moving the internal pImpl pointer. The source region becomes
     *          invalid after move operation.
     * 
     * @param other Region to move from
     * 
     * @note Source region becomes unusable after move
     * @thread_safety Exception-safe
     * @post Source region is in valid but unspecified state
     * @ingroup Neuronal Dynamics
     */
    NeuralRegion(NeuralRegion&&) noexcept;

    /**
     * @brief Move assignment operator (noexcept)
     * 
     * @details Move assignment allows efficient transfer of region ownership,
     *          properly cleaning up existing resources before transferring.
     * 
     * @param other Region to move from
     * 
     * @return Reference to this region after assignment
     * @note Source region becomes unusable after move
     * @thread_safety Exception-safe, may throw
     * @post Source region is in valid but unspecified state
     * @ingroup Neuronal Dynamics
     */
    NeuralRegion& operator=(NeuralRegion&&) noexcept;

    /**
     * @brief Get region ID
     * 
     * @details Retrieves the unique identifier of this region, which is used
     *          for addressing this region in brain operations and connections.
     * 
     * @return RegionId Identifier of this region
     * 
     * @note ID is immutable after construction
     * @thread_safety Thread-safe - read-only access
     * @return Unique region identifier
     * @ingroup Neuronal Dynamics
     */
    RegionId getId() const;

    /**
     * @brief Get region name
     * 
     * @details Retrieves the human-readable name of this region, which is used
     *          for debugging, logging, and visualization purposes.
     * 
     * @return const std::string& Reference to region name string
     * 
     * @note Name is used for identification and display
     * @thread_safety Thread-safe - read-only access
     * @return Region name
     * @ingroup Neuronal Dynamics
     */
    const std::string& getName() const;

    /**
     * @brief Set region name
     * 
     * @details Changes the human-readable name of this region. This is typically
     *          used for debugging, logging, and visualization purposes.
     * 
     * @param name New region name string
     * 
     * @throws std::runtime_error if region is in simulation
     * @note Name change affects logging and debugging output
     * @thread_safety Thread-unsafe if region is in simulation
     * @post Region name is updated
     * @ingroup Neuronal Dynamics
     */
    void setName(const std::string& name);

    /**
     * @brief Add a neural population
     * 
     * @details Creates a new neural population within this region with the
     *          specified size and neuron type. Populations are groups of neurons
     *          that share functional properties and are processed together.
     * 
     * @param size Number of neurons in the new population
     * @param type NeuronType of neurons in this population (default: Internal)
     * 
     * @return PopulationId Unique identifier for the new population
     * 
     * @throws std::runtime_error if population creation fails
     * @note Populations are owned by the region and managed automatically
     * @thread_safety Thread-unsafe population creation
     * @post New population is created and ready for neuron initialization
     * @ingroup Neuronal Dynamics
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);

    /**
     * @brief Get neural population by ID
     * 
     * @details Retrieves the neural population object for direct manipulation of
     *          its internal neurons and connections. Provides low-level access
     *          to population neural structures.
     * 
     * @param id Unique identifier of population to retrieve
     * 
     * @return NeuralPopulation* Pointer to population if found, nullptr if not found
     * 
     * @note Returns non-owning pointer - population lifetime managed by region
     * @thread_safety Thread-safe - read-only access to population pointer
     * @return Non-null pointer if population exists, nullptr otherwise
     * @ingroup Neuronal Dynamics
     */
    NeuralPopulation* getPopulation(PopulationId id);

    /**
     * @brief Get neural population by ID (const version)
     * 
     * @details Const version of getPopulation for read-only access to population data.
     * 
     * @param id Unique identifier of population to retrieve
     * 
     * @return const NeuralPopulation* Const pointer to population if found, nullptr if not found
     * 
     * @note Returns non-owning pointer - population lifetime managed by region
     * @thread_safety Thread-safe - read-only access to population data
     * @return Non-null pointer if population exists, nullptr otherwise
     * @ingroup Neuronal Dynamics
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;

    /**
     * @brief Get population count
     * 
     * @details Retrieves the number of neural populations currently in this region.
     * 
     * @return size_t Total number of populations in this region
     * 
     * @note Constant throughout simulation unless populations are added/removed
     * @thread_safety Thread-safe - read-only access
     * @return Number of populations
     * @ingroup Neuronal Dynamics
     */
    size_t getPopulationCount() const;

    /**
     * @brief Get all populations
     * 
     * @details Retrieves const reference to vector of unique pointers to all
     *          neural populations in this region. Provides read-only access to
     *          all regional populations.
     * 
     * @return const std::vector<std::unique_ptr<NeuralPopulation>>& Const reference to populations container
     * 
     * @note Populations are owned by the region and cannot be modified
     * @thread_safety Thread-safe - read-only access
     * @return Const reference to populations container
     * @ingroup Neuronal Dynamics
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;

    /**
     * @brief Get all populations (non-const)
     * 
     * @details Retrieves mutable reference to vector of unique pointers to all
     *          neural populations in this region. Allows direct modification
     *          of the populations container (with caution).
     * 
     * @return std::vector<NeuralPopulation*> Vector of population pointers
     * 
     * @note Use getPopulations() for read-only access
     * @thread_safety Thread-unsafe - modification requires external sync
     * @return Vector of population pointers
     * @ingroup Neuronal Dynamics
     */
    std::vector<NeuralPopulation*> getAllPopulations();

    /**
     * @brief Add a synapse (local connection)
     * 
     * @details Creates a new synapse connecting two neurons within this region.
     *          This represents local connectivity patterns within the brain region.
     * 
     * @param source NeuronId of pre-synaptic neuron
     * @param destination NeuronId of post-synaptic neuron
     * @param weight Synaptic weight (default 0.0)
     * @param delay Transmission delay in simulation steps (default 1)
     * 
     * @return SynapseId Unique identifier for the new synapse
     * 
     * @throws std::out_of_range if source or destination neurons don't exist
     * @throws std::runtime_error if synapse creation fails
     * @note This creates intra-regional (local) connections
     * @thread_safety Thread-unsafe synapse creation
     * @post New synapse is created and added to region's synapse database
     * @ingroup Neuronal Dynamics
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);

    /**
     * @brief Get synapse by ID
     * 
     * @details Retrieves the synapse object for direct manipulation of its
     *          internal properties. Provides low-level access to synapse state.
     * 
     * @param id Unique identifier of synapse to retrieve
     * 
     * @return Synapse* Pointer to synapse if found, nullptr if not found
     * 
     * @note Returns non-owning pointer - synapse lifetime managed by region
     * @thread_safety Thread-safe - read-only access to synapse pointer
     * @return Non-null pointer if synapse exists, nullptr otherwise
     * @ingroup Neuronal Dynamics
     */
    Synapse* getSynapse(SynapseId id);

    /**
     * @brief Get synapse by ID (const version)
     * 
     * @details Const version of getSynapse for read-only access to synapse data.
     * 
     * @param id Unique identifier of synapse to retrieve
     * 
     * @return const Synapse* Const pointer to synapse if found, nullptr if not found
     * 
     * @note Returns non-owning pointer - synapse lifetime managed by region
     * @thread_safety Thread-safe - read-only access to synapse data
     * @return Non-null pointer if synapse exists, nullptr otherwise
     * @ingroup Neuronal Dynamics
     */
    const Synapse* getSynapse(SynapseId id) const;

    /**
     * @brief Get synapse count
     * 
     * @details Retrieves the number of synapses currently in this region.
     * 
     * @return size_t Total number of synapses in this region
     * 
     * @note Includes both intra-regional connections
     * @thread_safety Thread-safe - read-only access
     * @return Number of synapses
     * @ingroup Neuronal Dynamics
     */
    size_t getSynapseCount() const;

    /**
     * @brief Get all synapses
     * 
     * @details Retrieves const reference to vector of unique pointers to all
     *          synapses in this region. Provides read-only access to all regional
     *          synaptic connections.
     * 
     * @return const std::vector<std::unique_ptr<Synapse>>& Const reference to synapses container
     * 
     * @note Synapses are owned by the region and cannot be modified
     * @thread_safety Thread-safe - read-only access
     * @return Const reference to synapses container
     * @ingroup Neuronal Dynamics
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;

    /**
     * @brief Get synapses from a specific neuron
     * 
     * @details Retrieves all synapses originating from the specified neuron,
     *          i.e., all outgoing connections from that neuron within this region.
     * 
     * @param neuron NeuronId of source neuron
     * 
     * @return std::vector<Synapse*> Vector of synapse pointers originating from neuron
     * 
     * @note Returns all outgoing connections to this neuron
     * @thread_safety Thread-safe - read-only access
     * @return Vector of outgoing synapses
     * @ingroup Neuronal Dynamics
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);

    /**
     * @brief Get synapses to a specific neuron
     * 
     * @details Retrieves all synapses terminating at the specified neuron,
     *          i.e., all incoming connections to that neuron within this region.
     * 
     * @param neuron NeuronId of destination neuron
     * 
     * @return std::vector<Synapse*> Vector of synapse pointers terminating at neuron
     * 
     * @note Returns all incoming connections to this neuron
     * @thread_safety Thread-safe - read-only access
     * @return Vector of incoming synapses
     * @ingroup Neuronal Dynamics
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);

    /**
     * @brief Get total neuron count
     * 
     * @details Retrieves the total number of neurons across all populations
     *          in this region.
     * 
     * @return size_t Total neuron count in region
     * 
     * @note Includes all neuron types and populations
     * @thread_safety Thread-safe - read-only access
     * @return Total neuron count
     * @ingroup Neuronal Dynamics
     */
    size_t getTotalNeuronCount() const;

    /**
     * @brief Get active neuron count
     * 
     * @details Retrieves the number of neurons currently in active firing state
     *          within this region.
     * 
     * @return size_t Number of active neurons
     * 
     * @note Active neurons are those not in refractory or resting state
     * @thread_safety Thread-safe - read-only access during simulation
     * @return Number of active neurons
     * @ingroup Neuronal Dynamics
     */
    size_t getActiveNeuronCount() const;

    /**
     * @brief Get firing neuron count
     * 
     * @details Retrieves the number of neurons currently firing (in refractory state)
     *          within this region.
     * 
     * @return size_t Number of firing neurons
     * 
     * @note Firing neurons are those that have just spiked this timestep
     * @thread_safety Thread-safe - read-only access
     * @return Number of firing neurons
     * @ingroup Neuronal Dynamics
     */
    size_t getFiringNeuronCount() const;

    /**
     * @brief Get average firing rate
     * 
     * @details Retrieves the average firing rate across all neurons in this region
     *          in Hertz (spikes per second).
     * 
     * @return float Average firing rate in Hz
     * 
     * @note Calculated as total spikes divided by simulation time
     * @thread_safety Thread-safe - read-only access
     * @return Average firing rate in Hz
     * @ingroup Neuronal Dynamics
     */
    float getAverageFiringRate() const;

    /**
     * @brief Get average synaptic weight
     * 
     * @details Retrieves the average synaptic weight across all synapses in this region.
     *          This provides a measure of overall connection strength within the region.
     * 
     * @return float Average synaptic weight
     * 
     * @note Takes into account both excitatory and inhibitory connections
     * @thread_safety Thread-safe - read-only access
     * @return Average synaptic weight
     * @ingroup Neuronal Dynamics
     */
    float getAverageSynapticWeight() const;

    /**
     * @brief Get synaptic density
     * 
     * @details Retrieves the synaptic density as a fraction of possible connections
     *          that are actually established within this region. This represents
     *          the sparseness of connectivity.
     * 
     * @return float Synaptic density (0.0 to 1.0, where 1.0 means fully connected)
     * 
     * @note Useful for characterizing network topology
     * @thread_safety Thread-safe - read-only access
     * @return Synaptic density
     * @ingroup Neuronal Dynamics
     */
    float getSynapticDensity() const;  // fraction of possible connections

    /**
     * @brief Step all populations and synapses
     * 
     * @details Advances all neurons and synapses within this region by one
     *          simulation timestep. This includes updating population-level dynamics,
     *          neuron membrane potentials, spike detection, and synaptic transmission.
     * 
     * @param currentTime Current simulation time in milliseconds
     * 
     * @throws std::runtime_error if step processing fails
     * @note This is called automatically by the Brain during simulation
     * @thread_safety Thread-unsafe - called by Brain simulation loop
     * @post All regional neurons and synapses are updated
     * @ingroup Neuronal Dynamics
     */
    void step(Timestamp currentTime);

    /**
     * @brief Reset all neurons and synapses
     * 
     * @details Resets the entire region to its initial state, clearing all neural
     *          activity, synaptic state, and internal counters. This effectively
     *          starts a new simulation episode for this region.
     * 
     * @throws std::runtime_error if reset fails
     * @note Clears both neuron states and synaptic properties
     * @thread_safety Thread-unsafe - should not be called during simulation
     * @post Region returns to initial state
     * @ingroup Neuronal Dynamics
     */
    void reset();

    /**
     * @brief Initialize connectivity
     * 
     * @details Initializes random connectivity patterns within this region,
     *          creating synapses between neurons according to specified probability
     *          and weight distribution. This establishes the initial network structure.
     * 
     * @param rng Reference to random number generator for sampling connections
     * @param connectionProbability Probability that any two neurons are connected
     * @param meanWeight Mean synaptic weight for new connections
     * @param weightVariance Variance of synaptic weights for new connections
     * 
     * @throws std::runtime_error if connectivity initialization fails
     * @note This creates intra-regional (local) connections
     * @thread_safety Thread-unsafe before simulation
     * @post Region has established synaptic connectivity
     * @ingroup Neuronal Dynamics
     */
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                       float connectionProbability,
                                       float meanWeight,
                                       float weightVariance);

    /**
     * @brief Get all neurons across all populations
     * 
     * @details Retrieves pointers to all neurons in this region, providing direct
     *          access to every neuron for operations that require global neuron access.
     * 
     * @return std::vector<Neuron*> Vector of neuron pointers across all populations
     * 
     * @note Returns non-owning pointers - neuron lifetime managed by populations
     * @thread_safety Thread-safe - read-only access
     * @return Vector of all neuron pointers
     * @ingroup Neuronal Dynamics
     */
    std::vector<Neuron*> getAllNeurons();

    /**
     * @brief Get all neurons across all populations (const version)
     * 
     * @details Const version of getAllNeurons for read-only access to neuron pointers.
     * 
     * @return std::vector<const Neuron*> Vector of const neuron pointers across all populations
     * 
     * @note Returns non-owning pointers - neuron lifetime managed by populations
     * @thread_safety Thread-safe - read-only access
     * @return Vector of all const neuron pointers
     * @ingroup Neuronal Dynamics
     */
    std::vector<const Neuron*> getAllNeurons() const;
    
/**
     * @brief Implementation details for NeuralRegion class (pImpl pattern)
     * 
     * @details Contains all internal state and implementation details of the NeuralRegion class.
     *          This follows the pImpl idiom to hide implementation from header files.
     *          The Impl structure contains all regional neural components, population management,
     *          synaptic connections, and regional simulation state.
     * 
     * @note Thread-unsafe access patterns apply to all Impl members
     * @warning Direct access to pImpl is discouraged - use NeuralRegion public interface
     * @ingroup Neuronal Dynamics
     */
    struct Impl;

    /**
     * @brief Non-owning pointer to implementation
     * 
     * @details Stores the actual implementation of NeuralRegion using the pImpl pattern.
     *          This allows the header file to remain clean while hiding implementation details.
     * 
     * @note Ownership of implementation is managed by NeuralRegion
     * @warning Direct access to pImpl is discouraged
     * @ingroup Neuronal Dynamics
     */
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
