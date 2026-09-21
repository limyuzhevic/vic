#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float learningRate;   // Reward-modulated learning rate
    float traceDecayRate; // Eligibility trace decay rate
    float rewardSensitivity; // How sensitive to reward signals
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             learningRate(1.0f), traceDecayRate(0.1f), rewardSensitivity(1.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    /*
     * Reward-Modulated STDP (R-STDP) implementation
     * 
     * Extends classic STDP with eligibility traces that are gated by reward signals
     * 
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+) * E(t)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = -A- * exp(Δt / τ-) * E(t)
     *   
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)
     *   τ+ = τ- = timeConstant (STDP time window)
     *   E(t) = eligibility trace at time t
     * 
     * Biological inspiration:
     *   - Combines spike timing with reward prediction error
     *   - Implements temporal difference learning in synapses
     *   - Enables credit assignment across time
     *   
     * How reward modulates learning:
     *   - Eligibility traces build when neurons fire
     *   - Reward signals multiply the eligibility trace
     *   - Product determines synaptic weight changes
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Get current eligibility trace
    float eligibilityTrace = synapse->getEligibilityTrace();
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->ltpWeight * std::exp(-dt / tau) * eligibilityTrace;
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -pImpl->ltdWeight * std::exp(dt / tau) * eligibilityTrace;
                totalDelta += delta;
            }
        }
    }
    
    // Apply weight change with reward modulation
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by reward sensitivity and learning rate
        float rewardEffect = getRecentReward() * pImpl->rewardSensitivity;
        totalDelta *= pImpl->learningRate * rewardEffect;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for next time step
        float newTrace = eligibilityTrace + totalDelta * dt * 0.01f; // dt * small decay factor
        synapse->setEligibilityTrace(newTrace);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

void RewardModulatedSTDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void RewardModulatedSTDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void RewardModulatedSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float RewardModulatedSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 10.0f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setTraceDecayRate(float decay) {
    pImpl->traceDecayRate = std::clamp(decay, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getTraceDecayRate() const {
    return pImpl->traceDecayRate;
}

void RewardModulatedSTDP::setRewardSensitivity(float sens) {
    pImpl->rewardSensitivity = std::clamp(sens, 0.0f, 5.0f);
}

float RewardModulatedSTDP::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

void RewardModulatedSTDP::configure(float ltpWeight, float ltdWeight, float tau, 
                                   float rate, float decay, float sens) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    setLearningRate(rate);
    setTraceDecayRate(decay);
    setRewardSensitivity(sens);
}

void RewardModulatedSTDP::onRewardReceived(float reward) {
    // Store recent reward for modulation
    recentReward_ = reward;
    
    // For small rewards, decay the eligibility trace (negative prediction error)
    if (reward < 0.1f) {
        // Negative reward predicts future reward should be higher
        // Reset eligibility trace to avoid learning on unexpected negative outcomes
    }
}

float RewardModulatedSTDP::getRecentReward() const {
    return recentReward_;
}

} // namespace nlm