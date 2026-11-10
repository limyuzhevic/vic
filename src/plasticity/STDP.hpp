#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Enhanced STDP with better control over learning dynamics

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
    
    // Enhanced STDP parameters for better selectivity
    void setSTDPLearningRate(float rate);      // Overall learning rate
    float getSTDPLearningRate() const;
    void setSelectivityFactor(float factor);  // Pattern selectivity factor
    float getSelectivityFactor() const;
    void setTemporalWindow(float window);     // Temporal window for STDP
    float getTemporalWindow() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    void configureAdvanced(float ltpWeight, float ltdWeight, float tau,
                          float learningRate, float selectivityFactor, float temporalWindow);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Reward-modulated STDP with prediction error integration
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
    void setPredictionErrorWeight(float weight);
    float getPredictionErrorWeight() const;
    void setRewardRange(float min, float max);
    void getRewardRange(float& min, float& max) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Triplet STDP with better handling of spike trains
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
    void setTraceTauPre(float tau);    // Pre-synaptic trace time constant
    float getTraceTauPre() const;
    void setTraceTauPost(float tau);   // Post-synaptic trace time constant
    float getTraceTauPost() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
