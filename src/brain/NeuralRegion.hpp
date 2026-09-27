#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

namespace nlm {

/**
 * @file NeuralRegion.hpp
 * @brief Header file for the NeuralRegion class implementing brain regions
 * @author NLM Framework
 * @date 2024
 * @version 1.0.0
 *
 * This file contains the NeuralRegion class definition, which represents
 * functional brain regions containing multiple neural populations and
 * local connectivity patterns. Regions are the highest-level organizational
 * units that implement localized processing and specialization.
 */

/**
 * @class NeuralRegion
 * @brief A brain region containing multiple populations and local connectivity
 * @since Version 1.0 (2024)
 * @version 1.0.0
 *
 * The NeuralRegion class represents functional brain regions that organize
 * neural processing into specialized domains. Each region contains multiple
 * neural populations, implements local connectivity patterns, and manages
 * region-specific dynamics and computations.
 *
 * Key features:
 * - Hierarchical organization of neurons into functional groups
 * - Local connectivity patterns within and between populations
 * - Region-level statistics and metrics
 * - Random connectivity initialization
 * - Integration with global brain dynamics
 *
 * @note Currently implemented as a placeholder for Phase 2 implementation.
 * Future enhancements will include region-specific plasticity rules,
 * lateral inhibition, and competitive dynamics.
 *
 * @warning Region implementation details should be finalized before
 * use in production systems. Current implementation is incomplete.
 *
 * @see Brain, NeuralPopulation, Synapse, NeuralWorkingMemory
 */
class NeuralRegion {
public:
    // Create region with ID
    /**
     * @brief Constructor for the NeuralRegion class
     * @param id Unique identifier for this brain region
     * @post Region is created with empty state
     * @note Region ID should be unique within the brain
     * @warning ID collisions may cause issues in region management
     * @see getId(), addRegion()
     */
    explicit NeuralRegion(RegionId id);
    
    // Create region with ID and name
    /**
     * @brief Constructor for the NeuralRegion class with name
     * @param id Unique identifier for this brain region
     * @param name Descriptive name for the region
     * @post Region is created with specified name
     * @note Names provide human-readable identification
     * @see getName(), setName()
     */
    NeuralRegion(RegionId id, const std::string& name);
    
    ~NeuralRegion();
    
    // Disable copying, enable moving
    /**
     * @brief Copy constructor (deleted)
     * @warning Regions cannot be copied due to pointer-based internal representation
     * @note Memory management handled through pImpl pattern
     */
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    
    /**
     * @brief Move constructor
     * @param other NeuralRegion object to move from
     * @post This region owns other's resources; other is in valid but unspecified state
     * @note Enables efficient transfer of region ownership
     * @see operator=(NeuralRegion&&)
     */
    NeuralRegion(NeuralRegion&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other NeuralRegion object to move from
     * @return Reference to this region
     * @post Current region's resources are transferred from other
     * @note Enables efficient transfer of region ownership
     */
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    // Identity
    /**
     * @brief Get the unique identifier of this region
     * @return RegionId Unique identifier
     * @post Identifier remains constant throughout region's lifetime
     * @note Used for indexing and referencing in region management
     */
    RegionId getId() const;
    
    /**
     * @brief Get the region's name
     * @return const std::string& Reference to region name
     * @post Name provides human-readable identification
     * @note Default name is empty string if not set
     */
    const std::string& getName() const;
    
    /**
     * @brief Set the region's name
     * @param name New descriptive name for the region
     * @post Region name is updated
     * @note Useful for debugging and visualization
     */
    void setName(const std::string& name);
    
    // Population management
    /**
     * @brief Add a neural population to this region
     * @param size Number of neurons in the population
     * @param type Neuron type for the population (default: Internal)
     * @return PopulationId Unique identifier for the created population
     * @post Population is created and added to region
     * @note Populations organize neurons by type and function
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    /**
     * @brief Get pointer to population by ID
     * @param id Population identifier
     * @return NeuralPopulation* Pointer to population, or nullptr if not found
     * @post Provides access to population for manipulation and statistics
     */
    NeuralPopulation* getPopulation(PopulationId id);
    
    /**
     * @brief Get constant pointer to population by ID
     * @param id Population identifier
     * @return const NeuralPopulation* Constant pointer to population
     * @post Provides read-only access to population for statistics
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * @brief Get number of populations in this region
     * @return size_t Count of populations
     * @post Returns total number of neural populations
     * @note Each population contains multiple neurons
     */
    size_t getPopulationCount() const;
    
    /**
     * @brief Get constant reference to all populations
     * @return const std::vector<std::unique_ptr<NeuralPopulation>>& Vector of population pointers
     * @post Provides read-only access to all populations
     * @note Populations are stored as unique_ptr for proper memory management
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    /**
     * @brief Get mutable pointers to all populations
     * @return std::vector<NeuralPopulation*> Vector of population pointers
     * @post Provides read-write access to all populations
     * @note Useful for batch operations and modifications
     */
    std::vector<NeuralPopulation*> getAllPopulations();
    
    // Synapse management (local connections within this region)
    /**
     * @brief Add a synapse between neurons within this region
     * @param source Source neuron identifier
     * @param destination Target neuron identifier
     * @param weight Initial synaptic weight (default: 0.0f)
     * @param delay Transmission delay in steps (default: 1)
     * @return SynapseId Unique identifier for the created synapse
     * @post Synapse is created and added to region connectivity
     * @note Local connections implement regional communication patterns
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Get pointer to synapse by ID
     * @param id Synapse identifier
     * @return Synapse* Pointer to synapse, or nullptr if not found
     * @post Provides access to synapse for manipulation
     */
    Synapse* getSynapse(SynapseId id);
    
    /**
     * @brief Get constant pointer to synapse by ID
     * @param id Synapse identifier
     * @return const Synapse* Constant pointer to synapse
     * @post Provides read-only access to synapse
     */
    const Synapse* getSynapse(SynapseId id) const;
    
    /**
     * @brief Get number of synapses in this region
     * @return size_t Count of synapses
     * @post Returns total number of local connections
     */
    size_t getSynapseCount() const;
    
    /**
     * @brief Get constant reference to all synapses
     * @return const std::vector<std::unique_ptr<Synapse>>& Vector of synapse pointers
     * @post Provides read-only access to all synapses
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    // Connectivity queries
    /**
     * @brief Get all synapses originating from a neuron
     * @param neuron Neuron identifier
     * @return std::vector<Synapse*> Vector of synapse pointers
     * @post Returns all outgoing connections from specified neuron
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    /**
     * @brief Get all synapses terminating at a neuron
     * @param neuron Neuron identifier
     * @return std::vector<Synapse*> Vector of synapse pointers
     * @post Returns all incoming connections to specified neuron
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    // Statistics
    /**
     * @brief Get total number of neurons in this region
     * @return size_t Total neuron count across all populations
     * @post Provides system-wide scale information
     * @note Sum of neurons in all populations
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get number of active neurons in this region
     * @return size_t Count of active neurons
     * @post Indicates current regional activation level
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of firing neurons in this region
     * @return size_t Count of neurons that are firing
     * @post More specific than getActiveNeuronCount()
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons in this region
     * @return float Average firing rate (Hz)
     * @post Returns regional statistical measure
     */
    float getAverageFiringRate() const;
    
    // Local connectivity statistics
    /**
     * @brief Get average synaptic weight in this region
     * @return float Average synaptic weight
     * @post Provides measure of regional synaptic strength
     */
    float getAverageSynapticWeight() const;
    
    /**
     * @brief Get synaptic density of this region
     * @return float Fraction of possible connections that are established (0.0 to 1.0)
     * @post Returns measure of regional connectivity
     */
    float getSynapticDensity() const;  // fraction of possible connections
    
    // Step all populations and synapses
    /**
     * @brief Update all populations and synapses in this region
     * @param currentTime Current simulation time
     * @post Regional state is updated according to neural dynamics
     * @note Called by Brain::step() for region-specific processing
     */
    void step(Timestamp currentTime);
    
    // Reset all neurons and synapses
    /**
     * @brief Reset all neurons and synapses in this region
     * @post All neural state is reset to initial values
     * @note Clears firing states, weights, and history
     */
    void reset();
    
    // Initialize connectivity
    /**
     * @brief Initialize random connectivity patterns within this region
     * @param rng Reference to random number generator
     * @param connectionProbability Probability that each possible connection exists
     * @param meanWeight Mean value for synaptic weights
     * @param weightVariance Variance for synaptic weights
     * @post Random connectivity is established between neurons
     * @note Creates stochastic patterns of connections
     */
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                       float connectionProbability,
                                       float meanWeight,
                                       float weightVariance);
    
    // Get all neurons across all populations
    /**
     * @brief Get pointers to all neurons in this region
     * @return std::vector<Neuron*> Vector of neuron pointers
     * @post Provides access to all neurons for batch operations
     */
    std::vector<Neuron*> getAllNeurons();
    
    /**
     * @brief Get constant pointers to all neurons in this region
     * @return std::vector<const Neuron*> Vector of constant neuron pointers
     * @post Provides read-only access to all neurons
     */
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm