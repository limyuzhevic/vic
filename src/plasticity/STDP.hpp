#pragma once

#include "PlasticityRule.hpp"
#include <unordered_map>

namespace nlm {

// Helper struct for synapse handle
struct SynapseHandle {
    uint64_t source;
    uint64_t destination;
    
    SynapseHandle() : source(0), destination(0) {}
    SynapseHandle(uint64_t src, uint64_t dst) : source(src), destination(dst) {}
    
    bool operator==(const SynapseHandle& other) const {
        return source == other.source && destination == other.destination;
    }
    
    bool operator!=(const SynapseHandle& other) const {
        return !(*this == other);
    }
};

// Spike-Timing-Dependent Plasticity (STDP)
// Advanced implementation with triplet interactions and eligibility traces

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Update synaptic weights based on spike timing
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Apply weight change with biological constraints
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Advanced STDP parameters
    void setTripletParameters(float aPlus, float aMinus, float bPlus, float bMinus);
    void setLearningRate(float rate);
    void setNeuromodulation(float factor);
    void setEligibilityTraceParams(float decayRate, float timeConstant);
    
    // Eligibility trace management
    void updateEligibilityTrace(Synapse* synapse, Timestamp currentTime);
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// STDP with eligibility traces for reward modulation
class RewardModulatedSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "Reward-Modulated STDP"; }
};

// Spike-timing dependent plasticity with triplet interactions
// More biologically accurate STDP model
class TripletSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "Triplet STDP"; }
};

// Eligibility trace for STDP
struct STDPBiomechela {
    float trace;
    Timestamp lastUpdated;
    float decayRate;
    float timeConstant;
    
    STDPBiomechela() : trace(0.0f), lastUpdated(0), decayRate(0.01f), timeConstant(100.0f) {}
};

} // namespace nlm
