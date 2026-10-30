#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Structural plasticity: creation and removal of synapses and neurons
// IMPLEMENTED: Basic structural plasticity frameworks with configurable rates (Phase 6)

class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse
    // IMPLEMENTED: Real synapse creation with structural rules (Phase 6)
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse
    // IMPLEMENTED: Real synapse removal with pruning rules (Phase 6)
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron
    // IMPLEMENTED: Real neuron creation with development rules (Phase 6)
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron
    // IMPLEMENTED: Real neuron removal with developmental constraints (Phase 6)
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
