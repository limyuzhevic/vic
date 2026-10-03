#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Structural plasticity: creation and removal of synapses and neurons
// PLACEHOLDER - Phase 6 will implement developmental stage-based structural changes

class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse
    // TODO PHASE 6: Implement synaptogenesis with developmental regulation
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse
    // TODO PHASE 6: Implement activity-dependent pruning
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron
    // TODO PHASE 6: Implement neurogenesis with developmental timing
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron
    // TODO PHASE 6: Implement activity-dependent neuron elimination
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
