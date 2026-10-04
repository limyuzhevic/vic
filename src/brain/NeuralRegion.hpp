#pragma once

/**
 * @file NeuralRegion.hpp
 * @brief Neural region interface - brain regions containing populations and local connectivity
 * @author NLM Team
 * @date 2024
 * @version 1.0
 * @warning Phase 1 implementation. Full region-level dynamics implementation planned for Phase 2.
 */

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

/**
 * @namespace nlm
 * @brief Neural Machine Learning (NLM) - A spiking neural network framework for cognitive computing
 *
 * This namespace encompasses the entire NLM ecosystem, from low-level neuron dynamics to
 * high-level cognitive functions including memory, prediction, attention, and development.
 */

/**
 * @class NeuralRegion
 * @brief A brain region containing multiple populations and local connectivity
 * @details Neural regions organize neurons into functional groups with local connectivity.
 *          Each region contains populations of neurons that process information within
 *          specific functional domains (e.g., sensory, motor, association areas).
 *
 * Key features:
 * - Population management: create and access neuron populations
 * - Local connectivity: manage synapses within the region
 * - Statistics and monitoring: compute regional statistics
 * - Connectivity queries: find synapses to/from specific neurons
 * - Step processing: update all neurons and synapses in the region
 * - Random connectivity initialization
 * - Reset and state management
 *
 * Regions are hierarchical components of the brain, with each region containing
 * multiple populations, and populations containing multiple neurons. Regions also
 * manage local synaptic connectivity and provide statistics for the overall brain.
 */

class NeuralRegion {
public:
    /**
     * @brief Constructs a neural region with the given ID
     * @param id Unique identifier for the region
     * @details Creates a new neural region instance and initializes its internal state.
     *          The region is created empty with no populations or synapses.
     * @pre The region ID should be unique within the brain
     */
    explicit NeuralRegion(RegionId id);
    
    /**
     * @brief Constructs a neural region with ID and name
     * @param id Unique identifier for the region
     * @param name Human-readable name for the region
     * @details Creates a new neural region with a descriptive name for identification
     *          and debugging purposes.
     * @pre The region ID should be unique within the brain
     */
    NeuralRegion(RegionId id, const std::string& name);
    
    /**
     * @brief Destructor cleans up region resources
     * @details Deletes the private implementation (pImpl) and cleans up
     *          any allocated resources. This ensures proper memory management.
     */
    ~NeuralRegion();
    
    /**
     * @brief Disables copying (region is not copyable)
     */
    NeuralRegion(const NeuralRegion&) = delete;
    
    /**
     * @brief Disables copy assignment
     */
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    
    /**
     * @brief Enables move construction (efficient transfer of ownership)
     */
    NeuralRegion(NeuralRegion&&) noexcept;
    
    /**
     * @brief Enables move assignment (efficient transfer of ownership)
     */
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    /**
     * @brief Gets the region's unique identifier
     * @return RegionId The region's unique ID
     */
    RegionId getId() const;
    
    /**
     * @brief Gets the region's name
     * @return const std::string& The region's name
     */
    const std::string& getName() const;
    
    /**
     * @brief Sets the region's name
     * @param name New name for the region
     * @details Updates the region's human-readable name. Useful for
     *          debugging, visualization, and region identification.
     */
    void setName(const std::string& name);
    
    /**
     * @brief Creates a new neuron population
     * @param size Number of neurons in the population
     * @param type Neuron type for all neurons in the population (default: Internal)
     * @return PopulationId ID of the newly created population
     * @details Creates a new population of neurons with the specified size and type.
     *          Populations are functional units within regions that contain neurons
     *          with similar properties and functions.
     * @warning Population IDs are assigned sequentially starting from 1.
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    /**
     * @brief Accesses a neuron population by ID
     * @param id Population ID
     * @return NeuralPopulation* Pointer to the population, or nullptr if not found
     * @details Retrieves the population with the specified ID from this region.
     *          Returns nullptr if the population ID is invalid or doesn't exist.
     */
    NeuralPopulation* getPopulation(PopulationId id);
    
    /**
     * @brief Accesses a neuron population by ID (const version)
     * @param id Population ID
     * @return const NeuralPopulation* Const pointer to the population, or nullptr if not found
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * @brief Gets the number of populations in the region
     * @return size_t Number of populations
     */
    size_t getPopulationCount() const;
    
    /**
     * @brief Gets all populations in the region
     * @return const std::vector<std::unique_ptr<NeuralPopulation>>& Reference to internal populations
     * @note Returns a reference to the internal vector of populations. For read-only
     *       access, const version is preferred.
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    /**
     * @brief Gets all population pointers (non-const version)
     * @return std::vector<NeuralPopulation*> Vector of population pointers
     * @details Provides access to population pointers for iteration and modification.
     *          This is useful when direct manipulation of populations is needed.
     */
    std::vector<NeuralPopulation*> getAllPopulations();
    
