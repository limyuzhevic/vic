#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include <string>
#include <functional>

namespace nlm {

// Plasticity types
enum class PlasticityType {
    Hebbian,
    STDP,
    AntiHebbian,
    BCM,
    Custom,
    RewardModulated,
    Homeostatic,
    Adaptive,
    Unknown
};

// Forward declaration for custom plasticity function type
class Synapse;
using PlasticityFunction = std::function<float(const Synapse&, const Synapse&, float, float)>;

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
    
    // Get rule type
    virtual PlasticityType getType() const { return PlasticityType::Unknown; }
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // Get type name for display
    virtual std::string getTypeName() const { return "PlasticityRule"; }
    
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

// Custom plasticity rule for advanced users
class CustomPlasticityRule : public PlasticityRule {
public:
    CustomPlasticityRule(const std::string& name, PlasticityType type, PlasticityFunction func);
    ~CustomPlasticityRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    PlasticityType getType() const override;
    std::string getTypeName() const override;
    
    // Set custom plasticity function
    void setPlasticityFunction(PlasticityFunction func);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// PLACEHOLDER - Phase 2
class AntiHebbianRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "AntiHebbian"; }
    PlasticityType getType() const override { return PlasticityType::AntiHebbian; }
};

// Bienenstock-Cooper-Munro (BCM) rule
// PLACEHOLDER - Phase 2
class BCMRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    const char* getName() const override { return "BCM"; }
    PlasticityType getType() const override { return PlasticityType::BCM; }
};

// Factory functions for common plasticity rules
namespace PlasticityFactories {
    std::shared_ptr<PlasticityRule> createHebbian() {
        return std::make_shared<HebbianRule>();
    }
    
    std::shared_ptr<PlasticityRule> createBoundedSTDP() {
        return std::make_shared<CustomPlasticityRule>(
            "BoundedSTDP",
            PlasticityType::Custom,
            [](const Synapse& pre, const Synapse& post, float preTrace, float postTrace) {
                float delta = 0.0f;
                if (postTrace > preTrace) {
                    // LTP
                    delta = 0.01f * postTrace * preTrace;
                    // Clamp to prevent runaway
                    delta = std::min(delta, 0.05f);
                } else {
                    // LTD
                    delta = -0.012f * postTrace * preTrace;
                    // Clamp to prevent runaway
                    delta = std::max(delta, -0.05f);
                }
                return delta;
            }
        );
    }
    
    std::shared_ptr<PlasticityRule> createExponentialSTDP() {
        return std::make_shared<CustomPlasticityRule>(
            "ExponentialSTDP",
            PlasticityType::Custom,
            [](const Synapse& pre, const Synapse& post, float preTrace, float postTrace) {
                // Exponential STDP with decay
                float ltp = 0.1f * preTrace * postTrace * std::exp(-std::abs(postTrace - preTrace));
                float ltd = 0.12f * postTrace * preTrace * std::exp(-std::abs(preTrace - postTrace));
                return ltp - ltd;
            }
        );
    }
    
    std::shared_ptr<PlasticityRule> createRewardModulatedHebbian(float rewardScale = 0.5f) {
        return std::make_shared<CustomPlasticityRule>(
            "RewardModulatedHebbian",
            PlasticityType::RewardModulated,
            [rewardScale](const Synapse& pre, const Synapse& post, float preTrace, float postTrace) {
                // Hebbian learning modulated by reward signal
                float delta = 0.01f * preTrace * postTrace;
                
                // Get reward signal from synapse (placeholder implementation)
                float reward = pre.getRewardSignal();
                
                // Apply reward modulation
                delta = delta * (0.01f + rewardScale * reward);
                
                // Clamp delta
                delta = std::max(-0.1f, std::min(delta, 0.1f));
                return delta;
            }
        );
    }
    
    std::shared_ptr<PlasticityRule> createHomeostaticPlasticity(float targetRate = 0.02f) {
        return std::make_shared<CustomPlasticityRule>(
            "HomeostaticPlasticity",
            PlasticityType::Homeostatic,
            [targetRate](const Synapse& pre, const Synapse& post, float globalError, float timeStep) {
                // Homeostatic weight adjustment based on global error
                float delta = globalError * targetRate * timeStep;
                
                // Clamp to reasonable range
                delta = std::max(-0.01f, std::min(delta, 0.01f));
                return delta;
            }
        );
    }
    
    // Advanced plasticity rule for expert users
    std::shared_ptr<PlasticityRule> createAdaptiveSTDP(float maxLTP = 0.02f, float maxLTD = 0.02f, float timeConstant = 100.0f) {
        return std::make_shared<CustomPlasticityRule>(
            "AdaptiveSTDP",
            PlasticityType::Adaptive,
            [maxLTP, maxLTD, timeConstant](const Synapse& pre, const Synapse& post, float preTime, float postTime) {
                // Calculate spike timing difference
                float dt = postTime - preTime;
                
                float delta;
                if (dt > 0) {
                    // Post after pre = LTP
                    delta = maxLTP * std::exp(-dt / timeConstant_);
                } else {
                    // Pre after post = LTD
                    delta = -maxLTD * std::exp(dt / timeConstant_);
                }
                
                return delta;
            }
        );
    }
}

// Advanced plasticity namespaces for power users
namespace AdvancedPlasticities {
    // Spike timing dependent plasticity with adaptive thresholds
    class AdaptiveSTDP {
    public:
        AdaptiveSTDP(float maxLTP = 0.02f, float maxLTD = 0.02f, float timeConstant = 100.0f)
            : maxLTP_(maxLTP), maxLTD_(maxLTD), timeConstant_(timeConstant) {
        }
        
        float compute(const Synapse& pre, const Synapse& post, float preTime, float postTime) {
            // Calculate spike timing difference
            float dt = postTime - preTime;
            
            float delta;
            if (dt > 0) {
                // Post after pre = LTP
                delta = maxLTP_ * std::exp(-dt / timeConstant_);
            } else {
                // Pre after post = LTD
                delta = -maxLTD_ * std::exp(dt / timeConstant_);
            }
            
            return delta;
        }
        
    private:
        float maxLTP_;
        float maxLTD_;
        float timeConstant_;
    };
    
    // Reward-modulated plasticity with eligibility traces
    class RewardModulatedPlasticity {
    public:
        RewardModulatedPlasticity(float learningRate = 0.001f, float traceDecay = 0.95f)
            : learningRate_(learningRate), traceDecay_(traceDecay) {
        }
        
        float compute(const Synapse& pre, const Synapse& post, float preTrace, float postTrace, float reward) {
            // Update eligibility traces
            float newPreTrace = preTrace * traceDecay_ + 1.0f;
            float newPostTrace = postTrace * traceDecay_ + 1.0f;
            
            // Compute weight change based on reward
            float delta = learningRate_ * newPreTrace * newPostTrace * reward;
            
            // Apply modulation based on trace difference
            float modulation = 1.0f + 0.5f * std::tanh(newPostTrace - newPreTrace);
            delta *= modulation;
            
            return delta;
        }
        
    private:
        float learningRate_;
        float traceDecay_;
    };
}

} // namespace nlm

