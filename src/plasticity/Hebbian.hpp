#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
// Real implementation based on covariance rule: Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
// Simplified version for spike-based systems: Δw = η * (coactivity - baseline)
// This implements "neurons that fire together, wire together" but with a
// threshold to prevent runaway potentiation.
// Biological inspiration: AMPA receptor trafficking in hippocampus
//                     : Correlation-based learning in visual cortex

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
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
