#pragma once

#include "../plasticity/PlasticityRule.hpp"

namespace nlm {

// Bayesian Cognitive Model (BCM) plasticity rule
// Implements stability-plasticity tradeoff with adaptive threshold mechanism
// BCM implements activity-dependent synaptic learning where plasticity occurs only when
// post-synaptic activity exceeds a sliding threshold parameter (θ_b), maintaining network stability.

class BCM : public PlasticityRule {
public:
    BCM();
    ~BCM() override;
    
    // Update synaptic weights based on BCM learning rule
    // BCM rule: Δw = η * x_pre * (x_post - θ_b) * learning_gate(x_post)
    // where θ_b is adaptive threshold and learning_gate controls plasticity onset
    void update(Synapse* synapse, 
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    
    // Apply weight change with BCM constraints
    // Applies BCM learning rule with activity-dependent plasticity modulation
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    // Get rule name
    const char* getName() const override;
    
    // Set BCM learning rate
    void setLearningRate(float rate);
    
    // Get current learning rate
    float getLearningRate() const;
    
    // Set BCM threshold parameter (θ_b)
    // Higher threshold reduces plasticity onset for given post-synaptic activity
    void setBLayer(float b);
    
    // Get current BCM threshold parameter
    float getBLayer() const;
    
    // Set learning threshold for plasticity activation
    // Post-synaptic activity must exceed this to enable plasticity
    void setLearningThreshold(float threshold);
    
    // Get current learning threshold
    float getLearningThreshold() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm