#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

// Structural plasticity: creation and removal of synapses and neurons
// Phase 6: Real structural plasticity for network development
// Implements synaptogenesis and pruning based on developmental stage

class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse
    // Phase 6: Real synapse creation with biological constraints
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse
    // Phase 6: Real synapse removal based on developmental program
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron
    // Phase 6: Real neuron creation during development
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron
    // Phase 6: Real neuron removal during development
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
    // Get creation rate
    float getSynaptogenesisRate() const;
    void setSynaptogenesisRate(float rate);
    
    // Get pruning rate
    float getPruningRate() const;
    void setPruningRate(float rate);
    
    // Update structural plasticity
    void update(Brain* brain, class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
