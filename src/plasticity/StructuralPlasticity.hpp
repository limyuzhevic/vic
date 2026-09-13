#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Structural plasticity: creation and removal of synapses and neurons
// PHASE 2: Implemented real structural plasticity with biological mechanisms

class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse
    // PHASE 2: Real synapse creation with biological constraints
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse
    // PHASE 2: Real synapse removal based on pruning rules
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron
    // PHASE 2: Real neuron creation with developmental stages
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron
    // PHASE 2: Real neuron removal with developmental considerations
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
