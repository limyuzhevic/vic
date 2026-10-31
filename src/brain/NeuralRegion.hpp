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
    
    ~NeuralRegion();
    
    // Disable copying, enable moving
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&&) noexcept;
    NeuralRegion& operator=(NeuralRegion&&) noexcept;
    
    // Identity
    RegionId getId() const;
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Population management
    // Set the next neuron ID to allocate. Brain uses this so neuron IDs are
    // globally unique across regions; without it every region started at 1
    // and Brain's neuron-by-ID spike-delivery map sent region 1's spikes
    // to region 2's neurons with the same ID, leaving region 1 silent.
    void setNextNeuronIndex(uint64_t next);
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    size_t getPopulationCount() const;
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
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
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Local connectivity statistics
    float getAverageSynapticWeight() const;
    float getSynapticDensity() const;  // fraction of possible connections
    
    // Step all populations and synapses
    void step(Timestamp currentTime);
    
    // Reset all neurons and synapses
    void reset();
    
    // Initialize connectivity
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                      float connectionProbability,
                                      float meanWeight,
                                      float weightVariance);
    
    // Get all neurons across all populations
    std::vector<Neuron*> getAllNeurons();
    std::vector<const Neuron*> getAllNeurons() const;
    
private:
    // Simplified implementation - no PIMPL
    RegionId id_;
    std::string name_;
    std::vector<std::unique_ptr<NeuralPopulation>> populations_;
    std::vector<std::unique_ptr<Synapse>> synapses_;
    std::unordered_map<NeuronId, std::vector<SynapseId>> outgoingSynapses_;
    std::unordered_map<NeuronId, std::vector<SynapseId>> incomingSynapses_;
    SynapseId nextSynapseId_;
    uint64_t nextNeuronIndex_;
    std::unordered_map<uint64_t, Synapse*> synapseById_;
};

} // namespace nlm
