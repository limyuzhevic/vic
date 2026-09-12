#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Structural plasticity: creation and removal of synapses and neurons
// PHASE 2: Real structural plasticity implementation

class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse between neurons
    // Implements Hebbian and activity-dependent synaptogenesis
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse from brain
    // Implements synaptic pruning based on weight and activity
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron in appropriate population
    // Implements neurogenesis based on developmental stage
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron and all its synapses
    // Implements neuronal death during development
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
    // Get current synaptogenesis rate
    float getSynaptogenesisRate() const;
    
    // Set synaptogenesis rate (probability of new synapse formation)
    void setSynaptogenesisRate(float rate);
    
    // Get current pruning rate
    float getPruningRate() const;
    
    // Set pruning rate (probability of synapse removal)
    void setPruningRate(float rate);
    
    // Update structural plasticity for one simulation step
    // Implements activity-dependent synaptic formation and elimination
    void update(Brain* brain, class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
