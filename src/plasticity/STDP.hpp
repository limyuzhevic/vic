#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// STDP: Spike-Timing-Dependent Plasticity
// Implements real STDP with biological plausibility
// Based on experimental data from hippocampal and cortical synapses

class STDP {
public:
    STDP();
    ~STDP();
    
    // Update synapse weight based on spike timing
    // Implements pairwise STDP rule with exponential windows
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt);
    
    // Apply weight change with bounds and additional effects
    void applyWeightChange(Synapse* synapse, SynapticWeight delta);
    
    // Get plasticity name
    const char* getName() const;
    
    // STDP parameters configuration
    void setLTPWeight(float weight);      // A+: Long-Term Potentiation weight
    void setLTDWeight(float weight);      // A-: Long-Term Depression weight
    void setTimeConstant(float tau);      // STDP time window constant
    void setMinWeight(float minWeight);   // Minimum synaptic weight
    void setMaxWeight(float maxWeight);   // Maximum synaptic weight
    void setLearningRate(float rate);     // Overall learning rate
    void setSpikeHistoryLimit(size_t limit); // Max spikes to store per neuron
    
    // Get current parameter values
    float getLTPWeight() const;
    float getLTDWeight() const;
    float getTimeConstant() const;
    float getMinWeight() const;
    float getMaxWeight() const;
    float getLearningRate() const;
    size_t getSpikeHistoryLimit() const;
    
    // Configure all parameters at once
    void configure(float ltpWeight, float ltdWeight, float tau,
                   float minWeight = -1.0f, float maxWeight = 1.0f,
                   float learningRate = 0.01f);
    
    // Get statistics
    struct Statistics {
        size_t totalLTPEvents;
        size_t totalLTDEvents;
        float totalWeightChange;
        float averageLTPMagnitude;
        float averageLTDMagnitude;
    };
    
    Statistics getStatistics() const;
    
    // Reset statistics
    void resetStatistics();
    
    // Enable/disable STDP for specific synapse types
    void enableForNeuronType(NeuronType type, bool enabled);
    bool isEnabledForNeuronType(NeuronType type) const;
    
    // Get presynaptic and postsynaptic spike contributions
    struct SpikeContribution {
        Timestamp spikeTime;
        float contribution;
        bool isPresynaptic;
    };
    
    std::vector<SpikeContribution> getContributions(
        const std::vector<Timestamp>& preSpikes,
        const std::vector<Timestamp>& postSpikes) const;
    
    // Check if STDP is biologically realistic
    bool isBiologicallyPlausible() const;
    
    // Adjust parameters to be within biological ranges
    void normalizeToBiologicalRanges();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
