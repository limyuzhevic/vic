#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Structural plasticity: creation and removal of synapses and neurons
// Implements biological mechanisms for synaptic growth and pruning

class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse - marks for removal by zeroing weight and schedules cleanup
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron - implements neurogenesis mechanisms
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron - removes all associated synapses and the neuron itself
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
    // Get creation rate
    float getSynaptogenesisRate() const;
    void setSynaptogenesisRate(float rate);
    
    // Get pruning rate
    float getPruningRate() const;
    void setPruningRate(float rate);
    
    // Update structural plasticity - implements activity-dependent growth and pruning
    void update(Brain* brain, class RandomGenerator& rng);
    
    // Statistics and monitoring
    size_t getTotalSynapsesCreated() const;
    size_t getTotalSynapsesPruned() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
