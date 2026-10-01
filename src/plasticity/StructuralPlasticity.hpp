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
    // TODO PHASE 6: Integrate synaptic creation with working memory and development stages
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);

    // Remove synapse
    // TODO PHASE 6: Integrate synaptic removal with working memory consolidation
    bool removeSynapse(Brain* brain, SynapseId synapse);

    // Create new neuron
    // TODO PHASE 6: Integrate neuron creation with development and learning
    NeuronId createNeuron(Brain* brain, NeuronType type);

    // Remove neuron
    // TODO PHASE 6: Integrate neuron removal with development and memory consolidation
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
