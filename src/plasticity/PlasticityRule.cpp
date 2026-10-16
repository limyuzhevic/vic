#include "PlasticityRule.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

// Forward declarations for forward references
class RandomGenerator;

class Reward;

// Real implementation of Hebbian learning with spike-timing dependent plasticity (STDP)
void HebbianRule::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse) {
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Implement proper STDP learning window
    // LTP (Long-Term Potentiation): post spike before pre spike within ~20ms
    // LTD (Long-Term Depression): pre spike before post spike within ~20ms
    
    float weightChange = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float timeDiff = postTime - preTime;
            
            if (std::abs(timeDiff) < 20.0) {
                if (timeDiff > 0) {
                    // Post before pre: LTP
                    float normalizedTimeDiff = timeDiff / 20.0f;
                    weightChange += pImpl->learningRate * (1.0f - normalizedTimeDiff);
                } else {
                    // Pre before post: LTD
                    float normalizedTimeDiff = (-timeDiff) / 20.0f;
                    weightChange -= pImpl->learningRate * (1.0f - normalizedTimeDiff);
                }
            }
        }
    }
    
    // Apply weight change with bounds checking
    if (weightChange != 0.0f) {
        applyWeightChange(synapse, weightChange);
        
        // Update eligibility trace for reward-modulated STDP
        float eligibilityTrace = synapse->getEligibilityTrace();
        eligibilityTrace += weightChange * pImpl->learningRate;
        synapse->setEligibilityTrace(eligibilityTrace);
        
        // Decay eligibility trace
        synapse->decayEligibilityTrace(pImpl->learningRate * 0.1f);
    }
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply biological constraints on synaptic weight
    const float minWeight = 0.0f;
    const float maxWeight = 2.0f; // Maximum synaptic strength
    const float saturationThreshold = maxWeight * 0.95f; // Start saturation before reaching max
    
    // Apply soft saturation for high weights
    if (newWeight > saturationThreshold) {
        newWeight = saturationThreshold + std::log(newWeight - saturationThreshold + 1.0f);
    } else if (newWeight < -saturationThreshold) {
        newWeight = -saturationThreshold - std::log(-newWeight - saturationThreshold + 1.0f);
    }
    
    newWeight = std::max(minWeight, std::min(newWeight, maxWeight));
    
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian-STDP";
}

void HebbianRule::setLearningRate(float rate) {
    // Validate learning rate
    pImpl->learningRate = std::max(0.001f, std::min(1.0f, rate));
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// Implementation of Reward-modulated plasticity
void RewardModulatedHebbianRule::update(Synapse* synapse,
                                        const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes,
                                        TimestepDuration dt) {
    if (!synapse || !reward_) {
        return;
    }
    
    // Get eligibility trace from synapse
    float eligibility = synapse->getEligibilityTrace();
    
    // Compute reward prediction error
    float reward = reward_->getValue();
    float predictionError = reward - lastPredictionError_;
    
    // Reward-modulated STDP: weight change = eligibility * prediction error
    float weightChange = eligibility * predictionError;
    
    // Apply weight change with biological constraints
    if (weightChange != 0.0f) {
        float currentWeight = synapse->getWeight();
        float newWeight = currentWeight + weightChange;
        
        // Apply weight bounds
        const float minWeight = 0.0f;
        const float maxWeight = 2.0f;
        newWeight = std::max(minWeight, std::min(newWeight, maxWeight));
        
        synapse->setWeight(newWeight);
        
        // Decay eligibility trace
        synapse->decayEligibilityTrace(dt * 0.01f);
    }
    
    lastPredictionError_ = reward;
}

float RewardModulatedHebbianRule::getName() const {
    return "Reward-Modulated Hebbian";
}

void RewardModulatedHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.001f, std::min(1.0f, rate));
}

float RewardModulatedHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedHebbianRule::setReward(Reward* reward) {
    reward_ = reward;
}

float RewardModulatedHebbianRule::getPredictionError() const {
    return lastPredictionError_;
}

void RewardModulatedHebbianRule::reset() {
    lastPredictionError_ = 0.0f;
    if (reward_) {
        reward_->reset();
    }
}

// Implementation of Oja's rule for normalization in Hebbian learning
void OjaHebbianRule::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    
    // Oja's learning rule: Δw = η * (y * x - y^2 * w)
    // where x is pre-synaptic activity, y is post-synaptic activity
    
    // Simplified implementation for synapse
    float preSynapticActivity = 1.0f; // Placeholder
    float postSynapticActivity = currentWeight; // Simplified
    
    float delta = pImpl->learningRate * (postSynapticActivity * preSynapticActivity - 
                                        postSynapticActivity * postSynapticActivity * currentWeight);
    
    float newWeight = currentWeight + delta;
    
    // Apply weight constraints
    newWeight = std::max(0.0f, std::min(newWeight, 2.0f));
    synapse->setWeight(newWeight);
}

const char* OjaHebbianRule::getName() const {
    return "Oja-Hebbian";
}

void OjaHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.001f, std::min(1.0f, rate));
}

float OjaHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

// Generic plasticity rule with parameter validation
bool PlasticityRule::validateParameters() const {
    return true; // Subclasses should override with specific validation
}

void PlasticityRule::setLearningRate(float rate) {
    learningRate_ = rate;
}

float PlasticityRule::getLearningRate() const {
    return learningRate_;
}

void PlasticityRule::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    if (!isEnabled()) return;
    if (!synapse) return;
    if (dt <= 0.0) return;
    
    if (!validateParameters()) {
        return; // Skip update if parameters are invalid
    }
    
    // Default implementation - subclasses should override
}

} // namespace nlm
