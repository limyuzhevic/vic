#pragma once

#include "PlasticityRule.hpp"
#include <vector>

namespace nlm {

// Oja's rule for normalized Hebbian learning
// This implements a normalization mechanism that prevents runaway potentiation
// by scaling weights proportional to the neuron's activity
class OjaRule : public PlasticityRule {
public:
    OjaRule();
    ~OjaRule() override;

    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;

    // Oja's rule parameters
    void setLearningRate(float rate);                    // Learning rate (η)
    float getLearningRate() const;
    void setNormalizationFactor(float factor);            // Normalization factor (λ)
    float getNormalizationFactor() const;
    void setMaxWeight(float max);                         // Maximum weight bound
    float getMaxWeight() const;
    void setMinWeight(float min);                         // Minimum weight bound
    float getMinWeight() const;

    // Normalized Hebbian implementation details
    void updateRunningVariance(SynapticWeight weightDelta);  // Update running variance for normalization
    float getRunningVariance() const;                       // Get current running variance
    float calculateNormalizedDelta(SynapticWeight weight, SynapticWeight preSignal, SynapticWeight postSignal) const; // Calculate normalized weight change
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm