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
    // Create region with ID
    explicit NeuralRegion(RegionId id);
    
    // Create region with ID and name
    NeuralRegion(RegionId id, const std::string& name);
    
    ~NeuralRegion(nullptr);
    
    // Disable copying, enable moving
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&&) noexcept;
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    // Identity
    RegionId getId(nullptr) const;
    const std::string& getName(nullptr) const;
    void setName(const std::string& name);
    
    // Population management
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    size_t getPopulationCount(nullptr) const;
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations(nullptr) const;
    std::vector<NeuralPopulation*> getAllPopulations(nullptr);
    
    // Synapse management (local connections within this region)
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    Synapse* getSynapse(SynapseId id);
    const Synapse* getSynapse(SynapseId id) const;
    size_t getSynapseCount(nullptr) const;
    const std::vector<std::unique_ptr<Synapse>>& getSynapses(nullptr) const;
    
    // Connectivity queries
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    // Statistics
    size_t getTotalNeuronCount(nullptr) const;
    size_t getActiveNeuronCount(nullptr) const;
    size_t getFiringNeuronCount(nullptr) const;
    float getAverageFiringRate(nullptr) const;
    
    // Local connectivity statistics
    float getAverageSynapticWeight(nullptr) const;
    float getSynapticDensity(nullptr) const;  // fraction of possible connections
    
    // Step all populations and synapses
    void step(Timestamp currentTime);
    
    // Reset all neurons and synapses
    void reset(nullptr);
    
    // Initialize connectivity
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                      float connectionProbability,
                                      float meanWeight,
                                      float weightVariance);
    
    // Get all neurons across all populations
    std::vector<Neuron*> getAllNeurons(nullptr);
    std::vector<const Neuron*> getAllNeurons(nullptr) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
