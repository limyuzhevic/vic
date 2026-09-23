// Implementation of RewardModulatedSTDP
// Reward-modulated STDP: combines spike timing with reward signals

#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float ltpWeight;              // LTP weight (positive)
    float ltdWeight;              // LTD weight (negative)
    float timeConstant;           // STDP time constant (ms)
    float rewardScalar;           // Reward modulation strength
    float minWeight;              // Minimum synaptic weight
    float maxWeight;              // Maximum synaptic weight
    float baselineLearningRate;   // Baseline learning rate without reward
    
    // Reward modulation state
    float recentReward;           // Recent reward signal
    float eligibilityTrace;       // Eligibility trace for reward modulation
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             rewardScalar(1.0f), minWeight(-1.0f), maxWeight(1.0f),
             baselineLearningRate(0.0001f), recentReward(0.0f), eligibilityTrace(0.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    /*
     * Reward-Modulated STDP implementation
     * 
     * Mathematical formulation:
     * Δw = (A+ * exp(-Δt/τ+) - A- * exp(Δt/τ-)) * (baseline + rewardScalar * recentReward)
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)
     *   τ+ = τ- = timeConstant (STDP time window)
     *   baseline = baselineLearningRate (unmodulated learning)
     *   rewardScalar = reward modulation strength
     *   recentReward = recent reward signal (0-1)
     *   
     * Biological inspiration:
     *   - Dopamine-mediated reward modulation
     *   - Credit assignment in temporal difference learning
     *   - Reinforcement learning with temporal credit assignment
     *   - combines spike timing with reward prediction error
     *   
     * Key features:
     *   - Baseline STDP when no reward
     *   - Reward amplification when reward received
     *   - Reward suppression when unexpected reward
     *   - Eligibility traces for delayed reward signals
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Calculate baseline STDP contribution
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    // Apply reward modulation
    float rewardModulation = pImpl->baselineLearningRate + 
                           pImpl->rewardScalar * pImpl->recentReward;
    
    // Scale STDP by reward
    totalDelta *= rewardModulation;
    
    // Update eligibility trace
    pImpl->eligibilityTrace += totalDelta * pImpl->recentReward;
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Apply eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + pImpl->eligibilityTrace);
    }
    
    // Decay reward signal
    pImpl->recentReward *= 0.9f;
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
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

void RewardModulatedSTDP::setRewardScalar(float scalar) {
    pImpl->rewardScalar = std::clamp(scalar, 0.0f, 10.0f);
}

float RewardModulatedSTDP::getRewardScalar() const {
    return pImpl->rewardScalar;
}

void RewardModulatedSTDP::setRecentReward(float reward) {
    pImpl->recentReward = std::clamp(reward, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getRecentReward() const {
    return pImpl->recentReward;
}

void RewardModulatedSTDP::setBaselineLearningRate(float rate) {
    pImpl->baselineLearningRate = std::clamp(rate, 0.00001f, 0.01f);
}

float RewardModulatedSTDP::getBaselineLearningRate() const {
    return pImpl->baselineLearningRate;
}

} // namespace nlm