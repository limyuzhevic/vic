#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Structural plasticity: creation and removal of synapses and neurons
// IMPLEMENTED: Real structural plasticity with homeostatic regulation

class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse
    // IMPLEMENTED: Real synapse creation with biological constraints
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse
    // IMPLEMENTED: Real synapse removal with activity-dependent criteria
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron
    // IMPLEMENTED: Real neuron creation with developmental constraints
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron
    // IMPLEMENTED: Real neuron removal with homeostatic regulation
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
    // Get creation rate
    float getSynaptogenesisRate() const;
    void setSynaptogenesisRate(float rate);
    
    // Get pruning rate
    float getPruningRate() const;
    void setPruningRate(float rate);
    
    // Update structural plasticity
    void update(Brain* brain, class RandomGenerator& rng);
    
    // Get structural statistics
    struct StructuralStats {
        size_t synapseCreationCount;
        size_t synapseRemovalCount;
        size_t neuronCreationCount;
        size_t neuronRemovalCount;
        float totalStructuralChange;
    };
    
    StructuralStats getStats() const;
    void resetStats();
    
    // Configuration and validation
    bool configureFromConfig(const std::string& configPrefix = "");
    std::string getDescription() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
