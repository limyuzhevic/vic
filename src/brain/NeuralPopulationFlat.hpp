#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

// Optimized NeuralPopulation with flat array storage
class NeuralPopulation {
public:
    // Create population with ID and size
    NeuralPopulation(PopulationId id, size_t size);
    
    ~NeuralPopulation();
    
    // Disable copying, enable moving
    NeuralPopulation(const NeuralPopulation&) = delete;
    NeuralPopulation& operator=(const NeuralPopulation&) = delete;
    NeuralPopulation(NeuralPopulation&&) noexcept;
    NeuralPopulation& operator=(NeuralPopulation&&) noexcept;
    
    // Identity
    PopulationId getId() const;
    
    // Size
    size_t getSize() const;
    bool isEmpty() const;
    
    // Neuron access - optimized for cache locality
    Neuron* getNeuron(NeuronIndex index);
    const Neuron* getNeuron(NeuronIndex index) const;
    
    // Add neuron to population - bulk initialization supported
    void addNeuron(Neuron* neuron);
    void addNeuronsBulk(const std::vector<Neuron*>& neurons);
    
    // Get all neurons - returns direct pointer to flat array for efficiency
    const Neuron* getNeurons() const { return m_neurons.data(); }
    Neuron** getNeuronsNonConst() { return m_neurons.data(); }
    size_t getNeuronCount() const { return m_neurons.size(); }
    
    // Population type
    NeuronType getNeuronType() const;
    void setNeuronType(NeuronType type);
    
    // Statistics
    size_t getActiveCount() const;
    size_t getFiringCount() const;
    float getAverageFiringRate() const;
    float getActivityLevel() const;
    
    // Initialize all neurons randomly - optimized
    void initializeRandom(class RandomGenerator& rng);
    
    // Step all neurons - can be vectorized
    void step(Timestamp currentTime);
    
    // Reset all neurons
    void reset();
    
    // Get neuron IDs
    std::vector<NeuronId> getNeuronIds() const;
    
    // Statistics tracking
    void recordActivity();
    float getMeanMembranePotential() const;
    float getVarianceMembranePotential() const;
    
    // Get memory address for cache-friendly access
    size_t getNeuronIndex(NeuronId id) const;
    
    // Bulk operations for performance
    void applyCurrentToAll(MembranePotential current);
    void injectCurrentToFiring(float current);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Optimized NeuralRegion with flat array storage
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
    
    // Population management - optimized bulk operations
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    void addPopulationsBulk(const std::vector<std::pair<size_t, NeuronType>>& populations);
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    size_t getPopulationCount() const;
    
    // Direct access to populations for cache efficiency
    const NeuralPopulation* getPopulations() const;
    size_t getTotalPopulationCount() const;
    
    // Synapse management
    SynapseId addSynapse(NeuronId source, NeuronId destination, 
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    Synapse* getSynapse(SynapseId id);
    const Synapse* getSynapse(SynapseId id) const;
    size_t getSynapseCount() const;
    
    // Optimized connectivity queries using flat arrays
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Local connectivity statistics
    float getAverageSynapticWeight() const;
    float getSynapticDensity() const;
    
    // Step all populations and synapses - optimized
    void step(Timestamp currentTime);
    
    // Reset all neurons and synapses
    void reset();
    
    // Initialize connectivity - optimized for cache locality
    void initializeRandomConnectivity(class RandomGenerator& rng, 
                                       float connectionProbability,
                                       float meanWeight,
                                       float weightVariance);
    
    // Get all neurons across all populations - optimized version
    std::vector<Neuron*> getAllNeurons() const;
    
    // For performance: direct access to neuron arrays
    const Neuron* getAllNeuronsBuffer() const;
    
    // Statistics tracking
    size_t getActivePopulationCount() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
