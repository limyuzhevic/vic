#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Structural plasticity: creation and removal of synapses and neurons
// Real implementation based on activity-dependent synaptic turnover

class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse between neurons
    // Real implementation based on probabilistic synaptogenesis
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse from brain
    // Real implementation based on weight-based pruning
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron in specified population
    // Real implementation based on neuronal birth and maturation
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron and all associated synapses
    // Real implementation based on activity-dependent neuronal death
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
    // Get synaptogenesis rate (probability of new synapse formation per step)
    float getSynaptogenesisRate() const;
    void setSynaptogenesisRate(float rate);
    
    // Get pruning rate (probability of synapse removal per step)
    float getPruningRate() const;
    void setPruningRate(float rate);
    
    // Update structural plasticity for all regions
    void update(Brain* brain, class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
