#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Anti-Hebbian plasticity rule: decrease weight when neurons fire together
// Implements anti-correlation learning for competition and stabilization

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
    void setMaxWeight(float weight);
    float getMaxWeight() const;
    void setMinWeight(float weight);
    float getMinWeight() const;
    void setTimeWindow(float window);
    float getTimeWindow() const;
    
    // Configuration
    void configure(float learningRate, float maxWeight, float minWeight, float timeWindow);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
