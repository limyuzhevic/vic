#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

/**
 * NeuralRegion - Brain region containing multiple populations and local connectivity
 * @brief Represents a brain region with population organization and local synaptic connections
 * @details This class models brain regions as functional units containing:
 *         - Multiple neural populations (sensory, internal, motor)
 *         - Local synaptic connectivity within the region
 *         - Regional statistics and properties
 *         - Population management and organization
 *         
 * Region characteristics:
 * - Regional boundaries: No synaptic connections between regions (except via inter-region connections)
 * - Population specialization: Each region contains specialized populations
 * - Local processing: Region handles internal neural dynamics independently
 * - Regional statistics: Region-level metrics for system analysis
 * 
 * Memory model: Uses pointer-to-implementation (PIMPL) for stable ABI and efficient memory management.
 * Thread safety: Not thread-safe; external synchronization required for concurrent access.
 * 
 * @warning Region IDs are sequential and immutable after creation
 * @note Regional boundaries are important for understanding neural system architecture
 * @see Brain for inter-region connection management
 */
class NeuralRegion {
public:
    /**
     * @brief Constructor - creates region with ID
     * @param id Unique identifier for this region
     * @post Region created with empty state; ready for population/synapse management
     * @warning Region ID is immutable after creation
     */
    explicit NeuralRegion(RegionId id);
    
    /**
     * @brief Constructor - creates region with ID and name
     * @param id Unique identifier for this region
     * @param name Human-readable name for the region
     * @post Region created with name; ready for population/synapse management
     * @warning Region ID is immutable after creation
     */
    NeuralRegion(RegionId id, const std::string& name);
    
    /**
     * @brief Destructor - cleans up resources
     * @post All populations and synapses destroyed; memory released
     */
    ~NeuralRegion();
    
    /**
     * @brief Copy constructor - deleted to prevent copying
     * @details Regions should not be copied to maintain unique identity
     * @warning Attempting to copy will result in compilation error
     */
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    
    /**
     * @brief Move constructor - enables efficient region transfer
     * @param other Region to move from (will be invalidated)
     * @post this->id = other->id, other->pImpl = nullptr
     * @note Move semantics provide efficient transfer without deep copying
     */
    NeuralRegion(NeuralRegion&&) noexcept;
    
    /**
     * @brief Move assignment operator - enables efficient region reassignment
     * @param other Region to move from (will be invalidated)
     * @post this->id = other->id, other->pImpl = nullptr
     * @return Reference to this region
     * @note Move assignment provides efficient transfer without deep copying
     */
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
/**
     * @brief Get region identifier
     * @return RegionId of this region
     * @post None
     * @note Region ID is immutable after creation
     */
    RegionId getId() const;
    
    /**
     * @brief Get region name
     * @return Reference to region name string
     * @post None
     */
    const std::string& getName() const;
    
    /**
     * @brief Set region name
     * @param name New name for the region
     * @post Name updated
     * @warning Name changes may affect user interface or debugging output
     */
    void setName(const std::string& name);
    
    /**
     * @brief Create new neural population
     * @param size Number of neurons in the population
     * @param type Neuron type (default: Internal)
     * @return PopulationId of newly created population
     * @post Population created; neurons initialized with zero membrane potential
     * @warning Population IDs are sequential and unique within the region
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    /**
     * @brief Get neural population by ID
     * @param id Population identifier
     * @return Pointer to population if found, nullptr otherwise
     * @post None
     * @note Returns nullptr if population doesn't exist
     */
    NeuralPopulation* getPopulation(PopulationId id);
    
    /**
     * @brief Get neural population by ID (const version)
     * @param id Population identifier
     * @return Const pointer to population if found, nullptr otherwise
     * @post None
     * @note Returns nullptr if population doesn't exist
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * @brief Get population count
     * @return Number of populations in region
     * @post None
     */
    size_t getPopulationCount() const;
    
    /**
     * @brief Get all populations in region
     * @return Const reference to vector of all populations
     * @post None
     * @note Useful for iterating over all populations
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    /**
     * @brief Get all population pointers (mutable version)
     * @return Vector of population pointers
     * @post None
     * @note Useful for iterating over all populations
     */
    std::vector<NeuralPopulation*> getAllPopulations();
    
