#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Bienenstock-Cooper-Munro (BCM) rule: dynamic threshold for learning
// Implements homeostatic plasticity for stability and criticality

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
    void setMaxWeight(float weight);
    float getMaxWeight() const;
    void setMinWeight(float weight);
    float getMinWeight() const;
    void setKappa(float kappa);        // BCM threshold variable
    float getKappa() const;
    void setMu(float mu);              // Sliding threshold rate
    float getMu() const;
    void setTimeWindow(float window);
    float getTimeWindow() const;
    
    // Configuration
    void configure(float learningRate, float maxWeight, float minWeight,
                   float kappa, float mu, float timeWindow);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
