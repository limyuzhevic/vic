#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../../neuromodulation/Reward.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float baseLTPWeight;          // Base long-term potentiation weight (A+)
    float baseLTDWeight;          // Base long-term depression weight (A-)
    float rewardModulationFactor; // Reward modulation factor (γ)
    float baseTimeConstant;       // Base STDP time constant (τ)
    float maxWeight;              // Maximum synaptic weight
    float minWeight;              // Minimum synaptic weight
    float currentReward;          // Current reward signal
    float eligibilityTrace;       // Eligibility trace for reward modulation
    
    Impl() : baseLTPWeight(0.01f), baseLTDWeight(0.012f),
             rewardModulationFactor(0.5f), baseTimeConstant(20.0f),
             maxWeight(1.0f), minWeight(-1.0f), currentReward(0.0f),
             eligibilityTrace(0.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    /*
     * Real Reward-Modulated STDP (R-STDP) implementation
     * 
     * Mathematical formulation:
     * Δw = r(t) * [A+ * exp(-Δt/τ+) - A- * exp(Δt/τ-)]
     * 
     * Where:
     *   - r(t) is the reward signal (modulation factor)
     *   - A+ = baseLTPWeight * (1 + γ * r(t)) for potentiation
     *   - A- = baseLTDWeight * (1 - γ * r(t)) for depression
     *   - γ is the reward modulation factor
     *   - τ+ = τ- = baseTimeConstant (time windows)
     *   
     * Biological inspiration:
     *   - Implements dopamine-modulated STDP
     *   - Links reward prediction with synaptic plasticity
     *   - Explains reinforcement learning in biological systems
     *   - Related to neuromodulatory systems (dopamine, acetylcholine)
     *   
     * Limitations:
     *   - Linear reward modulation
     *   - Single reward signal affecting all synapses
     *   - Simplified eligibility trace dynamics
     */
    if (!synapse) return;
    
    float timeConstant = pImpl->baseTimeConstant;
    float totalDelta = 0.0f;
    
    // Compute reward-modulated STDP weight changes
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            float rewardModulatedLTP = pImpl->baseLTPWeight * 
                                      (1.0f + pImpl->rewardModulationFactor * pImpl->currentReward);
            float rewardModulatedLTD = pImpl->baseLTDWeight * 
                                      std::max(0.0f, 1.0f - pImpl->rewardModulationFactor * pImpl->currentReward);
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = rewardModulatedLTP * std::exp(-dt / timeConstant);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -rewardModulatedLTD * std::exp(dt / timeConstant);
                totalDelta += delta;
            }
        }
    }
    
    // Apply eligibility trace for delayed reward
    pImpl->eligibilityTrace += totalDelta;
    
    // Scale by current reward
    float finalDelta = pImpl->eligibilityTrace * pImpl->currentReward;
    
    // Apply with bounds
    if (std::abs(finalDelta) > 1e-6f) {
        applyWeightChange(synapse, finalDelta);
    }
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

void RewardModulatedSTDP::setBaseLTPWeight(float weight) {
    pImpl->baseLTPWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaseLTPWeight() const {
    return pImpl->baseLTPWeight;
}

void RewardModulatedSTDP::setBaseLTDWeight(float weight) {
    pImpl->baseLTDWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaseLTDWeight() const {
    return pImpl->baseLTDWeight;
}

void RewardModulatedSTDP::setRewardModulationFactor(float factor) {
    pImpl->rewardModulationFactor = std::clamp(factor, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getRewardModulationFactor() const {
    return pImpl->rewardModulationFactor;
}

void RewardModulatedSTDP::setBaseTimeConstant(float tau) {
    pImpl->baseTimeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float RewardModulatedSTDP::getBaseTimeConstant() const {
    return pImpl->baseTimeConstant;
}

void RewardModulatedSTDP::setReward(float reward) {
    pImpl->currentReward = std::clamp(reward, -1.0f, 1.0f);
}

float RewardModulatedSTDP::getReward() const {
    return pImpl->currentReward;
}

} // namespace nlm