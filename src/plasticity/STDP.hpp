#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// IMPLEMENTED: Real STDP with exponential windows (Phase 2-7)

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
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Implements STDP with reward/eligibility traces
// Based on: 
// - Function: Approximation for reward prediction (Sutton, 1988)
// - Reinforcement learning with temporal difference errors
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Reward modulation parameters
    void setLearningRate(float rate);      // Base learning rate (η)
    float getLearningRate() const;
    void setTraceDecayRate(float decay);   // Eligibility trace decay (λ)
    float getTraceDecayRate() const;
    void setPredictionError(float error);  // TD prediction error δ
    float getPredictionError() const;
    void setCreditAssignmentStrength(float strength); // Reward signal integration
    float getCreditAssignmentStrength() const;
    
    // Update eligibility traces
    void decayEligibilityTrace(float dt);
    float getEligibilityTrace() const;
    void addToEligibilityTrace(float delta);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Implements generalized STDP considering pre-post triplets
// Based on: 
// - Natural STDP in cortical networks (Pfister et al., 2006)
// - Triple-synapse interactions for more biological realism
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
    
    // Triplet STDP parameters
    void setLTPWeight(float weight);      // A+ for pre-post potentiation
    float getLTPWeight() const;
    void setLTDWeight(float weight);      // A- for post-pre depression
    float getLTDWeight() const;
    void setTripletLTPWeight(float weight);   // Ax for pre-pre-post potentiation
    float getTripletLTPWeight() const;
    void setTripletLTDWeight(float weight);   // Ay for post-post-pre depression
    float getTripletLTDWeight() const;
    void setTimeConstant(float tau);      // τ for exponential decay
    float getTimeConstant() const;
    
    // Triplet activity tracking
    void recordPreSpike(Timestamp t);
    void recordPostSpike(Timestamp t);
    void resetActivityHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
