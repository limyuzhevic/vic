#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Anti-Hebbian plasticity rule: "neurons that fire together, don't wire together"
// Implements spike-timing dependent depression (STDP reverse)
// Uses activity-dependent depression to prevent runaway excitation
// 
// Biological inspiration:
// - Spike-timing dependent depression (reverse STDP)
// - Prevents runaway excitation in neural circuits
// - Homeostatic synaptic scaling
// - Compensatory mechanisms in biological neurons
// 
// Mathematical formulation:
// Δw = -η * (coactivity) for "fire together, don't wire together"
// 
// Where:
// - η is the depression rate
// - coactivity is the correlation between pre- and post-synaptic activity
// - This implements "neurons that fire together, wire together" but in reverse
// 
// Use cases:
// - Preventing runaway excitation in recurrent networks
// - Implementing homeostatic plasticity
// - Creating balanced neural circuits with both excitation and inhibition
// - Implementing loss modulation in neural systems
// 
// Limitations:
// - Simplified correlation model
// - Single learning rate (no separate potentiation/depression)
// - Assumes stationary statistics

class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    AntiHebbianRule(AntiHebbianRule&&) noexcept;
    ~AntiHebbianRule() override;
    
    AntiHebbianRule& operator=(AntiHebbianRule&&) noexcept;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Anti-Hebbian parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setDepressionRate(float rate);
    float getDepressionRate() const;
    
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    
    // Configure AntiHebbian parameters
    void configure(float learningRate, float depressionRate, float minWeight, float maxWeight);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm