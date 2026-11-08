#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

// Structural plasticity: creation and removal of synapses and neurons
    // Phase 6: Implements real structural plasticity with synaptogenesis and pruning
    
    class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse
    // Phase 6: Real synapse creation with structural rules and validation
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse
    // Phase 6: Real synapse removal with structural rules and validation
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron
    // Phase 6: Real neuron creation with structural rules and validation
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron
    // Phase 6: Real neuron removal with structural rules and validation
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
