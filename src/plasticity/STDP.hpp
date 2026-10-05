#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// STDP (Spike-Timing-Dependent Plasticity) constants
namespace STDPConstants {
    // Base STDP parameters from biological observations
    constexpr float BASE_LTP_WEIGHT = 0.01f;     // Long-term potentiation weight
    constexpr float BASE_LTD_WEIGHT = 0.012f;    // Long-term depression weight (slightly stronger than LTP)
    constexpr float BASE_TIME_CONSTANT = 20.0f;  // STDP time window (ms)
    
    // Weight bounds
    constexpr float MIN_WEIGHT = -1.0f;          // Minimum synaptic weight (inhibitory)
    constexpr float MAX_WEIGHT = 1.0f;           // Maximum synaptic weight (excitatory)
    
    // Activity-dependent parameters
    constexpr float WEIGHT_CLAMP_THRESHOLD = 1e-6f;  // Threshold for applying weight changes
    
    // Time constants (ms)
    constexpr float LTP_TIME_CONSTANT = 20.0f;   // Potentiation time constant
    constexpr float LTD_TIME_CONSTANT = 20.0f;   // Depression time constant
    constexpr float SHORT_TERM_WINDOW = 100.0f;   // Short-term plasticity window (ms)
    
    // Biological plausibility ranges
    constexpr float MIN_TIME_CONSTANT = 1.0f;    // Minimum time constant
    constexpr float MAX_TIME_CONSTANT = 100.0f;  // Maximum time constant
    constexpr float MIN_LTP_WEIGHT = 0.0f;       // Minimum LTP weight
    constexpr float MAX_LTP_WEIGHT = 0.1f;       // Maximum LTP weight
    constexpr float MIN_LTD_WEIGHT = 0.0f;       // Minimum LTD weight
    constexpr float MAX_LTD_WEIGHT = 0.1f;       // Maximum LTD weight
}

// Spike-Timing-Dependent Plasticity
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
    
    // Parameters
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    void setRewardModulation(float modulation);
    float getRewardModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
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
    
    // Parameters for triplet rule
    void setA1(float a1);      // First spike pair interaction
    float getA1() const;
    void setA2(float a2);      // Second spike pair interaction
    float getA2() const;
    void setB1(float b1);      // Third spike interaction
    float getB1() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
