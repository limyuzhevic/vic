#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float eligibilityDecay;  // How fast eligibility traces decay
    float rewardScale;    // Scaling factor for reward signal
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             eligibilityDecay(0.01f), rewardScale(1.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    /*
     * Reward-Modulated STDP (R-STDP)
     * 
     * Combines spike timing with reward signals for credit assignment
     * 
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+) * eligibility * reward
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = -A- * exp(Δt / τ-) * eligibility * reward
     * 
     * Where:
     *   eligibility = current eligibility trace
     *   reward = reward prediction error signal (positive for better than expected)
     *   If reward is negative (worse than expected), no changes occur
     *   
     * Biological inspiration:
     *   - Combines spike timing with neuromodulatory signals (dopamine)
     *   - Reflects reward prediction error in dopamine system
     *   - Credit assignment for temporally separated events
     *   
     * Applications:
     *   - Reinforcement learning in spiking neural networks
     *   - Credit assignment for delayed rewards
     *   - Value-based learning
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Get current reward prediction error from synapse (dopamine signal)
    float rewardPredictionError = synapse->getWeight() > 0 ? 
        synapse->getWeight() * pImpl->rewardScale : 
        0.0f;  // No reward signal for initially weak synapses
    
    // If reward is negative (worse than expected), don't learn
    if (rewardPredictionError <= 0.0f) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    float eligibility = synapse->getEligibilityTrace();
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // Cells that fire together, wire together when reward received
                float delta = pImpl->ltpWeight * std::exp(-dt / tau) * eligibility * rewardPredictionError;
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // Anti-Hebbian when reward received
                float delta = -pImpl->ltdWeight * std::exp(dt / tau) * eligibility * rewardPredictionError;
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes)
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for future reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
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

void RewardModulatedSTDP::setEligibilityDecay(float decay) {
    pImpl->eligibilityDecay = std::clamp(decay, 0.001f, 0.1f);
}

float RewardModulatedSTDP::getEligibilityDecay() const {
    return pImpl->eligibilityDecay;
}

void RewardModulatedSTDP::setRewardScale(float scale) {
    pImpl->rewardScale = std::clamp(scale, 0.0f, 10.0f);
}

float RewardModulatedSTDP::getRewardScale() const {
    return pImpl->rewardScale;
}

void RewardModulatedSTDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
