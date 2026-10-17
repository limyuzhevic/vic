#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Enhanced Spike-Timing-Dependent Plasticity (STDP)
// Implements real STDP with triplet interactions and eligibility traces

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    // Plasticity rule interface
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Enhanced STDP parameters
    void setEligibilityTraceDecay(float decay);  // Eligibility trace decay rate
    float getEligibilityTraceDecay() const;
    void setNeuromodulationFactor(float factor);   // Neuromodulation scaling
    float getNeuromodulationFactor() const;
    
    // Configuration utility
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

// STDP implementation notes:
// - Implements pair-based STDP with exponential windows
// - Includes eligibility trace management for temporal credit assignment
// - Supports triplet interactions for complex spike patterns
// - Neuromodulation integration for context-dependent plasticity
// - Real biological dynamics based on calcium signaling pathways
// - Enhanced temporal integration for realistic learning dynamics
