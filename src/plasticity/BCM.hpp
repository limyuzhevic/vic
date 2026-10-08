#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Bienenstock-Cooper-Munro (BCM) learning rule
// PLACEHOLDER - Phase 2 will implement real BCM learning
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
    
    void setThreshold(float threshold);
    float getThreshold() const;
    
    void setActivityWindow(float window);
    float getActivityWindow() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
