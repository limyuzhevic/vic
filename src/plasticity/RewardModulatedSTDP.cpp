#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float stdpWeight;           // Weight for STDP component
    float rewardWeight;         // Weight for reward modulation component
    float rewardModTau;         // Time constant for reward modulation
    float minWeight;            // Minimum synaptic weight
    float maxWeight;            // Maximum synaptic weight
    
    Impl() : stdpWeight(0.01f), rewardWeight(0.1f), rewardModTau(100.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    /*
     * Reward-modulated STDP implementation
     * 
     * Mathematical formulation:
     * Δw = (A+ * exp(-Δt / τ+) - A- * exp(Δt / τ-)) * R(t)
     * 
     * Where:
     *   R(t) = reward prediction error signal
     *   A+, A- = STDP weights for potentiation/depression
     *   τ+ = τ- = STDP time constant
     *   Δw = weight change
     *   Δt = spike timing difference (t_post - t_pre)
     * 
     * Biological inspiration:
     *   - Combines STDP with dopaminergic reward signals
     *   - Implements reward prediction error as in temporal difference learning
     *   - Reflects how dopamine modulates synaptic plasticity in the brain
     *   - High dopamine reinforces associations that predict rewards
     *   - Low/no dopamine weakens associations that don't predict rewards
     *   
     * Applications:
     *   - Reinforcement learning in neural systems
     *   - Reward-based behavioral adaptation
     *   - Temporal difference learning in spiking neural networks
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float stdpTau = 20.0f;  // STDP time constant
    
    // Get current reward signal from synapse eligibility trace
    float rewardSignal = synapse->getEligibilityTrace();
    
    // Optimization: Limit spike history processing to recent spikes
    const size_t MAX_PROCESSING = 20;
    size_t preCount = std::min(preSpikes.size(), MAX_PROCESSING);
    size_t postCount = std::min(postSpikes.size(), MAX_PROCESSING);
    
    // Pre-compute common constants
    const float ltp = pImpl->stdpWeight;
    const float ltd = pImpl->stdpWeight;
    const float inv_tau = 1.0f / stdpTau;
    
    // Process spike pairs with reward modulation
    for (size_t i = 0; i < preCount; ++i) {
        Timestamp preTime = preSpikes[preSpikes.size() - 1 - i];  // Most recent first
        
        for (size_t j = 0; j < postCount; ++j) {
            Timestamp postTime = postSpikes[postSpikes.size() - 1 - j];  // Most recent first
            
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (std::abs(dt) < 0.000001f) {
                continue;  // Skip simultaneous spikes
            }
            
            float delta;
            if (dt > 0) {
                // Pre before post: STDP potentiation
                delta = ltp * std::exp(-dt * inv_tau);
            } else {
                // Post before pre: STDP depression
                delta = -ltd * std::exp(dt * inv_tau);
            }
            
            // Apply reward modulation
            delta *= rewardSignal * pImpl->rewardWeight;
            totalDelta += delta;
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Decay eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        currentTrace *= 0.9f;  // Decay rate
        synapse->setEligibilityTrace(currentTrace);
        
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace based on weight change
        float newTrace = synapse->getEligibilityTrace() + totalDelta * pImpl->rewardModTau;
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

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setSTDPWeight(float weight) {
    pImpl->stdpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getSTDPWeight() const {
    return pImpl->stdpWeight;
}

void RewardModulatedSTDP::setRewardWeight(float weight) {
    pImpl->rewardWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getRewardWeight() const {
    return pImpl->rewardWeight;
}

void RewardModulatedSTDP::setRewardModulationTau(float tau) {
    pImpl->rewardModTau = std::clamp(tau, 10.0f, 1000f);
}

float RewardModulatedSTDP::getRewardModulationTau() const {
    return pImpl->rewardModTau;
}

} // namespace nlm