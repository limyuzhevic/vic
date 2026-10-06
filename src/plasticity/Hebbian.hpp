#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian plasticity rule: "neurons that fire together, wire together"
// Implements symmetric Hebbian learning for cooperative synaptic strengthening
// Captures associative learning through correlated neuronal activity

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
    void setMaxWeight(float weight);
    float getMaxWeight() const;
    void setMinWeight(float weight);
    float getMinWeight() const;
    void setTimeWindow(float window);
    float getTimeWindow() const;
    void setEligibilityTraceDecay(float decay);
    float getEligibilityTraceDecay() const;
    void setRewardModulation(float modulation);
    float getRewardModulation() const;
    
    // Configuration
    void configure(float learningRate, float maxWeight, float minWeight,
                   float timeWindow, float traceDecay, float rewardModulation);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
