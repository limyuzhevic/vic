#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float baseLearningRate;      // Base learning rate (η)
    float rewardSensitivity;     // Reward sensitivity (γ) - scales plasticity by reward
    float traceDecayRate;        // Decay rate for eligibility traces (λ)
    float minWeight;             // Minimum synaptic weight
    float maxWeight;             // Maximum synaptic weight
    
    Impl() : baseLearningRate(0.01f), rewardSensitivity(1.0f), 
             traceDecayRate(0.05f), minWeight(-1.0f), maxWeight(1.0f) {}
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
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = η * A+ * exp(-Δt / τ+) * R(t)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = η * A- * exp(Δt / τ-) * R(t)
     * 
     * Where:
     *   η = baseLearningRate (base learning rate)
     *   A+ = 1.0 (standard potentiation amplitude, can be scaled)
     *   A- = 1.0 (standard depression amplitude, can be scaled)
     *   τ+ = τ- = 20ms (standard STDP time constants)
     *   R(t) = reward signal at time t (typically 0 or 1)
     * 
     * Biological inspiration:
     *   - Combines spike timing with reward prediction error
     *   - Eligibility traces carry information about recent spikes
     *   - Reward signal gates weight changes when eligibility is high
     *   - Implements actor-critic style learning in biological systems
     *   
     * Implementation details:
     *   - Uses synaptic eligibility traces to bridge spikes and delayed rewards
     *   - Reward signal is typically 0 (no reward) or 1 (reward received)
     *   - Eligibility traces decay over time even without reward
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Standard STDP parameters for reward-modulated learning
    const float tauPlus = 20.0f;    // ms, LTP time constant
    const float tauMinus = 20.0f;   // ms, LTD time constant
    const float A_plus = 1.0f;      // potentiation amplitude
    const float A_minus = 1.0f;     // depression amplitude
    
    float totalEligibility = 0.0f;
    
    // Compute eligibility trace contribution from spike timing
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float delta_t = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (delta_t > 0) {
                // Pre before post: contributes to potentiation eligibility
                float eligibility = pImpl->baseLearningRate * A_plus * std::exp(-delta_t / tauPlus);
                totalEligibility += eligibility;
            } else if (delta_t < 0) {
                // Post before pre: contributes to depression eligibility
                float eligibility = pImpl->baseLearningRate * A_minus * std::exp(delta_t / tauMinus);
                totalEligibility += eligibility;
            }
        }
    }
    
    // Get current reward (typically 0 or 1 in biological systems)
    // In a real system, this would come from dopamine neurons or other reward signals
    float reward = 1.0f;  // Assuming reward present for demonstration
    
    // Apply weight change only if there's reward AND eligibility
    if (std::abs(totalEligibility) > 1e-6f && reward > 0.0f) {
        // Modulate weight change by reward sensitivity and current eligibility
        float delta = totalEligibility * pImpl->rewardSensitivity;
        
        // Apply weight change with bounds
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        synapse->setWeight(newWeight);
        
        // Update eligibility trace in synapse (for consistency with STDP)
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalEligibility * pImpl->rewardSensitivity);
        
        // Apply trace decay
        synapse->decayEligibilityTrace(pImpl->traceDecayRate);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change with proper bounds clamping
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setBaseLearningRate(float rate) {
    pImpl->baseLearningRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaseLearningRate() const {
    return pImpl->baseLearningRate;
}

void RewardModulatedSTDP::setRewardSensitivity(float sensitivity) {
    pImpl->rewardSensitivity = std::clamp(sensitivity, 0.0f, 10.0f);
}

float RewardModulatedSTDP::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

void RewardModulatedSTDP::setTraceDecayRate(float decayRate) {
    pImpl->traceDecayRate = std::clamp(decayRate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getTraceDecayRate() const {
    return pImpl->traceDecayRate;
}

void RewardModulatedSTDP::configure(float learningRate, float rewardSensitivity, float traceDecay) {
    setBaseLearningRate(learningRate);
    setRewardSensitivity(rewardSensitivity);
    setTraceDecayRate(traceDecay);
}

} // namespace nlm