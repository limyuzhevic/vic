#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <memory>

namespace nlm {

// NeuralPopulation: A group of neurons with shared properties
// OPTIMIZED VERSION: Phase 2 with caching and performance improvements

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
    
    // Neuron access
    Neuron* getNeuron(NeuronIndex index);
    const Neuron* getNeuron(NeuronIndex index) const;
    
    // Add neuron to population (caller retains ownership)
    void addNeuron(Neuron* neuron);
    
    // Create `size` neurons owned by this population, with sequential IDs
    // starting at startId. This is what actually materialises a population:
    // before this fix a population only stored a size counter and no neurons.
    void createNeurons(NeuronId startId);
    
    // Get all neurons
    const std::vector<Neuron*>& getNeurons() const;
    
    // Population type (all neurons in population share same type)
    NeuronType getNeuronType() const;
    void setNeuronType(NeuronType type);
    
    // Statistics (optimized with caching)
    size_t getActiveCount() const;
    size_t getFiringCount() const;
    float getAverageFiringRate() const;
    float getActivityLevel() const;  // fraction of active neurons
    
    // Initialize all neurons randomly
    void initializeRandom(class RandomGenerator& rng);
    
    // Step all neurons (optimized)
    void step(Timestamp currentTime);
    
    // Reset all neurons
    void reset();
    
    // Get neuron IDs in this population
    std::vector<NeuronId> getNeuronIds() const;
    
    // Statistics tracking
    void recordActivity();  // Record current state for statistics
    float getMeanMembranePotential() const;
    float getVarianceMembranePotential() const;
    
    // Optimized batch operations for better performance
    void batchStep(Timestamp currentTime, float timestep);
    void batchInitializeRandom(class RandomGenerator& rng);
    
    // Cache management
    void updateCachedStats() const;
    void invalidateCachedStats();
    bool isCacheDirty() const;
    
    // Enhanced connectivity and spike delivery
    void addOutputConnection(class NeuralPopulation* target, float weight);
    void deliverSynapticInputBatch(const std::vector<SynapticInput>& inputs);
    void processSpikesBatch(const std::vector<SpikeEvent>& spikeEvents);
    void processSpikesBatch(const std::vector<SpikeEvent>& spikeEvents);
    void deliverSpikeToTargets(const SpikeEvent& spikeEvent);
    
    // Neuron filtering and access patterns
    std::vector<Neuron*> getNeuronsByType(NeuronType type) const;
    std::vector<Neuron*> getFiringNeurons() const;
    
    // Condition checks
    bool hasActiveNeurons() const;
    float getTotalExcitatoryInput() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
