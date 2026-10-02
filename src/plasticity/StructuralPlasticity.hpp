#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Structural plasticity: creation and removal of synapses and neurons
// Phase 6: Simplified structural plasticity implementation
// Current implementation: Basic creation/removal with configurable rates
// TODO FUTURE PHASE: Implement real structural plasticity with biological mechanisms

class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse
    // Phase 6: Simplified synapse creation
    // TODO FUTURE PHASE: Implement real synapse creation with molecular signaling
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse
    // Phase 6: Simplified synapse removal
    // TODO FUTURE PHASE: Implement real synapse removal with activity-dependent mechanisms
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron
    // Phase 6: Simplified neuron creation
    // TODO FUTURE PHASE: Implement real neuron creation with developmental regulation
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron
    // Phase 6: Simplified neuron removal
    // TODO FUTURE PHASE: Implement real neuron removal with competitive mechanisms
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
