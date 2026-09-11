#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
// Current implementation: Covariance-based Hebbian learning
// Mathematical formulation: Δw = η * (coactivity - baseline)
// Simplified version: Δw = η * (correlationCount) with bounds clamping
// Implements: "neurons that fire together, wire together" with threshold
// Biological inspiration: AMPA receptor trafficking, hippocampal learning
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
