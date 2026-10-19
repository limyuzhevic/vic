#pragma once

#include "PlasticityRule.hpp"
#include "../neuromodulation/neuromodulator_integration.hpp"

namespace nlm
{

// Comprehensive Spike-Timing-Dependent Plasticity (STDP) implementation
// Implements asymmetric STDP with NMDA dependence, dopamine modulation,
// spatial windows, and multiple plasticity regimes

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
    void setLTPWeight(float weight);    // Long-term potentiation weight (A+)
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight (A-)
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    void setNMDAThreshold(float threshold);  // NMDA receptor activation threshold
    float getNMDAThreshold() const;
    void setDopamineModulation(float modulation);
    float getDopamineModulation() const;
    void setSpatialWindow(float radius);
    float getSpatialWindow() const;
    void setShortTermPlasticity(bool enable);
    bool getShortTermPlasticity() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau, float nmdatThreshold = 0.5f);
    
    // STDP variant controls
    void enableAsymmetricSTDP(bool enable);
    void enableSpatialSTDP(bool enable);
    void enableNMDASTDP(bool enable);
    void enableDopamineSTDP(bool enable);
    void enableTripletInteractions(bool enable);
    void enableNeuromodulatorIntegration(bool enable);
    
    // Getters for internal state
    bool isAsymmetricSTDP() const;
    bool isSpatialSTDP() const;
    bool isNMDASTDP() const;
    bool isDopamineSTDP() const;
    bool hasTripletInteractions() const;
    bool hasNeuromodulatorIntegration() const;
    
    // Get eligibility trace for reward-modulated learning
    float getEligibilityTrace() const;
    
    // Calculate STDP change for specific spike pair
    float calculatePairChange(float preTime, float postTime) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Reward-modulated STDP (R-STDP) with prediction error
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
    
    // Reward parameters
    void setPredictionError(float error);
    void setDopamineModulation(float modulation);
    void setDelayCreditAssignment(float delay);
    void setAsymmetricRewardSensitivity(bool enable);
    void setTrialBasedRewardModulation(bool enable);
    
    // Getters
    float getPredictionError() const;
    float getDopamineModulation() const;
    float getDelayCreditAssignment() const;
    bool getAsymmetricRewardSensitivity() const;
    bool getTrialBasedRewardModulation() const;
    
    // Reward prediction error calculation
    void calculatePredictionError(float predicted, float actual);
    
    // Reward-modulated STDP calculation
    float calculateRSTPDElasticity(float stdpChange, float rewardModulation) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Triplet-based STDP incorporating pre-post interactions
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
    void setPreTraceTimeConstant(float tau1);
    void setPostTraceTimeConstant(float tau2);
    void setTripletModulation(float modulation);
    float getPreTraceTimeConstant() const;
    float getPostTraceTimeConstant() const;
    float getTripletModulation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Metaplasticity - plasticity of plasticity
class MetaPlasticity : public PlasticityRule {
public:
    MetaPlasticity();
    ~MetaPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Metaplasticity parameters
    void setMetaplasticityRate(float rate);
    void setLearningHistoryCapacity(size_t capacity);
    void setStabilityFactor(float factor);
    float getMetaplasticityRate() const;
    size_t getLearningHistoryCapacity() const;
    float getStabilityFactor() const;
    
    // Learning history management
    void recordLearningEvent(float magnitude, float time);
    float getAverageLearningMagnitude() const;
    float getLearningVolatility() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm