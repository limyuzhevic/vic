#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"

namespace nlm {

// Structural plasticity: creation and removal of synapses and neurons
class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
    // Get creation rate
    float getSynaptogenesisRate() const;
    void setSynaptogenesisRate(float rate);
    
    // Get pruning rate
    float getPruningRate() const;
    void setPruningRate(float rate);
    
    // Get weight threshold for pruning
    float getWeightThreshold() const;
    void setWeightThreshold(float threshold);
    
    // Get activity threshold for pruning
    float getActivityThreshold() const;
    void setActivityThreshold(float threshold);
    
    // Update structural plasticity
    void update(Brain* brain, RandomGenerator& rng);
    
    // Get synapse density
    float getSynapseDensity(Brain* brain) const;
    
    // Get statistics
    size_t getTotalSynapsesCreated() const;
    size_t getTotalSynapsesPruned() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