    /**
     * @brief Create synapse between neurons in this region
     * @param source NeuronId of presynaptic neuron
     * @param destination NeuronId of postsynaptic neuron
     * @param weight Synaptic weight (default: 0.0)
     * @param delay Synaptic delay in steps (default: 1)
     * @return SynapseId of newly created synapse
     * @post Synapse created; registered in region's connectivity maps
     * @warning Synapse IDs are sequential and unique within the region
     * @warning No validation that source/destination neurons exist
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination,
                        SynapticWeight weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Get synapse by ID
     * @param id Synapse identifier
     * @return Pointer to synapse if found, nullptr otherwise
     * @post None
     * @note Returns nullptr if synapse doesn't exist
     */
    Synapse* getSynapse(SynapseId id);
    
    /**
     * @brief Get synapse by ID (const version)
     * @param id Synapse identifier
     * @return Const pointer to synapse if found, nullptr otherwise
     * @post None
     * @note Returns nullptr if synapse doesn't exist
     */
    const Synapse* getSynapse(SynapseId id) const;
    
    /**
     * @brief Get synapse count
     * @return Number of synapses in region
     * @post None
     */
    size_t getSynapseCount() const;
    
    /**
     * @brief Get all synapses in region
     * @return Const reference to vector of all synapses
     * @post None
     * @note Useful for iterating over all synapses
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    /**
     * @brief Get synapses originating from neuron
     * @param neuron Neuron identifier
     * @return Vector of synapse pointers originating from neuron
     * @post None
     * @note Returns empty vector if neuron doesn't exist or has no outgoing synapses
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    /**
     * @brief Get synapses terminating at neuron
     * @param neuron Neuron identifier
     * @return Vector of synapse pointers terminating at neuron
     * @post None
     * @note Returns empty vector if neuron doesn't exist or has no incoming synapses
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    /**
     * @brief Get total neuron count across all populations
     * @return Total number of neurons in region
     * @post None
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get active neuron count
     * @return Number of neurons with elevated membrane potential
     * @post None
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * @return Number of neurons that fired in current step
     * @post Value may change each step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * @return Average firing rate in Hz
     * @post None
     * @note Computed as sum of firing rates divided by population count
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get average synaptic weight
     * @return Average synaptic weight across all synapses
     * @post None
     * @note Returns 0.0 if no synapses exist
     */
    float getAverageSynapticWeight() const;
    
    /**
     * @brief Get synaptic density
     * @return Synaptic density as fraction of possible connections
     * @post None
     * @note Computed as actual synapses / maximum possible synapses
     * @note Maximum possible: N * (N-1) for directed graph
     */
    float getSynapticDensity() const;
    
    /**
     * @brief Step all populations and synapses
     * @param currentTime Current simulation time step
     * @post All populations and synapses updated
     * @note Calls population::step() and synapse::step() sequentially
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset all neurons and synapses
     * @post All neurons and synapses reset to initial state
     * @note Sets membrane potential to resting potential
     * @note Clears spike history and synaptic inputs
     */
    void reset();
    
    /**
     * @brief Initialize connectivity with random parameters
     * @param rng Reference to random number generator
     * @param connectionProbability Probability of synaptic connection (0.0-1.0)
     * @param meanWeight Mean synaptic weight
     * @param weightVariance Variance of synaptic weight
     * @post Creates random synapses based on neuron types and connection rules
     * @details Creates synapses based on pre-synaptic neuron type:
     *         - Excitatory neurons: positive weights
     *         - Inhibitory neurons: negative weights  
     *         - Other types: small random weights
     * @note No self-connections (i ≠ j)
     * @note Modulatory neurons do not create connections
     */
    void initializeRandomConnectivity(class RandomGenerator& rng,
                                      float connectionProbability,
                                      float meanWeight,
                                      float weightVariance);
    
    /**
     * @brief Get all neurons across all populations
     * @return Vector of neuron pointers
     * @post None
     * @note Useful for region-wide operations
     */
    std::vector<Neuron*> getAllNeurons();
    
    /**
     * @brief Get all neurons across all populations (const version)
     * @return Vector of const neuron pointers
     * @post None
     * @note Useful for region-wide const operations
     */
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
