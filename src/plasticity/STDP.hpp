#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Enhanced implementation with triplet interactions and neuromodulation support

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
    
    // Advanced parameters
    void setTimeWindow(float window);
    float getTimeWindow() const;
    void setTripletWeight(float weight);
    float getTripletWeight() const;
    
    // Neuromodulation integration
    void setNeuromodulationFactor(float factor);
    float getNeuromodulationFactor() const;
    
    // Configuration
    void configure(float ltpWeight, float ltdWeight, float tau);
    void configure(const std::string& configPrefix = "");
    std::string getDescription() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Enhanced implementation with reward prediction error integration
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
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setRewardPredictionError(float rpe);
    float getRewardPredictionError() const;
    
    // Configuration
    void configure(float learningRate, float rpeFactor);
    void configure(const std::string& configPrefix = "");
    std::string getDescription() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Enhanced implementation with biological triplet model
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
    void setAplus(float aplus);
    float getAplus() const;
    void setAminus(float aminus);
    float getAminus() const;
    void setTauPlus(float tauPlus);
    float getTauPlus() const;
    void setTauMinus(float tauMinus);
    float getTauMinus() const;
    
    // Configuration
    void configure(float aplus, float aminus, float tauPlus, float tauMinus);
    void configure(const std::string& configPrefix = "");
    std::string getDescription() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
