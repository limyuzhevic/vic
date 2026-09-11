#pragma once

#include "../plasticity/PlasticityRule.hpp"

namespace nlm {

// Bienenstock-Cooper-Munro (BCM) learning rule
// Implements sliding threshold plasticity: Hebbian with adaptive threshold
// Promotes stability while maintaining activity-dependent learning
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
    void setThetaPlus(float theta);
    float getThetaPlus() const;
    void setThetaMinus(float theta);
    float getThetaMinus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