    /**
     * @brief Creates a synapse between two neurons in this region
     * @param source Neuron ID of the presynaptic neuron
     * @param destination Neuron ID of the postsynaptic neuron
     * @param weight Synaptic weight (default: 0.0)
     * @param delay Synaptic delay in steps (default: 1)
     * @return SynapseId ID of the newly created synapse
     * @details Creates a new synapse connecting two neurons within this region.
     *          The synapse is added to both incoming and outgoing connectivity maps
     *          for efficient access during spike processing.
     * @warning Neurons must exist in the region before creating a synapse.
     * @warning Source and destination neurons must be distinct.
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Accesses a synapse by ID
     * @param id Synapse ID
     * @return Synapse* Pointer to the synapse, or nullptr if not found
     */
    Synapse* getSynapse(SynapseId id);
    
    /**
     * @brief Accesses a synapse by ID (const version)
     * @param id Synapse ID
     * @return const Synapse* Const pointer to the synapse, or nullptr if not found
     */
    const Synapse* getSynapse(SynapseId id) const;
    
    /**
     * @brief Gets the number of synapses in the region
     * @return size_t Number of synapses
     */
    size_t getSynapseCount() const;
    
    /**
     * @brief Gets all synapses in the region
     * @return const std::vector<std::unique_ptr<Synapse>>& Reference to internal synapses
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    /**
     * @brief Gets all synapses originating from a specific neuron
     * @param neuron Neuron ID to find outgoing synapses for
     * @return std::vector<Synapse*> Vector of synapse pointers
     * @details Retrieves all synapses where the specified neuron is the source.
     *          Useful for spike processing and synaptic output management.
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    /**
     * @brief Gets all synapses terminating at a specific neuron
     * @param neuron Neuron ID to find incoming synapses for
     * @return std::vector<Synapse*> Vector of synapse pointers
     * @details Retrieves all synapses where the specified neuron is the destination.
     *          Used for synaptic input processing and spike handling.
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    /**
     * @brief Gets the total number of neurons in the region
     * @return size_t Total neuron count across all populations
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Gets the number of active neurons in the region
     * @return size_t Number of neurons currently firing or active
     * @details Active neurons are those that are currently firing (spiking)
     *          or in the active state but not yet refractory.
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Gets the number of firing neurons in the region
     * @return size_t Number of neurons in firing state
     * @details Firing neurons are those that have crossed threshold and are
     *          currently in the firing state.
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Calculates the average firing rate across all populations
     * @return float Average firing rate (Hz)
     * @details Computes the mean firing rate of all populations in the region,
     *          not a global average of individual neuron firing rates.
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Calculates the average synaptic weight in the region
     * @return float Average synaptic weight
     * @details Computes the mean weight of all synapses in the region.
     *          Useful for monitoring network excitability and stability.
     */
    float getAverageSynapticWeight() const;
    
    /**
     * @brief Calculates the synaptic density in the region
     * @return float Synaptic density (fraction of possible connections)
     * @details Computes the ratio of actual synapses to maximum possible connections.
     *          Maximum connections = N * (N-1) for a directed graph where N is
     *          the total number of neurons in the region.
     * @warning Returns 0.0 if region has 0 or 1 neurons (no possible connections).
     */
    float getSynapticDensity() const;
    
    /**
     * @brief Steps all neurons and synapses in the region
     * @param currentTime The current simulation time (seconds)
     * @details Updates all neurons and synapses in the region for one simulation step.
     *          This includes:
     *          - Processing neuron LIF dynamics
     *          - Handling spike events
     *          - Updating synaptic transmission
     *          - Managing short-term plasticity
     * @note This is called by SimulationStepCoordinator::executeStep2() and
     *       executeStep7().
     */
    void step(Timestamp currentTime);
    
    /**
     * @brief Resets all neurons and synapses to initial state
     * @details Resets all neurons in all populations to their initial states:
     *          - Membrane potential to resting potential
     *          - Firing state to resting
     *          - Spike history cleared
     *          - Refractory counters reset
     *          Similarly resets all synapses to initial state.
     * @note This preserves the region's structure (populations, neurons, synapses)
     *       but clears their dynamic state. Useful for episode termination.
     */
    void reset();
    
    /**
     * @brief Initializes random connectivity patterns
     * @param rng Reference to random number generator
     * @param connectionProbability Probability of creating a synapse between neurons
     * @param meanWeight Mean synaptic weight for new synapses
     * @param weightVariance Variance of synaptic weights for new synapses
     * @details Creates random synaptic connections based on neuron types:
     *          - Excitatory neurons connect to all neuron types (excitatory synapses)
     *          - Inhibitory neurons connect to all neuron types (inhibitory synapses)
     *          - Modulatory neurons do not create direct connections
     *          - Self-connections are avoided
     * @note This method implements Phase 1 connectivity initialization.
     *       More sophisticated connectivity patterns will be added in Phase 2.
     */
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                       float connectionProbability,
                                       float meanWeight,
                                       float weightVariance);
    
    /**
     * @brief Gets all neurons across all populations
     * @return std::vector<Neuron*> Vector of neuron pointers
     * @details Collects all neurons from all populations in the region.
     *          Useful for region-wide operations and statistics.
     */
    std::vector<Neuron*> getAllNeurons();
    
    /**
     * @brief Gets all neurons across all populations (const version)
     * @return std::vector<const Neuron*> Vector of const neuron pointers
     * @details Const version for read-only access to neurons.
     */
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
