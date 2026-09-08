#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Bienenstock-Cooper-Munro (BCM) rule with synaptic scaling
// PLACEHOLDER - Phase 2
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
    
    // BCM parameters
    void setTheta(float theta);    // Sliding threshold
    float getTheta() const;
    void setLearningRate(float rate);    // Base learning rate
    float getLearningRate() const;
    void setSynapticScaling(float scaling);    // Synaptic weight scaling
    float getSynapticScaling() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
