#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// NeuralRegion: A brain region containing multiple populations and local connectivity
// PLACEHOLDER - Phase 2 will implement region-level dynamics

/**
 * @class NeuralRegion
 * @brief Represents a functional brain region with internal organization and processing capabilities
 * 
 * The NeuralRegion class implements hierarchical neural organization by grouping related
 * neural populations into functional regions. Each region represents a distinct computational
 * module with specialized processing functions and internal connectivity patterns.
 * 
 * **Region Organization:**
 * - Multiple neural populations (Excitatory, Inhibitory, Internal, etc.)
 * - Internal synaptic connections within the region
 * - Local connectivity patterns for efficient computation
 * - Population-to-population communication
 * 
 * **Computational Roles:**
 * - Feature extraction and processing
 * - Pattern recognition and categorization
 * - Memory storage and retrieval
 * - Decision making and selection
 * - Control and modulation of downstream regions
 * 
 * **Regional Architecture:**
 * - Structured hierarchy (cortex -> areas -> layers)
 * - Specialized functional zones
 * - Distributed processing across populations
 * - Integration at regional level
 * 
 * Regions are the primary organizational unit for implementing complex neural computations,
 * enabling modular design and scalable network architectures.
 * 
 * @note Uses Pimpl idiom for efficient implementation and memory management
 * @warning Region operations are not thread-safe
 * @tparam RegionId Type for unique region identification
 * @tparam PopulationId Type for population identification
 * @tparam NeuronId Type for neuron identification
 * @tparam SynapseId Type for synapse identification
 * 
 * @see NeuralPopulation for basic computational units
 * @see NeuralRegionManager for region-level coordination
 * @see Brain for top-level region management
 */

class NeuralRegion {
public:
    /**
     * @brief Construct a new NeuralRegion object with default name
     * 
     * Creates a brain region with specified identifier. This constructor establishes
     * the fundamental regional parameters including size, organization, and initial state.
     * 
     * @param id Unique identifier for this region
     * @post Region initialized with empty population and synapse lists
     * @pre Region ID must be unique within the brain
     * 
     * @note Region names can be set later via setName()
     * @see NeuralRegion(RegionId, const std::string&) for named region creation
     * @see setName() to assign human-readable name
     * @see getId() to retrieve region identifier
     */
    explicit NeuralRegion(RegionId id);
    
    /**
     * @brief Construct a new NeuralRegion object with custom name
     * 
     * Creates a brain region with specified identifier and human-readable name.
     * This constructor is useful for debugging and documentation purposes.
     * 
     * @param id Unique identifier for this region
     * @param name Human-readable name for this region (e.g., "Visual Cortex", "Hippocampus")
     * @post Region initialized with name and empty population/synapse lists
     * @pre Region ID must be unique within the brain
     * 
     * @note Name is displayed in status reports and debugging output
     * @see setName() to change region name later
     * @see getName() to retrieve region name
     * @see NeuralRegion(RegionId) for unnamed region creation
     */
    NeuralRegion(RegionId id, const std::string& name);
    
    ~NeuralRegion();
    
