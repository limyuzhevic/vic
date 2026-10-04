#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Hebbian learning implementation
// Real implementation based on covariance-based learning rule
// 
// Biological basis:
// - Reflects synaptic strengthening through correlated activity
// - "Neurons that fire together, wire together"
// - Underlying AMPA receptor trafficking mechanisms
// - Critical for memory formation and consolidation
// 
// Mathematical formulation (Covariance rule):
// Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
// 
// Simplified version for spike-based systems:
// Δw = η * (coactivity - baseline)
// 
// Key features:
// - Covariance-based weight updates
// - Learning rate parameter for stability control
// - Weight bounds to prevent runaway excitation
// - Correlation window for temporal integration
// 
// Biological inspiration:
// - Hippocampal memory consolidation
// - Visual cortex development
// - Cortical plasticity during learning
// 
// Limitations:
// - Assumes stationarity of input statistics
// - Single learning rate (no separate potentiation/depression)
// - Simplified spike timing model

class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    // Disable copying, enable moving
    Hebbian(const Hebbian&) = delete;
    Hebbian& operator=(const Hebbian&) = delete;
    Hebbian(Hebbian&&) noexcept;
    Hebbian& operator=(Hebbian&&) noexcept;
    
    // Update synaptic weights based on co-activity patterns
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    // Apply weight change directly
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // Hebbian learning parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    // Convenience configuration method
    void configure(float learningRate, float maxWeight, float minWeight);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
