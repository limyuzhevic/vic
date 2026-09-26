#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float baseLtpWeight;      // LTP weight from base STDP
    float baseLtdWeight;      // LTD weight from base STDP
    float timeConstant;       // STDP time constant (ms)
    float minWeight;          // Minimum synaptic weight
    float maxWeight;          // Maximum synaptic weight
    
    // Reward modulation parameters
    float rewardFactor;       // How strongly reward affects plasticity (0-1)
    float rewardDecayRate;    // Decay rate for eligibility trace
    float minReward;          // Minimum reward signal
    float maxReward;          // Maximum reward signal
    
    // Eligibility trace management
    float eligibilityTrace;   // Accumulated eligibility
    Timestamp lastTraceUpdate; // Last time trace was updated
    
    Impl() : baseLtpWeight(0.01f), baseLtdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), rewardFactor(0.5f), 
             rewardDecayRate(0.1f), minReward(-1.0f), maxReward(1.0f),
             eligibilityTrace(0.0f), lastTraceUpdate(-1.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Compute STDP contributions using the same logic as STDP.cpp
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->baseLtpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -pImpl->baseLtdWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes)
        }
    }
    
    // Update eligibility trace with STDP contributions
    if (std::abs(totalDelta) > 1e-6f) {
        pImpl->eligibilityTrace += totalDelta;
        pImpl->lastTraceUpdate = 0; // Mark for reward update
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply the STDP weight change with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "RewardModulatedSTDP";
}

void RewardModulatedSTDP::setRewardFactor(float factor) {
    pImpl->rewardFactor = std::clamp(factor, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getRewardFactor() const {
    return pImpl->rewardFactor;
}

void RewardModulatedSTDP::setRewardDecayRate(float rate) {
    pImpl->rewardDecayRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getRewardDecayRate() const {
    return pImpl->rewardDecayRate;
}

void RewardModulatedSTDP::updateWithReward(Synapse* synapse, float reward, Timestamp currentTime) {
    if (!synapse) return;
    
    // Clamp reward signal
    reward = std::clamp(reward, pImpl->minReward, pImpl->maxReward);
    
    // Apply reward-modulated weight change
    if (std::abs(pImpl->eligibilityTrace) > 1e-6f && std::abs(reward) > 1e-6f) {
        // Reward-modulated learning: Δw = reward * eligibility * factor
        float delta = reward * pImpl->eligibilityTrace * pImpl->rewardFactor;
        
        // Apply with bounds
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        synapse->setWeight(newWeight);
        
        // Decay eligibility trace
        pImpl->eligibilityTrace *= (1.0f - pImpl->rewardDecayRate);
        
        if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
            pImpl->eligibilityTrace = 0.0f;
        }
    }
}

void RewardModulatedSTDP::setBaseSTDP(float ltpWeight, float ltdWeight, float tau) {
    setBaseSTDP(ltpWeight, ltdWeight, tau);
}

} // namespace nlm