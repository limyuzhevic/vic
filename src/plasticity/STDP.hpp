#pragma once

#include "PlasticityRule.hpp"
#include "../core/Types/Types.hpp"

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
    
    // Reward-modulated STDP
    void setRewardModulation(float strength);
    float getRewardModulation() const;
    
    // Homeostatic plasticity
    void setHomeostaticTarget(float target);
    float getHomeostaticTarget() const;
    
    // Metaplasticity
    void setMetaplasticRate(float rate);
    float getMetaplasticRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Combines STDP with reward prediction error signals
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
    
    // Reward signal integration
    void setRewardSignal(float reward);
    float getRewardSignal() const;
    void setPredictionError(float error);
    float getPredictionError() const;
    
    // Modulation strength
    void setModulationStrength(float strength);
    float getModulationStrength() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Captures higher-order spike correlations beyond pairwise interactions
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
    
    // Triplet interaction parameters
    void setTripletWindow(float window);
    float getTripletWindow() const;
    
    // Trace variables for triplet detection
    void enableTripletTracking(bool enable);
    bool isTripletTrackingEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// BCM Theory (Bienenstock-Cooper-Munro) for homeostatic plasticity
// Maintains synaptic stability and prevents runaway excitation
class BCMPlasticity : public PlasticityRule {
public:
    BCMPlasticity();
    ~BCMPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // BCM threshold dynamics
    void setThetaBase(float thetaBase);
    float getThetaBase() const;
    void setThetaLearningRate(float rate);
    float getThetaLearningRate() const;
    
    // Normalizing factors
    void setSynapticMaximum(float maxWeight);
    float getSynapticMaximum() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Oja learning rule with normalization
// Hebbian learning rule that includes normalization for stable learning
class OjaLearning : public PlasticityRule {
public:
    OjaLearning();
    ~OjaLearning() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Normalization parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setOutputNormalization(float factor);
    float getOutputNormalization() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Metaplasticity rule
// Plasticity that regulates the plasticity itself
class Metaplasticity : public PlasticityRule {
public:
    Metaplasticity();
    ~Metaplasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Metaplastic parameters
    void setLearningHistoryCapacity(float capacity);
    float getLearningHistoryCapacity() const;
    void setMetaplasticRate(float rate);
    float getMetaplasticRate() const;
    
    // Eligibility trace for metaplastic updates
    void enableMetaplasticTrace(bool enable);
    bool isMetaplasticTraceEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
