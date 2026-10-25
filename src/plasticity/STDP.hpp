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
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    // Update with reward modulation
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Reward-modulated parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setPredictionError(float error);
    float getPredictionError() const;
    
    // Reset eligibility trace
    void resetEligibilityTrace();
    
    // Get current eligibility trace
    float getEligibilityTrace() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau,
                   float learningRate = 0.01f, float predictionError = 0.0f);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// PLACEHOLDER - Phase 2
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
    void setA1Plus(float a1Plus);    // First-order potentiation
    float getA1Plus() const;
    void setA2Plus(float a2Plus);    // Second-order potentiation
    float getA2Plus() const;
    void setA1Minus(float a1Minus);  // First-order depression
    float getA1Minus() const;
    void setA2Minus(float a2Minus);  // Second-order depression
    float getA2Minus() const;
    void setTauPlus(float tauPlus);  // Potentiation time constant
    float getTauPlus() const;
    void setTauMinus(float tauMinus); // Depression time constant
    float getTauMinus() const;
    
    // Update parameters from config
    void configure(float a1Plus, float a2Plus, float a1Minus, float a2Minus,
                   float tauPlus, float tauMinus);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
