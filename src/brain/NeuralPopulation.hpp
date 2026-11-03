#pragma once

#include "../core/Types/Types.hpp"
#include "Neuron.hpp"
#include <vector>
#include <memory>

namespace nlm {

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
    
    // Step all neurons with simulation timestep
    void updateNeurons(Timestamp currentTime, Timestamp timestep);
    
    // Detect spikes and manage spike events
    void detectSpikes(SimulationStep currentStep, Timestamp currentTime, 
                     Timestamp timestep, class SpikeSystem* spikeSystem);
    
    // Apply neuromodulation effects to all neurons
    void applyDopamineEffects(float level, Timestamp timestep);
    
    // Apply plasticity to all synapses in this population
    void applyPlasticity(class STDP* stdp, class Hebbian* hebbian,
                        float plasticityMod, Timestamp currentTime,
                        Timestamp timestep);
    
    // Get all synapses in this population
    const std::vector<std::unique_ptr<class Synapse>>& getSynapses() const;
    
    // Get all neurons by type
    std::vector<Neuron*> getNeuronsByType(NeuronType type);
    
    // Get neurons that have a specific property
    std::vector<Neuron*> findNeurons(std::function<bool(const Neuron*)> predicate);
    
    // Clear all neurons
    void clear();
    
    // Apply current injection to specific neurons
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Reset all neurons to initial state
    void resetAll();
