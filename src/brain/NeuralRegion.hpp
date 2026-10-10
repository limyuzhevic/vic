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
 * 
 * Neural region implementation for NLM
 * 
 * This file provides the neural region implementation, which is a specialized
 * grouping of neurons that perform specific computational functions within
 * the NLM system. Neural regions are the building blocks of the brain's
 * modular architecture, organizing neurons into functional units based on:
 * - Computational role (sensory, motor, association)
 * - Neuronal type and properties
 * - Local connectivity patterns
 * - Input/output relationships with other regions
 * 
 * Neural regions in NLM:
 * - Organize neurons into functionally coherent groups
 * - Implement local synaptic connectivity and dynamics
 * - Manage input/output relationships with other regions
 * - Support region-specific computation patterns
 * - Enable hierarchical brain organization
 * 
 * Key features:
 * - Population-based organization
 * - Local synaptic connectivity
 * - Regional statistics and monitoring
 * - Random connectivity initialization
 * - Efficient neuron and synapse management
 * - Regional input/output interfaces
 * - Statistical tracking and monitoring
 * 
 * @note Critical for modular brain architecture
 * @note Used by Brain for overall brain organization
 * @note Essential for regional computation and communication
 * @see Brain.hpp - For brain-level region management
 * @see NeuralPopulation.hpp - For population-level implementation
 * @see Synapse.hpp - For local connectivity
 * @see Synapse.hpp - For synaptic management
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class NeuralRegion
 * 
 * Neural region implementing a functional brain region with local connectivity.
 * 
 * The NeuralRegion class represents a specialized brain region that organizes
 * neurons into functional populations with local connectivity patterns.
 * This is the second level of organization in NLM (after the brain), providing
 * modular computation units that work together to form the complete neural
 * processing system.
 * 
 * Neural regions in NLM are characterized by:
 * - **Modularity**: Each region has a specific computational function
 * - **Self-containment**: Contains its own populations and local connections
 * - **Interface**: Provides inputs to and receives outputs from other regions
 * - **Specialisation**: Contains neurons with specific types and properties
 * - **Efficiency**: Optimizes local communication and computation
 * 
 * The regional organization in NLM:
 * 1. **Brain** coordinates all regions
 * 2. **Regions** provide modular computation units
 * 3. **Populations** group neurons with similar properties
 * 4. **Neurons** perform individual computations
 * 5. **Synapses** enable local connectivity within regions
 * 
 * Regional functions include:
 * - Processing specific computational tasks
 * - Integrating inputs from other regions
 * - Producing outputs to other regions
 * - Maintaining internal dynamics and stability
 * - Managing local synaptic plasticity
 * 
 * @note Essential for modular brain architecture
 * @note Used by Brain for overall brain organization
 * @note Critical for regional computation and communication
 * @see Brain.hpp - For brain-level region management
 * @see NeuralPopulation.hpp - For population-level implementation
 * @see Synapse.hpp - For local connectivity
 * 
 * @author NLM Development Team
 */
class NeuralRegion {
public:
    /**
     * @brief Constructor with ID only
     * 
     * Creates a new neural region with only a region ID. The region is
     * initialized with default parameters and can be further configured
     * with populations, connections, and other properties.
     * 
     * @param id Unique identifier for the region
     * 
     * @pre id must be valid (not zero or invalid)
     * @post Region created with specified ID
     * @post Region ready for population and connectivity setup
     * @post Name will be empty until set
     * 
     * @note Use NeuralRegion(RegionId, const std::string&) for named regions
     * @see getId() - For retrieving region ID
     * @see setName() - For setting region name
     */
    explicit NeuralRegion(RegionId id);
    
    /**
     * @brief Constructor with ID and name
     * 
     * Creates a new neural region with both ID and name. This is the
     * recommended constructor for creating regions with descriptive
     * names for identification and debugging.
     * 
     * @param id Unique identifier for the region
     * @param name Descriptive name for the region
     * 
     * @pre id must be valid
     * @pre name should be descriptive and unique
     * @post Region created with specified ID and name
     * @post Region ready for population and connectivity setup
     * 
     * @note Name is used for debugging and identification
     * @see getId() - For retrieving region ID
     * @see getName() - For retrieving region name
     * @see setName() - For changing region name
     */
    NeuralRegion(RegionId id, const std::string& name);
    
    /**
     * @brief Destructor
     * 
     * Cleans up region resources and performs any necessary
     * cleanup operations. The region is left in a valid but
     * non-operational state after destruction.
     * 
     * @post All resources released
     * @post Region cannot be used after destruction
     */
    ~NeuralRegion();
    
    // Disable copying - PImpl idiom
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&&) noexcept;
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    /**
     * @brief Get region identifier
     * 
     * Returns the unique identifier assigned to this neural region.
     * This ID is used for addressing regions in the brain and is
     * essential for regional management and connectivity.
     * 
     * @return RegionId Unique identifier
     * 
     * @post Return value is constant for region lifetime
     * @complexity O(1)
     * 
     * @see NeuralRegion(RegionId) - Constructor with ID
     * @see NeuralRegion(RegionId, const std::string&) - Constructor with name
     */
    RegionId getId() const;
    
    /**
     * @brief Get region name
     * 
     * Returns the descriptive name assigned to this neural region.
     * Region names are used for debugging, identification, and
     * human-readable reporting. They provide context for understanding
     * the region's functional role.
     * 
     * @return Const reference to region name
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see setName() - For setting region name
     * @see getId() - For retrieving region ID
     */
    const std::string& getName() const;
    
    /**
     * @brief Set region name
     * 
     * Changes the descriptive name of this neural region. Region names
     * are used for debugging, identification, and human-readable reporting.
     * This is useful for:
     * - Debugging and troubleshooting
     * - Human-readable output
     * - Region identification and labeling
     * - Experimental control
     * 
     * @param name New descriptive name for the region
     * 
     * @post Region name updated
     * @post Human-readable identification updated
     * 
     * @note Name is used for debugging and reporting
     * @see getName() - For retrieving region name
     */
    void setName(const std::string& name);
    
    /**
     * @brief Add neural population
     * 
     * Creates and adds a new neural population to this region. Populations
     * are specialized groups of neurons that perform similar computations
     * or have similar properties. This is the primary method for building
     * the region's internal structure.
     * 
     * @param size Number of neurons in the population
     * @param type Neuron type for the population (default: Internal)
     * @return PopulationId Unique ID for the created population
     * 
     * @pre size must be positive
     * @pre type must be a valid NeuronType
     * @post New population created and added to region
     * @post Population can be accessed via getPopulation()
     * 
     * @note Populations organize neurons by type and function
     * @see NeuralPopulation - For population class
     * @see getPopulation() - For accessing populations
     * @see getPopulations() - For accessing all populations
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    /**
     * @brief Get neural population (non-const version)
     * 
     * Returns a pointer to the neural population with the specified ID.
     * This allows access to and modification of population state.
     * 
     * @param id Population ID to retrieve
     * @return Pointer to neural population
     * 
     * @pre id must be a valid population ID
     * @post Return value is valid as long as region is initialized
     * @complexity O(1) for lookup
     * 
     * @note Returns nullptr if population not found
     * @see getPopulation(PopulationId) const - Const version
     * @see addPopulation() - For adding populations
     */
    NeuralPopulation* getPopulation(PopulationId id);
    
    /**
     * @brief Get neural population (const version)
     * 
     * Returns a const pointer to the neural population with the specified ID.
     * This provides read-only access to population state, useful for
     * inspection and debugging.
     * 
     * @param id Population ID to retrieve
     * @return Const pointer to neural population
     * 
     * @pre id must be a valid population ID
     * @post Return value is valid as long as region is initialized
     * @complexity O(1) for lookup
     * 
     * @note Returns nullptr if population not found
     * @see getPopulation(PopulationId) - Non-const version
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * @brief Get number of populations
     * 
     * Returns the total number of neural populations in this region.
     * This is an important statistic for understanding region complexity
     * and computational capacity.
     * 
     * @return Number of populations
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see getPopulation(PopulationId) - For accessing specific population
     * @see getPopulations() - For accessing all populations
     */
    size_t getPopulationCount() const;
    
    /**
     * @brief Get all neural populations
     * 
     * Returns a const reference to the vector containing all neural
     * populations in this region. This provides access to the complete
     * regional population structure.
     * 
     * @return Const reference to vector of neural populations
     * 
     * @post Return value stable during simulation
     * @complexity O(1) for reference, O(n) for copying
     * 
     * @note Returns populations in creation order
     * @see getPopulation() - For accessing specific population
     * @see addPopulation() - For adding populations
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    /**
     * @brief Get all neural populations (non-const version)
     * 
     * Returns a vector of pointers to all neural populations in this region.
     * This provides mutable access to all populations in the region.
     * 
     * @return Vector of neural population pointers
     * 
     * @post Return value stable during simulation
     * @complexity O(n) for creating new vector
     * 
     * @note Useful for batch operations on all populations
     * @see getPopulations() const - Const version
     */
    std::vector<NeuralPopulation*> getAllPopulations();
    
    /**
     * @brief Add local synapse
     * 
     * Creates and adds a local synapse to this region. Local synapses
     * are connections between neurons within the same region, enabling
     * local computation and processing. This is the primary method for
     * building regional connectivity.
     * 
     * @param source Source neuron ID
     * @param destination Destination neuron ID
     * @param weight Synaptic weight (default: 0.0f)
     * @param delay Transmission delay (default: 1 step)
     * @return SynapseId Unique ID for the created synapse
     * 
     * @pre source and destination must be valid neuron IDs within this region
     * @pre source != destination (no self-connections)
     * @post New synapse created and added to region
     * @post Synapse available for neural computation
     * 
     * @note Only allows neurons within this region
     * @note Self-connections are not allowed
     * @see Synapse - For synapse class
     * @see getSynapse() - For accessing synapses
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                        SynapticWeight weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Get synapse (non-const version)
     * 
     * Returns a pointer to the synapse with the specified ID within this region.
     * This allows access to and modification of synapse state.
     * 
     * @param id Synapse ID to retrieve
     * @return Pointer to synapse
     * 
     * @pre id must be a valid synapse ID in this region
     * @post Return value is valid as long as region is initialized
     * @complexity O(1) for lookup
     * 
     * @note Returns nullptr if synapse not found
     * @see getSynapse(SynapseId) const - Const version
     */
    Synapse* getSynapse(SynapseId id);
    
    /**
     * @brief Get synapse (const version)
     * 
     * Returns a const pointer to the synapse with the specified ID within this region.
     * This provides read-only access to synapse state.
     * 
     * @param id Synapse ID to retrieve
     * @return Const pointer to synapse
     * 
     * @pre id must be a valid synapse ID in this region
     * @post Return value is valid as long as region is initialized
     * @complexity O(1) for lookup
     * 
     * @note Returns nullptr if synapse not found
     * @see getSynapse(SynapseId) - Non-const version
     */
    const Synapse* getSynapse(SynapseId id) const;
    
    /**
     * @brief Get number of synapses
     * 
     * Returns the total number of synapses in this region. This is an
     * important statistic for understanding regional connectivity and
     * computational complexity.
     * 
     * @return Number of synapses
     * 
     * @post Return value stable during simulation
     * @complexity O(1)
     * 
     * @see getSynapse() - For accessing specific synapse
     * @see getSynapses() - For accessing all synapses
     */
    size_t getSynapseCount() const;
    
    /**
     * @brief Get all synapses
     * 
     * Returns a const reference to the vector containing all synapses in this
     * region. This provides access to the complete regional synaptic structure.
     * 
     * @return Const reference to vector of synapses
     * 
     * @post Return value stable during simulation
     * @complexity O(1) for reference, O(n) for copying
     * 
     * @note Returns synapses in creation order
     * @see getSynapse() - For accessing specific synapse
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    /**
     * @brief Get all synapses from a specific neuron
     * 
     * Returns a vector of synapses originating from the specified neuron,
     * representing all outgoing connections within this region.
     * 
     * @param neuron Neuron ID to query
     * @return Vector of synapse pointers (source = neuron)
     * 
     * @pre neuron must be a valid neuron ID in this region
     * @post Return value is stable during simulation
     * @complexity O(n) where n is synapse count
     * 
     * @note Includes all outgoing connections from neuron
     * @see getSynapsesTo() - For incoming connections
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    /**
     * @brief Get all synapses to a specific neuron
     * 
     * Returns a vector of synapses targeting the specified neuron,
     * representing all incoming connections within this region.
     * 
     * @param neuron Neuron ID to query
     * @return Vector of synapse pointers (destination = neuron)
     * 
     * @pre neuron must be a valid neuron ID in this region
     * @post Return value is stable during simulation
     * @complexity O(n) where n is synapse count
     * 
     * @note Includes all incoming connections to neuron
     * @see getSynapsesFrom() - For outgoing connections
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    /**
     * @brief Get total neuron count
     * 
     * Returns the total number of neurons in all populations of this region.
     * This is an important statistic for understanding region size and
     * computational capacity.
     * 
     * @return Total neuron count
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is population count
     * 
     * @see getTotalSynapseCount() - For synaptic count
     * @see getActiveNeuronCount() - For active neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get active neuron count
     * 
     * Returns the number of neurons currently firing or active in all
     * populations of this region. This is a key indicator of current
     * regional activity levels.
     * 
     * @return Number of active neurons
     * 
     * @post Return value fluctuates during simulation
     * @complexity O(n) where n is neuron count
     * 
     * @note Used for regional activity monitoring
     * @note Critical for resource allocation
     * @see getFiringNeuronCount() - For firing neurons
     * @see getTotalNeuronCount() - For total neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * 
     * Returns the number of neurons that have fired in the current timestep
     * across all populations in this region. This differs from active neurons,
     * which includes neurons that are currently firing.
     * 
     * @return Number of neurons that fired this timestep
     * 
     * @post Return value resets each timestep
     * @complexity O(n) where n is neuron count
     * 
     * @see getActiveNeuronCount() - For currently active neurons
     * @see getTotalSpikeCount() - For cumulative spikes
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * 
     * Calculates and returns the average firing rate across all neurons
     * in all populations of this region. This is an important metric for
     * understanding regional dynamics and performance.
     * 
     * @return Average firing rate in Hz (spikes per second)
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is neuron count
     * 
     * @note Weighted by number of spikes per neuron
     * @note Critical for regional analysis
     * @see getTotalSpikeCount() - For total spikes
     * @see getFiringNeuronCount() - For firing neurons
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get average synaptic weight
     * 
     * Calculates and returns the average synaptic weight of all synapses
     * in this region. This is an important indicator of regional connectivity
     * strength and computational dynamics.
     * 
     * @return Average synaptic weight
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is synapse count
     * 
     * @note Reflects regional connectivity strength
     * @see getSynapticDensity() - For connectivity density
     */
    float getAverageSynapticWeight() const;
    
    /**
     * @brief Get synaptic density
     * 
     * Calculates and returns the synaptic density of this region, defined
     * as the fraction of possible connections that are actually present.
     * This is a measure of regional connectivity sparsity.
     * 
     * @return Synaptic density (0.0 to 1.0)
     * 
     * @post Return value stable during simulation
     * @complexity O(n*m) where n is neuron count, m is possible connections
     * 
     * @note 0.0 = no connections, 1.0 = complete connectivity
     * @see getTotalNeuronCount() - For neuron count
     * @see getAverageSynapticWeight() - For average weight
     */
    float getSynapticDensity() const;  // fraction of possible connections
    
    /**
     * @brief Step all populations and synapses
     * 
     * Advances the region by one timestep, updating all neuron and
     * synapse computations. This is the primary computation function
     * for the region that:
     * 1. Advances all neurons through their dynamics
     * 2. Updates all synaptic connections
     * 3. Applies regional plasticity
     * 4. Manages regional state
     * 
     * @param currentTime Current simulation time (ms)
     * 
     * @pre currentTime must be non-negative
     * @post All regional computations updated
     * @post Regional state advanced
     * 
     * @note Calls step() on all populations
     * @note Calls step() on all synapses
     * @note Primary regional computation function
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset all neurons and synapses
     * 
     * Resets the region to its initial state, undoing all changes
     * accumulated during simulation. This includes:
     * - All neurons reset to initial state
     * - All synapses reset to initial state
     * - Regional state cleared
     * - Activity counters reset
     * 
     * @post Region returned to initial state
     * @post Ready for new simulation
     * @post All regional variables reset
     * 
     * @see step() - For simulation stepping
     * @see initializeRandomConnectivity() - For connectivity setup
     */
    void reset();
    
    /**
     * @brief Initialize random connectivity
     * 
     * Initializes the regional connectivity with random parameters,
     * creating a biologically realistic synaptic connectivity pattern.
     * This function is essential for creating diverse and interesting
     * regional dynamics.
     * 
     * @param rng Random number generator for reproducible initialization
     * @param connectionProbability Probability of each possible connection
     * @param meanWeight Mean of synaptic weight distribution
     * @param weightVariance Variance of synaptic weight distribution
     * 
     * @pre rng must be valid and seeded
     * @pre 0.0 <= connectionProbability <= 1.0
     * @post Regional connectivity initialized
     * @post All synapses created with random parameters
     * 
     * @note Creates sparse random connectivity
     * @note Weight distribution affects dynamics
     * @see reset() - For state reset
     * @see getSynapticDensity() - For connectivity statistics
     */
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                      float connectionProbability,
                                      float meanWeight,
                                      float weightVariance);
    
    /**
     * @brief Get all neurons across all populations
     * 
     * Returns a vector of pointers to all neurons in all populations
     * of this region. This provides access to the complete regional
     * neuronal structure.
     * 
     * @return Vector of neuron pointers
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is total neuron count
     * 
     * @note Includes neurons from all populations
     * @see getAllNeurons() const - Const version
     */
    std::vector<Neuron*> getAllNeurons();
    
    /**
     * @brief Get all neurons across all populations (const version)
     * 
     * Returns a vector of const pointers to all neurons in all populations
     * of this region. This provides read-only access to all regional neurons.
     * 
     * @return Vector of const neuron pointers
     * 
     * @post Return value stable during simulation
     * @complexity O(n) where n is total neuron count
     * 
     * @note Includes neurons from all populations
     * @see getAllNeurons() - Non-const version
     */
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
