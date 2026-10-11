#pragma once

#include "PlasticityRule.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Hebbian learning implementation
// Based on the biological "cells that fire together, wire together" principle
// Forms symmetric synaptic weights based on correlated neural activity
// Phase 2: Enhanced implementation with better weight normalization and stability

class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    // Update synapse weights based on spike timing
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Apply weight change with safety checks
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get learning rule name
    const char* getName() const override;
    
    // Learning rate configuration
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Weight normalization configuration
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setWeightNormalization(bool enable);
    bool isWeightNormalizationEnabled() const;
    
    // Stability configuration
    void setStabilityFactor(float factor);
    float getStabilityFactor() const;
    
    // Advanced Hebbian variants
    void enableBCM(bool enable);
    bool isBCMEnabled() const;
    
    // Get statistical information
    float getAverageWeightChange() const;
    size_t getSynapseCountUpdated() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

