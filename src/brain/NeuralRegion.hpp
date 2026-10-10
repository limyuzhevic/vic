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
 * 
 * Brain regions are modular computational units that organize neurons into functional
 * groups based on their computational roles, connectivity patterns, or developmental origins.
 * Each region contains neural populations and implements local connectivity rules,
 * enabling distributed brain architectures with both local processing and global integration.
 * 
 * Key features:
 * - Population-based organization of neurons
 * - Local synaptic connectivity within regions
 * - Regional statistics and monitoring
 * - Random connectivity initialization
 * - Regional development and adaptation
 * - Integration with global brain systems
 * 
 * Regions enable several important architectural benefits:
 * - Modularity: Each region can operate semi-independently
 * - Scalability: Large brains can be composed of many small regions
 * - Development: Regions can differentiate and specialize
 * - Computation: Local processing reduces global communication load
 */
class NeuralRegion {
public:
    /**
     * @brief Construct a new NeuralRegion with specified identifier
     * 
     * @param id Unique identifier for this brain region
     * 
     * @note Each region must have a unique identifier that can be used for
     *       inter-region connections and system-wide operations.
     */
    explicit NeuralRegion(RegionId id);
    
    /**
     * @brief Construct a new NeuralRegion with identifier and name
     * 
     * @param id Unique identifier for this brain region
     * @param name Human-readable name for the region (defaults to empty string)
     * 
     * @note Region names are primarily for debugging, logging, and visualization.
     *       They do not affect computational behavior or connectivity.
     */
    NeuralRegion(RegionId id, const std::string& name);
    
    /**
     * @brief Destroy the NeuralRegion instance
     * 
     * @note Ensures proper cleanup of all contained populations, synapses,
     *       and allocated memory. The region should be properly reset before
     *       destruction to avoid memory leaks.
     */
    ~NeuralRegion();
    
