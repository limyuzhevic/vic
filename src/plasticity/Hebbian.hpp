#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Advanced Hebbian learning implementation
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Basic configuration
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    // Advanced Hebbian learning rules
    void setNormalizationFactor(float factor);
    float getNormalizationFactor() const;
    void setBCMThreshold(float threshold);
    float getBCMThreshold() const;
    
    // Learning rule selection
    void enableCovarianceRule();      // Standard covariance rule
    void enableOjaRule(float factor = 1.0f);  // Normalized Hebbian (Oja's rule)
    void enableBCMTheory(float threshold = 0.5f);  // Sliding threshold theory
    void enableImplicitNormalization();  // Automatic normalization
    
private:
    struct Impl;
    Impl* pImpl;
    
    void applyHebbianWeightChange(Synapse* synapse, SynapticWeight delta);
};

} // namespace nlm