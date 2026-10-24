#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Structural plasticity: creation and removal of synapses and neurons
// PLACEHOLDER - Phase 2 will implement real structural plasticity

class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse
    // Phase 2 completed - Real synapse creation implemented in StructuralPlasticity.cpp
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse
    // Phase 2 completed - Real synapse removal implemented in StructuralPlasticity.cpp
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron
    // Phase 2 completed - Real neuron creation implemented in StructuralPlasticity.cpp
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron
    // Phase 2 completed - Real neuron removal implemented in StructuralPlasticity.cpp
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
