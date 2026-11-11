#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"
#include <memory>

namespace nlm {

class Brain;

// Structural plasticity: creation and removal of synapses and neurons
// REAL IMPLEMENTATION - Phase 2 real structural plasticity
class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse
    // REAL IMPLEMENTATION: Implement real synapse creation
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse
    // REAL IMPLEMENTATION: Implement real synapse removal
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron
    // REAL IMPLEMENTATION: Implement real neuron creation
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron
    // REAL IMPLEMENTATION: Implement real neuron removal
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
    // Get creation rate
    float getSynaptogenesisRate() const;
    void setSynaptogenesisRate(float rate);
    
    // Get pruning rate
    float getPruningRate() const;
    void setPruningRate(float rate);
    
    // Update structural plasticity
    void update(Brain* brain, RandomGenerator& rng);
    
    // Get structural statistics
    float getCurrentSynapticDensity() const;
    float getCurrentNeuronDensity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
