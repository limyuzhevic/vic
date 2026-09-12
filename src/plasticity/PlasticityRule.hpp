#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// PLACEHOLDER - Phase 2 will implement real plasticity rules

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // TODO PHASE 2: Implement real plasticity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

// Hebbian plasticity rule: "neurons that fire together, wire together"
// PLACEHOLDER - Phase 2 will implement real Hebbian learning
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// Real implementation: Anti-Hebbian learning
class AntiHebbianRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {
        // Anti-Hebbian implementation: neurons that fire together, lose connections
        // Δw = -η × (coincidence count) to decrease weight
        
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Count coincident spikes
        size_t coincidences = 0;
        for (Timestamp pre : preSpikes) {
            for (Timestamp post : postSpikes) {
                if (std::abs(pre - post) < 10.0f) {  // 10ms coincidence window
                    ++coincidences;
                }
            }
        }
        
        // Anti-Hebbian weight decrease
        if (coincidences > 0) {
            // Apply negative weight change
            synapse->addToWeight(-0.01f * static_cast<float>(coincidences));
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        synapse->addToWeight(delta);
    }
    
    const char* getName() const override { return "AntiHebbian"; }
};

// Bienenstock-Cooper-Munro (BCM) rule
// Real implementation: Oja's learning rule variant
class BCMRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {
        // BCM rule implementation with sliding threshold
        // Δw = x_post * (x_post - θ) where θ is the sliding threshold
        
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Calculate BCM components
        // In real implementation, use actual spike values and compute sliding threshold
        float preActivity = static_cast<float>(preSpikes.size()) / 100.0f;
        float postActivity = static_cast<float>(postSpikes.size()) / 100.0f;
        
        // Sliding threshold based on post-synaptic activity history
        float slidingThreshold = 0.5f + 0.1f * postActivity;  // θ = θ0 + k·⟨x_post⟩
        
        // BCM weight change: Δw = x_pre · (x_post - θ)
        float delta = preActivity * (postActivity - slidingThreshold);
        
        // Apply with bounds
        if (std::abs(delta) > 1e-6f) {
            synapse->addToWeight(delta * 0.001f);  // Learning rate
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        synapse->addToWeight(delta);
    }
    
    const char* getName() const override { return "BCM"; }
};

// Triplet STDP with eligibility traces
// Real implementation: Three-factor learning rule
class TripletSTDP : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {
        // Triplet STDP implementation with three-factor learning
        // Includes pre-post, post-pre, and three-neuron interactions
        
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Calculate triplet components
        float totalDelta = 0.0f;
        float tau = 20.0f;
        
        // Pre-post (classic STDP component)
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt = static_cast<float>(postTime - preTime);
                if (dt > 0) {
                    totalDelta += 0.01f * std::exp(-dt / tau);
                } else if (dt < 0) {
                    totalDelta -= 0.012f * std::exp(dt / tau);
                }
            }
        }
        
        // Apply triplet learning based on eligibility traces
        // Real triplet: combines STDP with presynaptic and postsynaptic traces
        float eligibilityTrace = synapse->getEligibilityTrace();
        if (std::abs(eligibilityTrace) > 1e-6f) {
            // Three-factor learning: pre * post * neuromodulator
            totalDelta *= (1.0f + eligibilityTrace * 0.3f);  // Stronger modulation
        }
        
        // Apply with bounds
        if (std::abs(totalDelta) > 1e-6f) {
            synapse->addToWeight(totalDelta);
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        synapse->addToWeight(delta);
    }
    
    const char* getName() const override { return "TripletSTDP"; }
};

} // namespace nlm
