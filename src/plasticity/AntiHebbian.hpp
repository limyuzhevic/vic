#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Anti-Hebbian plasticity rule: decrease weight when neurons fire together
// PLACEHOLDER - Phase 2 will implement real Anti-Hebbian learning
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
    
    void setThreshold(float threshold);
    float getThreshold() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
