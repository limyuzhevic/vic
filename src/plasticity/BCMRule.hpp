#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Bienenstock-Cooper-Munro (BCM) learning rule: sliding threshold mechanism
// Δw = ℳ(y - θ(y)) * (y - θ)  where θ is activity-dependent threshold
// Biological inspiration: synaptic modification based on post-synaptic activity
// Used for normalizing firing rates and maintaining stability

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
    
    void setModulationStrength(float strength);
    float getModulationStrength() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm