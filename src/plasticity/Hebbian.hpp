#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian plasticity rule: "neurons that fire together, wire together"
// Enhanced with reward-modulated learning support

class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Enhanced Hebbian with reward modulation support
    // Implements Reward-Modulated Hebbian (R-Hebbian)
    // Δw = Hebbian_delta * reward_signal * eligibility_trace
    void updateWithReward(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt,
                          float rewardSignal);
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    void setCorrelationWindow(float window);  // Time window for correlation
    float getCorrelationWindow() const;
    
    // Update parameters from config
    void configure(float learningRate, float maxWeight, float minWeight,
                   float correlationWindow = 100.0f);
    
    // Hebbian statistics and debugging
    std::string getStatistics() const;
    
    // Advanced Hebbian variants
    // BCM (Bienenstock-Cooper-Munro) rule for stability
    void updateWithBCM(Synapse* synapse,
                       const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       TimestepDuration dt);

private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// PLACEHOLDER - Phase 2
class AntiHebbian : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "AntiHebbian"; }
    
    // Anti-Hebbian specific parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
};

// Bienenstock-Cooper-Munro (BCM) rule
// PLACEHOLDER - Phase 2
class BCMRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override { return "BCM"; }
    
    // BCM specific parameters
    void setTheta(float theta);  // Threshold for potentiation/depression
    float getTheta() const;
    void setLearningRate(float rate);
    float getLearningRate() const;
};

} // namespace nlm
