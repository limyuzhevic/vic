#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    float learningRate;
    
    Impl() : learningRate(0.01f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // TODO PHASE 2: Implement real Hebbian learning
    // PLACEHOLDER: Simple correlated firing increases weight
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count coincident spikes (simplified)
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms window
                ++coincidences;
            }
        }
    }
    
    // Apply weight change proportional to coincidences
    if (coincidences > 0) {
        applyWeightChange(synapse, pImpl->learningRate * static_cast<float>(coincidences));
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// Add custom plasticity rule support
struct CustomPlasticityRule::Impl {
    std::string name;
    PlasticityType type;
    PlasticityFunction plasticFunc;
    bool enabled;
    
    Impl(const std::string& ruleName, PlasticityType ruleType, PlasticityFunction func)
        : name(ruleName), type(ruleType), plasticFunc(func), enabled(true) {
    }
};

CustomPlasticityRule::CustomPlasticityRule(const std::string& name, PlasticityType type, PlasticityFunction func)
    : pImpl(new Impl(name, type, func)) {
}

CustomPlasticityRule::~CustomPlasticityRule() = default;

bool CustomPlasticityRule::isEnabled() const {
    return pImpl->enabled;
}

void CustomPlasticityRule::setEnabled(bool enabled) {
    pImpl->enabled = enabled;
}

const char* CustomPlasticityRule::getName() const {
    return pImpl->name.c_str();
}

PlasticityType CustomPlasticityRule::getType() const {
    return pImpl->type;
}

void CustomPlasticityRule::update(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) {
    if (!pImpl->enabled) return;
    
    if (preSpikes.empty() || postSpikes.empty() || !pImpl->plasticFunc) {
        return;
    }
    
    // For custom plasticity rules, we use the plasticity function directly
    // Simplified approach using first/last spikes
    float preTrace = preSpikes.empty() ? 0.0f : 1.0f;
    float postTrace = postSpikes.empty() ? 0.0f : 1.0f;
    
    float delta = pImpl->plasticFunc(*synapse, *synapse, preTrace, postTrace);
    applyWeightChange(synapse, delta);
}

void CustomPlasticityRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
}

void CustomPlasticityRule::setPlasticityFunction(PlasticityFunction func) {
    pImpl->plasticFunc = func;
}

std::string CustomPlasticityRule::getTypeName() const {
    return "CustomPlasticityRule";
}

// Factory functions for common custom plasticity rules
namespace PlasticityFactories {
    std::shared_ptr<PlasticityRule> createBoundedSTDP() {
        auto rule = std::make_shared<CustomPlasticityRule>(
            "BoundedSTDP",
            PlasticityType::Custom,
            [](const Synapse& pre, const Synapse& post, float preTrace, float postTrace) {
                // Simple bounded STDP implementation
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
        return rule;
    }
    
    std::shared_ptr<PlasticityRule> createExponentialSTDP() {
        auto rule = std::make_shared<CustomPlasticityRule>(
            "ExponentialSTDP",
            PlasticityType::Custom,
            [](const Synapse& pre, const Synapse& post, float preTrace, float postTrace) {
                // Exponential STDP with decay
                float ltp = 0.1f * preTrace * postTrace * std::exp(-std::abs(postTrace - preTrace));
                float ltd = 0.12f * postTrace * preTrace * std::exp(-std::abs(preTrace - postTrace));
                return ltp - ltd;
            }
        );
        return rule;
    }
    
    std::shared_ptr<PlasticityRule> createRewardModulatedHebbian(float rewardScale) {
        auto rule = std::make_shared<CustomPlasticityRule>(
            "RewardModulatedHebbian",
            PlasticityType::Custom,
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
        return rule;
    }
    
    std::shared_ptr<PlasticityRule> createHomeostaticPlasticity(float targetRate) {
        auto rule = std::make_shared<CustomPlasticityRule>(
            "HomeostaticPlasticity",
            PlasticityType::Custom,
            [targetRate](const Synapse& pre, const Synapse& post, float globalError, float timeStep) {
                // Homeostatic weight adjustment based on global error
                float delta = globalError * targetRate * timeStep;
                
                // Clamp to reasonable range
                delta = std::max(-0.01f, std::min(delta, 0.01f));
                return delta;
            }
        );
        return rule;
    }
}

// Example custom plasticity rule for power users
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
