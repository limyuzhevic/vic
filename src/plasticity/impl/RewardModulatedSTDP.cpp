#include "STDP.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float learningRate;      // Base learning rate
    float discountFactor;    // Reward discount factor
    float traceDecay;        // Eligibility trace decay rate
    float reward;            // Current reward signal
    
    Impl() : learningRate(0.01f), discountFactor(0.95f), traceDecay(0.01f), reward(0.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                  const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) {
    /*
     * Reward-modulated STDP (R-STDP) implementation
     * 
     * Mathematical formulation:
     * Combines standard STDP with reward prediction error
     * 
     * Δw = η * δ_t * eligibility_trace
     * 
     * Where:
     *   η = learningRate (base learning rate)
     *   δ_t = reward prediction error (current reward)
     *   eligibility_trace = running trace of synaptic activity
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Update eligibility trace based on spike timing
    // For each pre-post spike pair, add to trace
    float eligibility = 0.0f;
    float decay = pImpl->traceDecay;
    
    // Use nearest-neighbor for efficiency (same as standard STDP)
    float dt = static_cast<float>(postSpikes.back() - preSpikes.back());
    
    if (dt > 0) {
        // Pre before post: positive eligibility contribution
        eligibility += 1.0f;
    } else if (dt < 0) {
        // Post before pre: negative eligibility contribution
        eligibility -= 1.0f;
    }
    
    // Apply decay
    eligibility *= (1.0f - decay * dt);
    
    // Get current eligibility trace from synapse (existing trace)
    float currentTrace = synapse->getEligibilityTrace();
    
    // Update eligibility trace with new eligibility contribution
    synapse->setEligibilityTrace(currentTrace + eligibility);
    
    // Apply weight change based on reward
    float weightChange = pImpl->learningRate * pImpl->reward * synapse->getEligibilityTrace();
    
    if (std::abs(weightChange) > 1e-6f) {
        // Scale by synaptic efficacy
        float efficacy = synapse->getEfficacy();
        weightChange *= efficacy;
        
        // Apply weight change using parent's method
        applyWeightChange(synapse, weightChange);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to reasonable bounds for R-STDP
    float newWeight = synapse->getWeight() + delta;
    
    // R-STDP typically produces smaller weight changes
    float minWeight = -0.5f;
    float maxWeight = 0.5f;
    
    newWeight = std::clamp(newWeight, minWeight, maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, rate);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setDiscountFactor(float factor) {
    pImpl->discountFactor = std::clamp(factor, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getDiscountFactor() const {
    return pImpl->discountFactor;
}

void RewardModulatedSTDP::setTraceDecay(float decay) {
    pImpl->traceDecay = std::clamp(decay, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getTraceDecay() const {
    return pImpl->traceDecay;
}

void RewardModulatedSTDP::applyReward(float reward) {
    pImpl->reward = reward;
}

float RewardModulatedSTDP::getReward() const {
    return pImpl->reward;
}

void RewardModulatedSTDP::resetReward() {
    pImpl->reward = 0.0f;
}

} // namespace nlm