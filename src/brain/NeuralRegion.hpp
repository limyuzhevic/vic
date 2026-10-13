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

class NeuralRegion {
public:
    /**
     * @brief Create neural region with ID
     * @param id Unique identifier for the region
     */
    explicit NeuralRegion(RegionId id);
    
    /**
     * @brief Create neural region with ID and name
     * @param id Unique identifier for the region
     * @param name Human-readable name for the region
     */
    NeuralRegion(RegionId id, const std::string& name);
    
    /**
     * @brief Destructor
     */
    ~NeuralRegion();
    
    /**
     * @brief Copy constructor deleted (regions are not copyable)
     */
    NeuralRegion(const NeuralRegion&) = delete;
    
    /**
     * @brief Copy assignment operator deleted (regions are not copyable)
     */
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    
    /**
     * @brief Move constructor
     * @param other Region to move from
     */
    NeuralRegion(NeuralRegion&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other Region to move from
     * @return Reference to this region
     */
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    /**
     * @brief Get region identifier
     * @return Region ID
     */
    RegionId getId() const;
    
    /**
     * @brief Get region name
     * @return Human-readable region name
     */
    const std::string& getName() const;
    
    /**
     * @brief Set region name
     * @param name New human-readable name for the region
     */
    void setName(const std::string& name);
    
    /**
     * @brief Add neural population to region
     * @param size Number of neurons in new population
     * @param type Neuron type for new population
     * @return ID of newly created population
     */
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    
    /**
     * @brief Get neural population by ID
     * @param id Population ID to retrieve
     * @return Pointer to population, or nullptr if not found
     */
    NeuralPopulation* getPopulation(PopulationId id);
    
    /**
     * @brief Get neural population by ID (const version)
     * @param id Population ID to retrieve
     * @return Const pointer to population, or nullptr if not found
     */
    const NeuralPopulation* getPopulation(PopulationId id) const;
    
    /**
     * @brief Get number of populations in region
     * @return Count of populations
     */
    size_t getPopulationCount() const;
    
    /**
     * @brief Get all populations in region
     * @return Const reference to vector of population pointers
     */
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    /**
     * @brief Get all populations in region (mutable version)
     * @return Vector of population pointers (non-const)
     */
    std::vector<NeuralPopulation*> getAllPopulations();
    
    /**
     * @brief Add synapse to region
     * @param source Neuron ID where synapse originates
     * @param destination Neuron ID where synapse terminates
     * @param weight Initial synaptic weight
     * @param delay Synaptic delay in simulation steps
     * @return ID of newly created synapse
     */
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Get synapse by ID
     * @param id Synapse ID to retrieve
     * @return Pointer to synapse, or nullptr if not found
     */
    Synapse* getSynapse(SynapseId id);
    
    /**
     * @brief Get synapse by ID (const version)
     * @param id Synapse ID to retrieve
     * @return Const pointer to synapse, or nullptr if not found
     */
    const Synapse* getSynapse(SynapseId id) const;
    
    /**
     * @brief Get number of synapses in region
     * @return Count of synapses
     */
    size_t getSynapseCount() const;
    
    /**
     * @brief Get all synapses in region
     * @return Const reference to vector of synapse pointers
     */
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    /**
     * @brief Get synapses originating from neuron
     * @param neuron Neuron ID
     * @return Vector of outgoing synapse pointers
     */
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    
    /**
     * @brief Get synapses terminating at neuron
     * @param neuron Neuron ID
     * @return Vector of incoming synapse pointers
     */
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    /**
     * @brief Get total neuron count in region
     * @return Total number of neurons across all populations
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get active neuron count
     * @return Number of neurons in Active or Refractory state
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * @return Number of neurons in Refractory state
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * @return Average firing rate across all neurons in Hz
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get average synaptic weight
     * @return Mean synaptic weight across all synapses
     */
    float getAverageSynapticWeight() const;
    
    /**
     * @brief Get synaptic density
     * @return Fraction of possible connections that exist
     */
    float getSynapticDensity() const;  // fraction of possible connections
    
    /**
     * @brief Step all populations and synapses
     * @param currentTime Current simulation time
     * @param dt Timestep duration in milliseconds
     * @details Advances the entire neural region by one simulation timestep:
     *          - Steps all neurons in all populations using their LIF dynamics
     *          - Steps all synapses using their synaptic dynamics
     *          - Called by Brain::step() with the global timestep
     */
    void step(Timestamp currentTime, TimestepDuration dt);
    
    /**
     * @brief Reset all neurons and synapses
     * @details Calls reset() for all populations and synapses in region
     */
    void reset();
    
    /**
     * @brief Initialize random connectivity
     * @param rng Random number generator
     * @param connectionProbability Probability of creating each possible connection
     * @param meanWeight Mean synaptic weight for new synapses
     * @param weightVariance Variance of synaptic weight distribution
     * @details Creates realistic random connectivity based on neuron types:
     *          - Excitatory neurons connect to all neurons (excitatory synapses)
     *          - Inhibitory neurons connect to all neurons (inhibitory synapses)
     *          - Other types have limited connectivity
     */
    void initializeRandomConnectivity(RandomGenerator& rng,
                                       float connectionProbability,
                                       float meanWeight,
                                       float weightVariance);
private:
    /**
     * @brief Private implementation structure for NeuralRegion
     * @details Internal implementation details to reduce header size and improve compilation times
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
