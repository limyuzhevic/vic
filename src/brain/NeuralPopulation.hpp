#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations to break circular dependencies
class Neuron;
class RandomGenerator;

// NeuralPopulation: A group of neurons with shared properties
// PLACEHOLDER - Phase 2 will implement population-level dynamics

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
    
    // Add neuron to population
    void addNeuron(Neuron* neuron);
    
    // Get all neurons
    const std::vector<Neuron*>& getNeurons() const;
    
    // Population type (all neurons in population share same type)
    NeuronType getNeuronType() const;
    void setNeuronType(NeuronType type);
    
    // Statistics
    size_t getActiveCount() const;
    size_t getFiringCount() const;
    float getAverageFiringRate() const;
    float getActivityLevel() const;  // fraction of active neurons
    
    // Initialize all neurons randomly
    void initializeRandom(RandomGenerator& rng);
    
    // Step all neurons
    void step(Timestamp currentTime);
    
    // Reset all neurons
    void reset();
    
    // Get neuron IDs in this population
    std::vector<NeuronId> getNeuronIds() const;
    
    // Statistics tracking
    void recordActivity();  // Record current state for statistics
    float getMeanMembranePotential() const;
    float getVarianceMembranePotential() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
