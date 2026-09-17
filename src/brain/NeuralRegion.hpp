#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class NeuralPopulation;
class Synapse;
class RandomGenerator;

// NeuralRegion - A collection of neural populations
// Represents a region of the brain with organized neural tissue
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
    
    // Name and metadata
    const std::string& getName() const;
    void setName(const std::string& name);
    
    // Population management
    PopulationId addPopulation(size_t size, NeuronType type);
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    size_t getPopulationCount() const;
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    
    // Get all populations as pointers (convenience)
    std::vector<NeuralPopulation*> getAllPopulations();
    
    // Synapse management
    SynapseId addSynapse(NeuronId source, NeuronId destination,
                        SynapticWeight weight, Delay delay);
    Synapse* getSynapse(SynapseId id);
    const Synapse* getSynapse(SynapseId id) const;
    size_t getSynapseCount() const;
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;
    
    // Synapse lookup
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    // Neuron access
    std::vector<Neuron*> getAllNeurons();
    std::vector<const Neuron*> getAllNeurons() const;
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getAverageSynapticWeight() const;
    float getSynapticDensity() const;
    
    // Simulation
    void step(Timestamp currentTime);
    void reset();
    
    // Network initialization
    void initializeRandomConnectivity(RandomGenerator& rng,
                                     float connectionProbability,
                                     float meanWeight,
                                     float weightVariance);
    
    // Integration state
    void setIntegrationWeight(float weight);
    float getIntegrationWeight() const;
    
    // Recording and analysis
    void recordActivity();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
