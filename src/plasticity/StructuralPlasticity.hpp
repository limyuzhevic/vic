#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <string>

namespace nlm {

// Structural plasticity: creation and removal of synapses and neurons
// Implements real structural plasticity with biological plausibility

class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse between neurons
    // Returns INVALID_SYNAPSE_ID if creation fails
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight, SynapticDelay delay = 1);
    
    // Remove synapse
    // Returns true if synapse was successfully removed
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron
    // Returns INVALID_NEURON_ID if creation fails
    NeuronId createNeuron(Brain* brain, NeuronType type, float x = 0.0f, float y = 0.0f, float z = 0.0f);
    
    // Remove neuron and all its synapses
    // Returns true if neuron was successfully removed
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
    // Get creation rate (synaptogenesis probability per step)
    float getSynaptogenesisRate() const;
    void setSynaptogenesisRate(float rate);
    
    // Get pruning rate (pruning probability per step)
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
    
    // Get maximum total synapses in brain
    size_t getMaxTotalSynapses() const;
    void setMaxTotalSynapses(size_t max);
    
    // Update structural plasticity based on neural activity
    // brain: the brain instance
    // rng: random number generator for probabilistic operations
    void update(Brain* brain, RandomGenerator& rng);
    
    // Check if structural plasticity is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // Get statistics
    struct Statistics {
        size_t totalSynapsesCreated;
        size_t totalSynapsesPruned;
        size_t totalNeuronsCreated;
        size_t totalNeuronsPruned;
        float averageSynapseWeight;
        float averageConnectionsPerNeuron;
    };
    
    Statistics getStatistics() const;
    
    // Reset statistics
    void resetStatistics();
    
    // Reinitialize with new parameters
    void reinitialize();
    
    // Configure for different network topologies
    void configureForDenseNetwork();
    void configureForSparseNetwork();
    void configureForSmallWorld();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