    // Disable copying, enable moving
    /**
     * @brief Copy constructor is deleted to prevent copying
     * 
     * Regions are managed objects with unique identifiers. Copying would violate
     * the one-region-per-functional-unit model and lead to duplicate regional organization.
     * 
     * @param other Another region object (unused)
     * 
     * @throws std::runtime_error if attempted copy
     * @note Use move semantics for transferring region ownership
     */
    NeuralRegion(const NeuralRegion&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted to prevent copying
     * 
     * Prevents copy assignment to maintain region uniqueness and ownership semantics.
     * 
     * @param other Another region object (unused)
     * 
     * @return Reference to this region (never returns, throws on attempt)
     * @note Use move assignment for transferring ownership
     */
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership of implementation
     * 
     * Efficiently transfers region ownership from source to destination without copying.
     * This is the preferred way to transfer region objects.
     * 
     * @param other Source region object to transfer ownership from
     * @post Source region becomes invalid (null implementation pointer)
     * @throws None - noexcept ensures no exceptions
     * 
     * @note After move, the source region cannot be used
     * @see operator=() for move assignment
     */
    NeuralRegion(NeuralRegion&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership of implementation
     * 
     * Efficiently transfers region ownership from source to destination without copying.
     * Self-assignment is handled safely.
     * 
     * @param other Source region object to transfer ownership from
     * @return Reference to this region (for chaining)
     * @post Source region becomes invalid (null implementation pointer)
     * @throws None - noexcept ensures no exceptions
     * 
     * @note After move, the source region cannot be used
     * @see NeuralRegion(NeuralRegion&&) for move constructor
     */
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    // Identity
    /**
     * @brief Get the unique identifier of this region
     * 
     * @return RegionId The unique identifier for this region
     * @note This ID remains constant throughout the region's lifetime
     * @see getName() to get the region's human-readable name
     * @see addPopulation() to add populations to this region
     */
    RegionId getId() const;
    
    /**
     * @brief Get the human-readable name of this region
     * 
     * @return const std::string& Name assigned to this region
     * @see setName() to change the region's name
     * @see getId() to get the region's identifier
     * @note Name is used for debugging and documentation purposes
     */
    const std::string& getName() const;
    
    /**
     * @brief Set the human-readable name of this region
     * 
     * @param name New name for this region
     * @post Region name updated
     * @see getName() to retrieve current name
     * @note Name does not affect region functionality, only identification
     */
    void setName(const std::string& name);
    
    // Population management
    /**
     * @brief Add a new neural population to this region
     * 
     * @param size Number of neurons in the new population
     * @param type Type of neurons in the population (default: Internal)
     * @return PopulationId Unique identifier for the created population
     * @post New population created and added to region's population list
     * @pre Population size must be greater than 0
     * @pre Region must have available population ID
     * 
     * @note Population is created with random initial parameters
     * @see getPopulation() to retrieve populations by ID
     * @see getPopulations() to get all populations
     * @see addSynapse() to add connections within region
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    /**
     * @brief Get a population by its identifier
     * 
     * @param id Population identifier
     * @return NeuralPopulation* Pointer to population with specified ID
     * @throws std::out_of_range if population ID not found
     * @pre Population must have been added to this region
     * @post Returned pointer is valid for region lifetime
     * 
     * @note Index-based access is more efficient for known populations
     * @see getPopulations() for all populations
     * @see getPopulationCount() for population count
     */
    NeuralPopulation* getPopulation(PopulationId id);
    
    /**
     * @brief Get const population by its identifier
     * 
     * @param id Population identifier
     * @return const NeuralPopulation* Const pointer to population with specified ID
     * @throws std::out_of_range if population ID not found
     * @pre Population must have been added to this region
     * 
     * @note For reading access to population data
     * @see getPopulations() const for all populations
     * @see getPopulationCount() for population count
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * @brief Get the number of populations in this region
     * 
     * @return size_t Number of populations in this region
     * @see getPopulations() to get all populations
     * @see addPopulation() to add new populations
     */
    size_t getPopulationCount() const;
    
    /**
     * @brief Get all populations in this region as a const vector
     * 
     * @return const std::vector<std::unique_ptr<NeuralPopulation>>& Const reference to vector of populations
     * @see getPopulation() for ID-based access
     * @see addPopulation() to add populations
     * @note Vector reference may be invalidated by future operations
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    /**
     * @brief Get all populations in this region as non-const pointers
     * 
     * @return std::vector<NeuralPopulation*> Vector of population pointers
     * @see getPopulation() for ID-based access
     * @see getPopulations() const for const version
     * @note Useful for batch operations on all populations
     */
    std::vector<NeuralPopulation*> getAllPopulations();
    
    // Synapse management (local connections within this region)
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    Synapse* getSynapse(SynapseId id);
    const Synapse* getSynapse(SynapseId id) const;
    size_t getSynapseCount() const;
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    // Connectivity queries
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    // Synapse management (local connections within this region)
    /**
     * @brief Add a synaptic connection between neurons within this region
     * 
     * @param source NeuronId Identifier of pre-synaptic neuron
     * @param destination NeuronId Identifier of post-synaptic neuron  
     * @param weight Initial synaptic weight (default: 0.0f)
     * @param delay Transmission delay in simulation steps (default: 1)
     * @return SynapseId Unique identifier for the created synapse
     * @post Synapse created and added to region's synapse list
     * @pre Both source and destination neurons must be in this region
     * @pre Neurons must not be the same (no self-connections)
     * 
     * @note Creates a directed connection from source to destination
     * @see getSynapse() to retrieve synapses by ID
     * @see getSynapses() to get all synapses
     * @see addInterRegionConnection() for connections to other regions
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Get a synapse by its identifier
     * 
     * @param id Synapse identifier
     * @return Synapse* Pointer to synapse with specified ID
     * @throws std::out_of_range if synapse ID not found
     * @pre Synapse must have been added to this region
     * @post Returned pointer is valid for region lifetime
     * 
     * @note Index-based access is more efficient for known synapses
     * @see getSynapses() for all synapses
     * @see getSynapseCount() for synapse count
     */
    Synapse* getSynapse(SynapseId id);
    
    /**
     * @brief Get const synapse by its identifier
     * 
     * @param id Synapse identifier
     * @return const Synapse* Const pointer to synapse with specified ID
     * @throws std::out_of_range if synapse ID not found
     * @pre Synapse must have been added to this region
     * 
     * @note For reading access to synapse data
     * @see getSynapses() const for all synapses
     * @see getSynapseCount() for synapse count
     */
    const Synapse* getSynapse(SynapseId id) const;
    
    /**
     * @brief Get the number of synapses in this region
     * 
     * @return size_t Number of synapses in this region
     * @see getSynapses() to get all synapses
     * @see addSynapse() to add new synapses
     */
    size_t getSynapseCount() const;
    
    /**
     * @brief Get all synapses in this region as a const vector
     * 
     * @return const std::vector<std::unique_ptr<Synapse>>& Const reference to vector of synapses
     * @see getSynapse() for ID-based access
     * @see addSynapse() to add synapses
     * @note Vector reference may be invalidated by future operations
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    // Connectivity queries
    /**
     * @brief Get all synapses originating from a specific neuron
     * 
     * @param neuron Neuron identifier
     * @return std::vector<Synapse*> Vector of synapses where neuron is pre-synaptic
     * @pre Neuron must be in this region
     * @post Returned vector contains valid synapse pointers
     * 
     * @note Useful for neuron output analysis and plasticity computation
     * @see getSynapsesTo() for incoming connections
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    /**
     * @brief Get all synapses targeting a specific neuron
     * 
     * @param neuron Neuron identifier
     * @return std::vector<Synapse*> Vector of synapses where neuron is post-synaptic
     * @pre Neuron must be in this region
     * @post Returned vector contains valid synapse pointers
     * 
     * @note Useful for neuron input analysis and synaptic integration
     * @see getSynapsesFrom() for outgoing connections
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    // Statistics
    /**
     * @brief Get total number of neurons across all populations in this region
     * 
     * @return size_t Total neuron count for this region
     * @see getActiveNeuronCount() to get count of active neurons
     * @see getFiringNeuronCount() to get count of firing neurons
     * @see getAverageFiringRate() to get firing statistics
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get number of active neurons in this region
     * 
     * @return size_t Number of neurons currently receiving/processing input
     * @see getTotalNeuronCount() for total neuron count
     * @see getFiringNeuronCount() for firing neuron count
     * @see getPopulations() for per-population statistics
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of firing neurons in this region
     * 
     * @return size_t Number of neurons that generated spikes in current timestep
     * @see getActiveNeuronCount() for broader activity measure
     * @see getTotalNeuronCount() for total neuron count
     * @note Firing differs from active in that active includes neurons receiving input
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons in this region
     * 
     * @return float Average firing rate (Hz) across all neurons
     * @see getTotalNeuronCount() for total neuron count
     * @see getFiringNeuronCount() for firing count
     * @see getPopulations() for per-population firing statistics
     */
    float getAverageFiringRate() const;
    
    // Local connectivity statistics
    /**
     * @brief Get the average synaptic weight across all synapses in this region
     * 
     * @return float Average synaptic weight (-1.0 to 1.0 typical range)
     * @see getSynapseCount() for synapse count
     * @see getSynapticDensity() for connectivity measure
     * @note Reflects overall excitation-inhibition balance
     */
    float getAverageSynapticWeight() const;
    
    /**
     * @brief Get the synaptic density (fraction of possible connections)
     * 
     * @return float Synaptic density (0.0 to 1.0)
     * @see getTotalNeuronCount() for total neuron count
     * @see getSynapseCount() for actual synapse count
     * @note Higher density enables more complex computations
     * @note Sparse connectivity is often biologically realistic
     */
    float getSynapticDensity() const;  // fraction of possible connections
    
    // Step all populations and synapses
    /**
     * @brief Update all neurons and synapses in this region for one timestep
     * 
     * @param currentTime Current simulation time (seconds)
     * @post All neurons and synapses in region updated according to their dynamics
     * @see NeuralPopulation::step() for population updates
     * @see Synapse::step() for synapse updates
     * @see getTotalNeuronCount() for neuron count
     * @see getSynapseCount() for synapse count
     */
    void step(Timestamp currentTime);
    
    // Reset all neurons and synapses
    /**
     * @brief Reset all neurons and synapses in this region to initial state
     * 
     * @post All neurons reset to resting state and all synapses reset to initial weights
     * @see step() for simulation updates
     * @see initializeRandomConnectivity() for re-initialization with random parameters
     * @note This is typically called at the start of each simulation epoch
     * @note Cleared spike histories and reset plasticity state
     */
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
