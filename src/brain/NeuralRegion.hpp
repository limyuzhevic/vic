#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralPopulation.hpp"
#include "Synapse.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// NeuralRegion: A brain region containing multiple populations and local connectivity
// Phase 6 implementation: Fully functional region with integrated memory and processing

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
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    class NeuralPopulation* getPopulation(PopulationId id);
    const class NeuralPopulation* getPopulation(PopulationId id) const;
    size_t getPopulationCount() const;
    const std::vector<std::unique_ptr<class NeuralPopulation>>& getPopulations() const;
    std::vector<class NeuralPopulation*> getAllPopulations();
    
    // Synapse management (local connections within this region)
    class SynapseId addSynapse(class NeuronId source, class NeuronId destination, 
                             SynapticWeight weight = 0.0f, Delay delay = 1);
    class Synapse* getSynapse(SynapseId id);
    const class Synapse* getSynapse(SynapseId id) const;
    size_t getSynapseCount() const;
    const std::vector<std::unique_ptr<class Synapse>>& getSynapses() const;
    
    // Connectivity queries
    std::vector<class Synapse*> getSynapsesFrom(class NeuronId neuron);
    std::vector<class Synapse*> getSynapsesTo(class NeuronId neuron);
    
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
    std::vector<class Neuron*> getAllNeurons();
    std::vector<const class Neuron*> getAllNeurons() const;
    
    // Integration with brain systems
    void setBrainReference(class Brain* brain) { brainRef_ = brain; }
    
private:
    struct Impl;
    Impl* pImpl;
    class Brain* brainRef_;
};

} // namespace nlm
