#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// PLACEHOLDER - Phase 2 will implement real plasticity rules

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // TODO PHASE 2: Implement real plasticity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// PLACEHOLDER - Phase 2 will implement real Hebbian learning
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP)
// Combines STDP with reward signals for model-based learning

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
    void setRewardRate(float rate);
    float getRewardRate() const;
    void configure(float ltpWeight, float ltdWeight, float tau, float rewardRate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// Implements "neurons that fire together, divide together"
// Based on: Desai et al. (1996), "Competitive synaptic plasticity in visual cortex"

class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMinMaxWeights(float minWeight, float maxWeight);
    void setMinWeight(float minWeight);
    void setMaxWeight(float maxWeight);
    float getMinWeight() const;
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Triplet STDP with presynaptic and postsynaptic spike history
// Advanced STDP incorporating triplet interactions (Gerstner et al., 1996)

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
    
    // Parameters
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstants(float tauPre, float tauPost, float tau);
    void setTripletWeights(float aPlus, float aMinus, float bPlus, float bMinus);
    void configure(float ltpWeight, float ltdWeight, float tauPre, float tauPost, float tau, float aPlus, float aMinus, float bPlus, float bMinus);
    
private:
    struct Impl;
    Impl* pImpl;
};

// BCM (Bienenstock-Cooper-Munro) rule
// Homeostatic synaptic scaling based on postsynaptic activity
// Implements synaptic competition while maintaining stability

class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setBeta(float beta);
    float getBeta() const;
    void setTheta(float theta);
    float getTheta() const;
    void setMinMaxWeights(float minWeight, float maxWeight);
    void setMinWeight(float minWeight);
    void setMaxWeight(float maxWeight);
    float getMinWeight() const;
    float getMaxWeight() const;
    void configure(float beta, float theta, float learningRate, float minWeight, float maxWeight);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
