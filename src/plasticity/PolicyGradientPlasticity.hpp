#pragma once

#include "PlasticityRule.hpp"
#include <vector>

namespace nlm {

// Policy gradient based plasticity
// Implements policy gradient methods for reward-modulated synaptic changes
class PolicyGradientPlasticity : public PlasticityRule {
public:
    PolicyGradientPlasticity();
    ~PolicyGradientPlasticity() override;

    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;

    // Policy gradient parameters
    void setLearningRate(float rate);                    // Learning rate (α)
    float getLearningRate() const;
    void setPolicyTemperature(float temperature);        // Policy temperature (β) for exploration
    float getPolicyTemperature() const;
    void setAdvantageEstimation(float estimation);       // Advantage estimation factor (A)
    float getAdvantageEstimation() const;
    void setValueFunctionWeight(float weight);           // Value function weight (w)
    float getValueFunctionWeight() const;
    void setMaxWeight(float max);                         // Maximum weight bound
    float getMaxWeight() const;
    void setMinWeight(float min);                         // Minimum weight bound
    float getMinWeight() const;

    // Policy gradient implementation
    void updatePolicyGradient(SynapticWeight weight, float advantage, float logProbability);  // Update policy gradient
    void updateValueFunction(SynapticWeight weight, float value);                             // Update value function
    float calculatePolicyGradient(float weight, float action) const;                         // Calculate policy gradient
    float estimateAdvantage(float weight, float reward) const;                               // Estimate advantage
    float predictValue(float weight) const;                                                  // Predict value
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm