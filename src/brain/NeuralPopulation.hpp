#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <memory>
#include <stdexcept>

namespace nlm {

// NeuralPopulation: A group of neurons with shared properties
// Provides memory-safe access to neurons with bounds checking and error handling
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
    
    // Neuron access with bounds checking
    Neuron* getNeuron(NeuronIndex index);
    const Neuron* getNeuron(NeuronIndex index) const;
    
    // Add neuron to population (takes ownership)
    void addNeuron(std::unique_ptr<Neuron> neuron);
    
    // Get all neurons - safe view of neuron pointers
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
    void initializeRandom(class RandomGenerator& rng);
    
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
    
    // Safety and validation methods
    bool isValidNeuronIndex(NeuronIndex index) const;
    size_t getSafeNeuronCount() const;  // Returns actual count, not just size
    
    // Statistics methods
    float getWeightedActivityLevel() const;  // Weighted by membrane potential
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm