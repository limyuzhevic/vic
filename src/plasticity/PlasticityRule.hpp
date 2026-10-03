#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules

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
class AntiHebbianRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Count coincident spikes
        size_t coincidences = 0;
        for (Timestamp pre : preSpikes) {
            for (Timestamp post : postSpikes) {
                if (std::abs(pre - post) < 10.0) {  // 10ms window
                    ++coincidences;
                }
            }
        }
        
        // Apply negative weight change proportional to coincidences
        if (coincidences > 0) {
            applyWeightChange(synapse, -0.01f * static_cast<float>(coincidences));
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        synapse->addToWeight(delta);
    }
    
    const char* getName() const override { return "AntiHebbian"; }
};

// Bienenstock-Cooper-Munro (BCM) rule
class BCMRule : public PlasticityRule {
public:
    BCMRule() : theta(25.0f), sigma(10.0f), learningRate(0.001f) {}
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Calculate postsynaptic activity (simplified)
        float postActivity = std::min(1.0f, static_cast<float>(postSpikes.size()) * 0.1f);
        
        // BCM learning rule: Δw = σ^2 * (postActivity - θ) * preActivity
        // Simplified: weight change proportional to postActivity
        float weightChange = sigma * sigma * (postActivity - theta) * 0.01f;
        
        if (std::abs(weightChange) > 1e-6f) {
            applyWeightChange(synapse, weightChange);
        }
        
        // Update sliding threshold
        theta = theta + learningRate * (postActivity - theta);
        theta = std::max(0.0f, std::min(theta, 50.0f));  // Keep theta reasonable
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        synapse->addToWeight(delta);
    }
    
    const char* getName() const override { return "BCM"; }
    
    void setTheta(float t) { theta = t; }
    float getTheta() const { return theta; }
    
    void setSigma(float s) { sigma = s; }
    float getSigma() const { return sigma; }
    
    void setLearningRate(float lr) { learningRate = lr; }
    float getLearningRate() const { return learningRate; }
    
private:
    float theta;      // Sliding threshold
    float sigma;      // Learning rate parameter
    float learningRate; // Rate of theta adaptation
};

} // namespace nlm
