#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Advanced STDP implementation with multiple timescales and reward modulation

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
    
    // Advanced STDP parameters
    void setAdditiveLTPWeight(float weight);    // Additive LTP component
    float getAdditiveLTPWeight() const;
    void setAdditiveLTDWeight(float weight);    // Additive LTD component
    float getAdditiveLTDWeight() const;
    void setMultiTimeConstant(float tau1, float tau2); // Multi-timescale parameters
    void getMultiTimeConstant(float& tau1, float& tau2) const;
    void setDepressionNormalization(bool enable); // Normalization for depression
    bool getDepressionNormalization() const;
    void setEligibilityTraceDecay(float decayRate); // Eligibility trace decay
    float getEligibilityTraceDecay() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    void configureAdvanced(float ltpWeight, float ltdWeight, float tau1, float tau2, 
                          bool normalizeDepression, float traceDecay);
    
    // Reward-modulated STDP (R-STDP) methods
    void enableRewardModulation(float predictionError = 1.0f);
    void disableRewardModulation();
    bool isRewardModulated() const;
    float getModulationFactor() const;
    
    // Learning curve parameters
    void setLearningRate(float rate);        // Base learning rate
    float getLearningRate() const;
    void setForgettingRate(float rate);      // Weight decay rate
    float getForgettingRate() const;
    
    // State management
    void reset();                            // Reset plasticity state
    bool isConverged() const;                // Check if weights have converged
    float getWeightChangeSum() const;        // Total accumulated weight changes
    
    // Batch operations
    void processSpikeTrains(const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           std::vector<std::pair<float, float>>& weightChanges);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Advanced STDP with reward prediction error modulation
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "R-STDP"; }
    
    // Reward modulation parameters
    void setPredictionErrorWeight(float weight);      // Weight for prediction error
    float getPredictionErrorWeight() const;
    void setTemporalDifferenceFactor(float factor);   // TD learning rate
    float getTemporalDifferenceFactor() const;
    void setRewardHistorySize(size_t size);           // Size of reward history
    size_t getRewardHistorySize() const;
    
    // Reward processing
    void addReward(float reward, Timestamp time);
    void clearRewards();
    float getAverageReward() const;
    float getRewardPredictionError() const;
    
    // Integration with external reward signals
    void setRewardSource(std::function<float(Timestamp)> source);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Advanced STDP accounting for triplet spike interactions
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "TripletSTDP"; }
    
    // Triplet parameters
    void setPairingStrength(float strength);         // Pairing rule strength
    float getPairingStrength() const;
    void setPostBeforePreFactor(float factor);       // Post-before-pre contribution
    float getPostBeforePreFactor() const;
    void setPreBeforePostFactor(float factor);       // Pre-before-post contribution
    float getPreBeforePostFactor() const;
    
    // Three spike interaction rules
    void enableThreeSpikeRules(bool enable);         // Enable/disable triplet rules
    bool areThreeSpikeRulesEnabled() const;
    void setTripleWindows(float preWindow, float postWindow); // Time windows for triplets
    void getTripleWindows(float& preWindow, float& postWindow) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