    // Disable copying, enable moving for efficient resource management
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&&) noexcept;
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    /**
     * @brief Get the region's unique identifier
     * 
     * @return RegionId The region's identifier
     * 
     * @note Used for system-wide operations, inter-region connectivity,
     *       and region management functions.
     */
    RegionId getId() const;
    
    /**
     * @brief Get the region's human-readable name
     * 
     * @return const std::string& The region's name
     * 
     * @note Names are primarily for debugging, logging, and visualization.
     *       Multiple regions can have the same name if needed for organization.
     */
    const std::string& getName() const;
    
    /**
     * @brief Set the region's name
     * 
     * @param name New human-readable name for the region
     * 
     * @note Changes to region names take effect immediately and affect
     *       logging, debugging output, and visualization.
     */
    void setName(const std::string& name);
    
    // -------------------------------------------------------------------------
    // POPULATION MANAGEMENT
    // -------------------------------------------------------------------------
    
    /**
     * @brief Create a new neural population within this region
     * 
     * @param size Number of neurons in the new population
     * @param type Neuron type for all neurons in the population (default: Internal)
     * @return PopulationId Unique identifier for the created population
     * 
     * @note Populations are the basic computational units within regions.
     *       All neurons in a population share the same type and typically
     *       have similar computational properties. Populations enable
     *       population-level operations and statistics.
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    /**
     * @brief Get a reference to a specific neural population
     * 
     * @param id Population identifier
     * @return NeuralPopulation* Pointer to the requested population
     * 
     * @note Returns nullptr if population ID is invalid or population doesn't exist.
     *       Use getPopulationCount() to verify population existence.
     */
    NeuralPopulation* getPopulation(PopulationId id);
    
    /**
     * @brief Get constant reference to a specific neural population
     * 
     * @param id Population identifier
     * @return const NeuralPopulation* Const pointer to the requested population
     * 
     * @note Thread-safe read access. Returns nullptr if population ID is invalid.
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * @brief Get the number of populations in this region
     * 
     * @return size_t Number of neural populations
     */
    size_t getPopulationCount() const;
    
    /**
     * @brief Get all population identifiers in this region
     * 
     * @return const std::vector<std::unique_ptr<NeuralPopulation>>& Reference to population collection
     * 
     * @note Provides access to all populations for comprehensive regional control.
     *       Populations are owned by the region and managed automatically.
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    /**
     * @brief Get all neural populations as pointers (non-const version)
     * 
     * @return std::vector<NeuralPopulation*> Vector of population pointers
     * 
     * @note Use this method when you need to modify populations directly.
     *       Be careful not to create ownership conflicts.
     */
    std::vector<NeuralPopulation*> getAllPopulations();
    
    // -------------------------------------------------------------------------
    // SYNAPSE MANAGEMENT (LOCAL CONNECTIVITY)
    // -------------------------------------------------------------------------
    
    /**
     * @brief Create a synaptic connection within this region
     * 
     * @param source Source neuron identifier
     * @param destination Target neuron identifier
     * @param weight Initial synaptic weight (default: 0.0)
     * @param delay Transmission delay in simulation steps (default: 1)
     * @return SynapseId Unique identifier for the created synapse
     * 
     * @note This creates local connectivity within the region. For connections
     *       between different regions, use the brain's addInterRegionConnection()
     *       method. Local synapses are typically faster to process and have
     *       lower transmission delays.
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Get a reference to a specific synapse
     * 
     * @param id Synapse identifier
     * @return Synapse* Pointer to the requested synapse
     * 
     * @note Returns nullptr if synapse ID is invalid or synapse doesn't exist.
     */
    Synapse* getSynapse(SynapseId id);
    
    /**
     * @brief Get constant reference to a specific synapse
     * 
     * @param id Synapse identifier
     * @return const Synapse* Const pointer to the requested synapse
     * 
     * @note Thread-safe read access. Returns nullptr if synapse ID is invalid.
     */
    const Synapse* getSynapse(SynapseId id) const;
    
    /**
     * @brief Get the number of synapses in this region
     * 
     * @return size_t Number of synaptic connections within the region
     */
    size_t getSynapseCount() const;
    
    /**
     * @brief Get all synapses in this region
     * 
     * @return const std::vector<std::unique_ptr<Synapse>>& Reference to synapse collection
     * 
     * @note Provides access to all local synaptic connections for regional processing.
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    /**
     * @brief Get all synapses originating from a specific neuron
     * 
     * @param neuron Neuron identifier
     * @return std::vector<Synapse*> Vector of outgoing synapses from the neuron
     * 
     * @note Useful for spike propagation, synaptic transmission, and plasticity
     *       computations. Returns empty vector if neuron doesn't exist or has no outgoing connections.
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    /**
     * @brief Get all synapses targeting a specific neuron
     * 
     * @param neuron Neuron identifier
     * @return std::vector<Synapse*> Vector of incoming synapses to the neuron
     * 
     * @note Useful for synaptic input processing, membrane potential integration,
     *       and eligibility trace computation. Returns empty vector if neuron doesn't exist.
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    // -------------------------------------------------------------------------
    // STATISTICS AND MONITORING
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the total number of neurons in this region
     * 
     * @return size_t Total neuron count across all populations
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the number of neurons currently active (firing)
     * 
     * @return size_t Count of neurons with firing state other than Resting
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the number of neurons currently in refractory period
     * 
     * @return size_t Count of neurons unable to fire due to refractory state
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons in the region
     * 
     * @return float Average firing rate in Hertz (spikes per second)
     * 
     * @note Provides a measure of regional activity level and computational engagement.
     */
    float getAverageFiringRate() const;
    
    // -------------------------------------------------------------------------
    // LOCAL CONNECTIVITY STATISTICS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get the average synaptic weight within the region
     * 
     * @return float Average synaptic weight across all local synapses
     * 
     * @note Reflects the overall strength of local connectivity and influences
     *       regional dynamics and information processing capabilities.
     */
    float getAverageSynapticWeight() const;
    
    /**
     * @brief Get the synaptic density (fraction of possible connections)
     * 
     * @return float Synaptic density (0.0 to 1.0)
     * 
     * @note Measures how densely connected the region is, which affects
     *       information propagation speed and computational capacity.
     */
    float getSynapticDensity() const;  // fraction of possible connections
    
    // -------------------------------------------------------------------------
    // SIMULATION OPERATIONS
    // -------------------------------------------------------------------------
    
    /**
     * @brief Execute one simulation step for all neurons and synapses in the region
     * 
     * @param currentTime Current simulation time (seconds)
     * 
     * @note This performs the core neural computation cycle including:
     * - LIF neuron state updates and spike generation
     * - Synaptic transmission and spike propagation
     * - Population-level activity computation
     * - Local connectivity processing
     * 
     * @note Regional stepping is typically called by the Brain class
     *       as part of the overall simulation pipeline.
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Reset all neurons and synapses to initial state
     * 
     * @note Clears all neural activity, synaptic weights, and internal state.
     *       Resets neurons to resting state with default parameters.
     *       Useful for preparing regions for new simulation runs.
     */
    void reset();
    
    /**
     * @brief Initialize random connectivity patterns within the region
     * 
     * @param rng Reference to random number generator
     * @param connectionProbability Probability of connection between neurons (0.0-1.0)
     * @param meanWeight Mean synaptic weight for random connections
     * @param weightVariance Standard deviation of synaptic weights
     * 
     * @note Creates sparse random connectivity patterns that are typical of
     *       biological neural networks. The resulting connectivity graph
     *       determines how information flows within the region.
     */
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                      float connectionProbability,
                                      float meanWeight,
                                      float weightVariance);
    
    // -------------------------------------------------------------------------
    // NEURON ACCESS AND PROCESSING
    // -------------------------------------------------------------------------
    
    /**
     * @brief Get all neurons across all populations in this region
     * 
     * @return std::vector<Neuron*> Vector of all neuron pointers in the region
     * 
     * @note Useful for system-wide operations that need to access all neurons
     *       in a specific region. Note that neurons are owned by their populations,
     *       so this returns pointers rather than ownership.
     */
    std::vector<Neuron*> getAllNeurons();
    
    /**
     * @brief Get all neurons across all populations as constant pointers
     * 
     * @return std::vector<const Neuron*> Vector of constant neuron pointers
     * 
     * @note Thread-safe read access. Useful for read-only operations that need
     *       to access neuron state across the entire region.
     */
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    struct Impl;
    Impl* pImpl;  ///< Private implementation for PIMPL pattern
};

} // namespace nlm