// PlasticityRule.hpp - Standardized plasticity rule base class
// 
// This header provides a consistent interface for all plasticity rules
// with standardized parameters, methods, and behavior patterns.

#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Standard plasticity rule parameters for consistent configuration
struct PlasticityParameters {
    float learningRate;      // Base learning rate (0.0-1.0)
    float minWeight;         // Minimum synaptic weight
    float maxWeight;         // Maximum synaptic weight
    float timeConstant;      // Time constant for dynamics (ms)
    bool enabled;            // Whether rule is active
    
    PlasticityParameters()
        : learningRate(0.01f)
        , minWeight(-1.0f)
        , maxWeight(1.0f)
        , timeConstant(20.0f)
        , enabled(true) {}
};

// Enhanced base class for plasticity rules
// 
// This provides standardized behavior patterns while allowing
// rule-specific implementations for the update logic.
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // === Core plasticity interface ===
    
    // Update synaptic weights based on pre/post synaptic activity
    virtual void update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply direct weight change (for reward-modulated learning)
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule identification
    virtual const char* getName() const = 0;
    
    // === Standardized parameter interface ===
    
    // Enable/disable the plasticity rule
    virtual void setEnabled(bool enabled) = 0;
    bool isEnabled() const { return enabled_; }
    
    // Get parameters
    const PlasticityParameters& getParameters() const { return params_; }
    void setParameters(const PlasticityParameters& params) {
        params_ = params;
        onParametersChanged();
    }
    
    // === Common plasticity methods ===
    
    // Configure from individual parameters (convenience method)
    virtual void configure(float learningRate = 0.01f,
                           float minWeight = -1.0f,
                           float maxWeight = 1.0f,
                           float timeConstant = 20.0f) {
        PlasticityParameters p;
        p.learningRate = learningRate;
        p.minWeight = minWeight;
        p.maxWeight = maxWeight;
        p.timeConstant = timeConstant;
        p.enabled = true;
        setParameters(p);
    }
    
    // Safe weight clamping with rule-specific bounds
    float clampWeight(SynapticWeight weight) const {
        return std::clamp(weight, params_.minWeight, params_.maxWeight);
    }
    
    // Compute weight change with safety checks
    float computeSafeDelta(SynapticWeight delta) const {
        return std::abs(delta) > 1e-6f ? delta : 0.0f;
    }
    
    // Apply eligibility trace update
    void updateEligibilityTrace(Synapse* synapse, SynapticWeight delta) {
        if (!synapse) return;
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + delta);
    }
    
    // Get synaptic efficacy factor (for modulation)
    float getSynapticEfficacy(Synapse* synapse) const {
        return synapse ? synapse->getEfficacy() : 1.0f;
    }
    
    // === Statistics and monitoring ===
    
    virtual void resetStatistics() {
        learningEvents_ = 0;
        lastUpdateTime_ = 0.0;
    }
    
    size_t getLearningEvents() const { return learningEvents_; }
    float getLastUpdateTime() const { return lastUpdateTime_; }
    
protected:
    PlasticityRule() : enabled_(true) {}
    
    // Rule-specific initialization
    virtual void onParametersChanged() {}
    
    // Increment learning statistics
    void recordLearningEvent(float delta, float currentTime) {
        if (std::abs(delta) > 1e-6f) {
            learningEvents_++;
            lastUpdateTime_ = currentTime;
        }
    }
    
    // Rule-specific update implementation
    virtual void onUpdate(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) = 0;
    
    // Rule-specific weight change implementation
    virtual void onApplyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
private:
    PlasticityParameters params_;
    bool enabled_;
    size_t learningEvents_;
    float lastUpdateTime_;
};

// === Convenience factory functions ===

// Create STDP rule with biological parameters
inline std::unique_ptr<PlasticityRule> createSTDP(float ltpWeight = 0.01f,
                                                  float ltdWeight = 0.012f,
                                                  float timeConstant = 20.0f) {
    // STDP implementation would be here
    return nullptr;
}

// Create Hebbian rule with covariance-based learning
inline std::unique_ptr<PlasticityRule> createHebbian(float learningRate = 0.01f,
                                                     float correlationWindow = 100.0f) {
    // Hebbian implementation would be here  
    return nullptr;
}

// Create reward-modulated STDP
inline std::unique_ptr<PlasticityRule> createRewardModulatedSTDP(
    float ltpWeight = 0.01f,
    float ltdWeight = 0.012f,
    float timeConstant = 20.0f,
    float rewardWeight = 0.5f) {
    // Reward-modulated STDP implementation would be here
    return nullptr;
}

} // namespace nlm