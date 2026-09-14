#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

/**
     * @brief NeuralRegion: A brain region containing multiple populations and local connectivity
     * @brief PLACEHOLDER - Phase 2 will implement region-level dynamics
     * 
     * Neural regions are specialized groups of neurons that serve different functional
     * purposes in the brain. Each region has its own characteristic neuron types,
     * connectivity patterns, and computational functions. Regions enable hierarchical
     * organization of neural processing.
     * 
     * Region types in biological systems:
     * - Sensory regions: Process external inputs
     * - Motor regions: Generate behavioral outputs
     * - Association regions: Integrate information across modalities
     * - Memory regions: Store and retrieve experiences
     * - Executive regions: Coordinate complex behaviors
     * 
     * Computational benefits:
     * - Enables modular processing
     * - Supports hierarchical information flow
     * - Allows for region-specific plasticity
     * - Facilitates functional specialization
     */
class NeuralRegion {
public:
    /**
     * @brief Construct a neural region with a unique identifier
     * 
     * Creates a new brain region with the specified ID. The region starts with
     * no populations and no internal neurons. Populations and synapses must be
     * added before the region can participate in neural computation.
     * 
     * @param id Unique identifier for this region within the brain
     * @pre id must be valid and not represent a reserved region ID
     * @post Region object is created with initial empty state
     * @post Region ID is set
     * @post Region name is empty
     * @post No populations or synapses exist initially
     * 
     * @note Region IDs should be unique within a brain
     * @see RegionId for identifier structure
     */
    explicit NeuralRegion(RegionId id);
    
    /**
     * @brief Construct a neural region with identifier and name
     * 
     * Creates a named brain region, useful for identification and debugging.
     * The region can be referenced by its name in logging and configuration.
     * 
     * @param id Unique identifier for this region within the brain
     * @param name Human-readable name for the region
     * @pre id must be valid and not represent a reserved region ID
     * @post Region object is created with name set
     * @post Region ID is set
     * @post Region name is set to provided value
     */
    NeuralRegion(RegionId id, const std::string& name);
    
    /**
     * @brief Destroy the neural region and clean up all resources
     * 
     * This destructor ensures proper cleanup of all region resources,
     * including deletion of all populations and synapses. The region
     * is removed from any brain that contained it.
     * 
     * @post All populations are destroyed
     * @post All synapses are destroyed
     * @post Region is removed from parent brain
     * @post Memory is properly deallocated
     */
    ~NeuralRegion();
    
    // Disable copying, enable moving
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&&) noexcept;
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    /**
     * @brief Get the unique identifier of this region
     * 
     * @return The region's identifier
     * @pre Region object must be valid
     * @post Identifier is returned unchanged
     */
    RegionId getId() const;
    
    /**
     * @brief Get the human-readable name of this region
     * 
     * Region names provide semantic information about the region's
     * function and purpose. They are useful for debugging, logging,
     * and configuration.
     * 
     * @return Const reference to the region's name string
     * @pre Region object must be valid
     * @post Name string is returned
     */
    const std::string& getName() const;
    
    /**
     * @brief Set the human-readable name of this region
     * 
     * Changes the region's name. This is primarily used for identification
     * and debugging purposes. Changing the name does not affect region
     * functionality or connectivity.
     * 
     * @param name New human-readable name for the region
     * @pre Region object must be valid
     * @post Region name is updated
     */
    void setName(const std::string& name);
    
    /**
     * @brief Add a new neural population to this region
     * 
     * Creates a new population containing neurons with shared properties
     * and adds it to the region's collection of populations. Populations
     * enable grouping of neurons by type and function.
     * 
     * @param size Number of neurons in the new population
     * @param type Neuron type for all neurons in the population (default: Internal)
     * @return Unique identifier for the newly created population
     * @pre Region object must be valid
     * @post Population is created and added to region
     * @post Population ID is unique and valid
     * @post Population is initialized with specified size and type
     * 
     * @note Population IDs are unique within a region
     * @see PopulationId for identifier structure
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    /**
     * @brief Get a reference to a specific population in this region
     * 
     * Provides access to a population by its ID for manipulation and observation.
     * This is the primary interface for working with individual populations.
     * 
     * @param id The unique ID of the population to retrieve
     * @return Pointer to the NeuralPopulation instance, or nullptr if not found
     * @pre Region object must be valid
     * @post Returns reference to requested population
     * 
     * @note Returns nullptr if population ID is invalid
     */
    NeuralPopulation* getPopulation(PopulationId id);
    
    /**
     * @brief Get a const reference to a specific population in this region
     * 
     * Const version of getPopulation() for read-only access to population data.
     * 
     * @param id The unique ID of the population to retrieve
     * @return Const pointer to the NeuralPopulation instance, or nullptr if not found
     * @pre Region object must be valid
     * @post Returns const reference to requested population
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * @brief Get the number of populations in this region
     * 
     * @return Number of active populations in the region
     * @pre Region object must be valid
     * @post Returns count of all populations
     */
    size_t getPopulationCount() const;
    
    /**
     * @brief Get all populations in this region
     * 
     * Returns a const reference to the vector of all populations in the region.
     * The populations are owned by the region and should not be deleted externally.
     * 
     * @return Const reference to vector of all populations
     * @pre Region object must be valid
     * @post Returns reference to all populations
     * 
     * @note Populations are owned by the region
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    /**
     * @brief Get all population pointers in this region
     * 
     * Returns a vector of raw pointers to all populations in the region.
     * This is useful for iteration but care must be taken not to delete
     * the returned pointers as they are owned by the region.
     * 
     * @return Vector of population pointers
     * @pre Region object must be valid
     * @post Returns pointers to all populations
     */
    std::vector<NeuralPopulation*> getAllPopulations();
    
    /**
     * @brief Add a synaptic connection within this region
     * 
     * Creates a new synapse connecting two neurons within the same region.
     * This is for local connectivity. For connections between regions,
     * use Brain::addInterRegionConnection().
     * 
     * @param source Source neuron ID within this region
     * @param destination Destination neuron ID within this region
     * @param weight Synaptic weight for the connection (default: 0.0f)
     * @param delay Transmission delay in simulation steps (default: 1)
     * @return Unique identifier for the newly created synapse
     * @pre Region object must be valid
     * @post Synapse is created and added to region
     * @post Synapse ID is unique within the region
     * @post Connection is established between source and destination
     * 
     * @note This creates intra-regional connectivity
     * @see Brain::addInterRegionConnection() for inter-regional connections
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Get a reference to a specific synapse in this region
     * 
     * Provides access to a synapse by its ID for manipulation and observation.
     * 
     * @param id The unique ID of the synapse to retrieve
     * @return Pointer to the Synapse instance, or nullptr if not found
     * @pre Region object must be valid
     * @post Returns reference to requested synapse
     */
    Synapse* getSynapse(SynapseId id);
    
    /**
     * @brief Get a const reference to a specific synapse in this region
     * 
     * Const version of getSynapse() for read-only access to synapse data.
     * 
     * @param id The unique ID of the synapse to retrieve
     * @return Const pointer to the Synapse instance, or nullptr if not found
     * @pre Region object must be valid
     * @post Returns const reference to requested synapse
     */
    const Synapse* getSynapse(SynapseId id) const;
    
    /**
     * @brief Get the number of synapses in this region
     * 
     * @return Number of synaptic connections in the region
     * @pre Region object must be valid
     * @post Returns synapse count
     */
    size_t getSynapseCount() const;
    
    /**
     * @brief Get all synapses in this region
     * 
     * Returns a const reference to the vector of all synapses in the region.
     * Synapses are owned by the region and should not be deleted externally.
     * 
     * @return Const reference to vector of all synapses
     * @pre Region object must be valid
     * @post Returns reference to all synapses
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    /**
     * @brief Get all synapses originating from a specific neuron
     * 
     * Returns a vector of synapse pointers for all synapses where the
     * specified neuron is the source (presynaptic). This is useful for
     * spike propagation and connectivity analysis.
     * 
     * @param neuron The neuron ID to query
     * @return Vector of synapse pointers originating from the neuron
     * @pre Region object must be valid
     * @post Returns list of outgoing synapses
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    /**
     * @brief Get all synapses terminating at a specific neuron
     * 
     * Returns a vector of synapse pointers for all synapses where the
     * specified neuron is the destination (postsynaptic). This is useful
     * for synaptic input analysis and connectivity statistics.
     * 
     * @param neuron The neuron ID to query
     * @return Vector of synapse pointers terminating at the neuron
     * @pre Region object must be valid
     * @post Returns list of incoming synapses
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    /**
     * @brief Get the total number of neurons across all populations in this region
     * 
     * @return Total count of all neurons in the region
     * @pre Region object must be valid
     * @post Returns total neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the number of neurons currently active in this region
     * 
     * @return Count of active neurons (firing or refractory state)
     * @pre Region object must be valid
     * @post Returns active neuron count
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the number of neurons currently firing spikes in this region
     * 
     * @return Count of neurons in refractory state (just fired)
     * @pre Region object must be valid
     * @post Returns firing neuron count
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons in this region
     * 
     * Computes the mean firing rate (in Hz) of all neurons in the region,
     * providing a regional measure of neural activity.
     * 
     * @return Average firing rate in Hz
     * @pre Region object must be valid
     * @post Returns computed average
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get the average synaptic weight in this region
     * 
     * Computes the mean synaptic weight across all synapses in the region,
     * providing a measure of overall synaptic strength and balance.
     * 
     * @return Average synaptic weight
     * @pre Region object must be valid
     * @post Returns computed average
     */
    float getAverageSynapticWeight() const;
    
    /**
     * @brief Get the synaptic density of this region
     * 
     * Computes the fraction of possible synaptic connections that are
     * actually present in the region. This measures how connected the
     * region is relative to its potential.
     * 
     * @return Synaptic density (0.0 to 1.0)
     * @pre Region object must be valid
     * @post Returns computed density
     */
    float getSynapticDensity() const;  // fraction of possible connections
    
    /**
     * @brief Perform one simulation step for all components in this region
     * 
     * This method advances all populations and synapses in the region by
     * one simulation step, coordinating their interactions and updating
     * their internal states.
     * 
     * @param currentTime Current simulation time in seconds
     * @pre Region object must be valid
     * @post All populations are updated
     * @post All synapses are updated
     * @post Regional dynamics are computed
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset all neurons and synapses in this region
     * 
     * This method returns all components in the region to their initial
     * states, clearing activity, resetting potentials, and clearing
     * synaptic modifications.
     * 
     * @pre Region object must be valid
     * @post All neurons are reset to initial state
     * @post All synapses are reset to initial state
     * @post Regional state is cleared
     */
    void reset();
    
    /**
     * @brief Initialize random connectivity within this region
     * 
     * Creates random synaptic connections between neurons in this region
     * according to the specified parameters. This implements the initial
     * wiring of the region's neural network.
     * 
     * @param rng Reference to random number generator
     * @param connectionProbability Probability of connection between neuron pairs
     * @param meanWeight Mean synaptic weight for new connections
     * @param weightVariance Variance of synaptic weights for new connections
     * @pre Region object must be valid
     * @post Random connections are established
     * @post Synaptic weights are set according to parameters
     * @post Connectivity patterns are biologically plausible
     */
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                      float connectionProbability,
                                      float meanWeight,
                                      float weightVariance);
    
    /**
     * @brief Get all neurons across all populations in this region
     * 
     * Returns a vector of raw pointers to all neurons in the region.
     * This is useful for global operations but care must be taken not
     * to delete the returned pointers as they are owned by the region.
     * 
     * @return Vector of neuron pointers
     * @pre Region object must be valid
     * @post Returns pointers to all neurons in the region
     */
    std::vector<Neuron*> getAllNeurons();
    
    /**
     * @brief Get all neurons across all populations in this region (const version)
     * 
     * Const version of getAllNeurons() for read-only access to neuron data.
     * 
     * @return Vector of const neuron pointers
     * @pre Region object must be valid
     * @post Returns const pointers to all neurons in the region
     */
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
