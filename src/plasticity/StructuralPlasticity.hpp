#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Structural plasticity constants
namespace StructuralPlasticityConstants {
    // Rates for structural changes (per simulation step)
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;  // Probability of new synapse per step
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;         // Probability of pruning per step
    
    // Thresholds for structural changes
    constexpr float MIN_WEIGHT_THRESHOLD = 0.05f;             // Below this weight, synapse is weak
    constexpr float ACTIVITY_THRESHOLD = 0.001f;             // Below this activity, synapse is unused
    
    // Limits for network connectivity
    constexpr size_t MAX_SYNAPSES_PER_NEURON = 100;           // Maximum outgoing synapses per neuron
    constexpr size_t MAX_TOTAL_SYNAPSES = 1000000;            // Global maximum synapses
    
    // Activity dependency settings
    constexpr bool DEFAULT_USE_ACTIVITY_DEPENDENCY = true;  // Whether to use activity for pruning
    
    // Development stage modifiers
    constexpr float INITIAL_DEVELOPMENT_PLASTICITY = 1.0f;    // High plasticity at start
    constexpr float MATURE_DEVELOPMENT_PLASTICITY = 0.2f;     // Stable in adulthood
    constexpr float CRITICAL_PERIOD_PLASTICITY = 0.8f;        // High but not maximal
    
    // Time constants for development
    constexpr size_t INITIAL_DEVELOPMENT_STEPS = 1000;        // Development period length
    constexpr size_t REPLAY_INTERVAL = 100;                   // Replay episodes every 100 steps
    constexpr size_t CONSOLIDATION_INTERVAL = 1000;           // Consolidate every 1000 steps
}

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
    
    // Update structural plasticity
    void update(Brain* brain, class RandomGenerator& rng);
    
    // Statistics
    size_t getTotalSynapsesCreated() const;
    size_t getTotalSynapsesPruned() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
