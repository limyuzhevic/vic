#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Structural plasticity: creation and removal of synapses and neurons
// Implements activity-dependent synaptogenesis and pruning
// Contributes to brain development and homeostatic regulation

class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse between neurons
    // Returns synapse ID if successful, INVALID_SYNAPSE_ID otherwise
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse
    // Returns true if synapse was removed, false otherwise
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron of specified type
    // Returns neuron ID if successful, INVALID_NEURON_ID otherwise
    // Note: Currently returns INVALID_NEURON_ID (Phase 2 TODO)
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron and all its synapses
    // Returns true if neuron was removed, false otherwise
    // Note: Currently returns false (Phase 2 TODO)
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
    // Get current synaptogenesis rate
    float getSynaptogenesisRate() const;
    void setSynaptogenesisRate(float rate);
    
    // Get current pruning rate
    float getPruningRate() const;
    void setPruningRate(float rate);
    
    // Get minimum weight threshold for pruning
    float getMinWeightThreshold() const;
    void setMinWeightThreshold(float threshold);
    
    // Get activity threshold for pruning
    float getActivityThreshold() const;
    void setActivityThreshold(float threshold);
    
    // Get maximum synapses per neuron
    size_t getMaxSynapsesPerNeuron() const;
    void setMaxSynapsesPerNeuron(size_t max);
    
    // Get maximum total synapses for brain
    size_t getMaxTotalSynapses() const;
    void setMaxTotalSynapses(size_t max);
    
    // Get whether activity dependency is used for pruning
    bool getUseActivityDependency() const;
    void setUseActivityDependency(bool use);
    
    // Update structural plasticity
    // Implements activity-dependent synaptogenesis and pruning
    // Uses random number generator for probabilistic operations
    void update(Brain* brain, class RandomGenerator& rng);
    
    // Statistics
    size_t getTotalSynapsesCreated() const;
    size_t getTotalSynapsesPruned() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
