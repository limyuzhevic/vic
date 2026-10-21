#pragma once

#include "PlasticityRule.hpp"
#include <vector>

namespace nlm {

// Pairwise STDP with different time constants
// Implements separate time windows for potentiation (τ+) and depression (τ-)
class PairwiseSTDP : public PlasticityRule {
public:
    PairwiseSTDP();
    ~PairwiseSTDP() override;

    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;

    // Pairwise STDP parameters
    void setLTPWeight(float weight);                    // Potentiation weight (A+)
    float getLTPWeight() const;
    void setLTDWeight(float weight);                    // Depression weight (A-)
    float getLTDWeight() const;
    void setTimeConstantPotentiation(float tauPlus);    // Time constant for potentiation (τ+)
    float getTimeConstantPotentiation() const;
    void setTimeConstantDepression(float tauMinus);    // Time constant for depression (τ-)
    float getTimeConstantDepression() const;
    void setMaxWeight(float max);                        // Maximum weight
    float getMaxWeight() const;
    void setMinWeight(float min);                        // Minimum weight
    float getMinWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm