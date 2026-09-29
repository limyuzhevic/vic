#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// PLACEHOLDER - Phase 2 will implement real STDP

class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
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
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    // Reward modulation support (for compatibility with RewardModulatedSTDP)
    void setRewardStrength(float strength) { /* Default implementation - no-op */ }
    float getRewardStrength() const { return 1.0f; }
    
    void setTDDiscount(float discount) { /* Default implementation - no-op */ }
    float getTDDiscount() const { return 1.0f; }
    
    void resetEligibilityTrace(Synapse* synapse) { /* Default implementation - no-op */ }
    float getEligibilityTrace(Synapse* synapse) const { return 0.0f; }
    
    void configure(float ltpWeight, float ltdWeight, float tau,
                   float rewardStrength, float tDiscount) { /* Default implementation - no-op */ }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// PLACEHOLDER - Phase 2
class RewardModulatedSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "R-STDP"; }
};

// Spike-timing dependent plasticity with triplet interactions
// Advanced implementation capturing non-Markovian temporal dependencies
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Triplet parameters
    void setA1Plus(float a1plus);       // Pre-pre-post potentiation
    float getA1Plus() const;
    void setA1Minus(float a1minus);     // Pre-pre-post depression
    float getA1Minus() const;
    void setA2Plus(float a2plus);       // Pre-post-post potentiation
    float getA2Plus() const;
    void setA2Minus(float a2minus);     // Pre-post-post depression
    float getA2Minus() const;
    void setTauPlus(float tauPlus);     // Potentiation time constant
    float getTauPlus() const;
    void setTauMinus(float tauMinus);   // Depression time constant
    float getTauMinus() const;
    
    // Triplet rule configuration
    void configure(float a1plus, float a1minus, float a2plus, float a2minus,
                   float tauPlus, float tauMinus);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm


