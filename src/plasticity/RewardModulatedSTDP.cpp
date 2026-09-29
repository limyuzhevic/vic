#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float ltpWeight;           // LTP amplitude (A+)
    float ltdWeight;           // LTD amplitude (A-)
    float timeConstant;        // STDP time constant (tau)
    float rewardStrength;      // Reward modulation strength (alpha)
    float tDiscount;           // Temporal discount factor (lambda)
    float minWeight;           // Minimum synaptic weight
    float maxWeight;           // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             rewardStrength(1.0f), tDiscount(0.95f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    /*
     * Reward-Modulated STDP Implementation
     * 
     * Mathematical formulation:
     * 1. Standard STDP delta calculation for each pre-post pair:
     *    Δw = A+ * exp(-Δt / τ+) for Δt > 0 (pre before post)
     *    Δw = -A- * exp(Δt / τ-) for Δt < 0 (post before pre)
     * 
     * 2. Eligibility trace update:
     *    E(t) = E(t-1) * λ + Σ STDP(t) * δ(t-t_spike)
     *    Where λ is the temporal discount factor
     * 
     * 3. Reward-modulated weight change:
     *    w(t) = w(t-1) + α * R(t) * E(t)
     *    Where R(t) is the reward prediction error
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
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
    
    // Update eligibility trace with temporal discounting
    if (std::abs(totalDelta) > 1e-6f) {
        float currentTrace = synapse->getEligibilityTrace();
        // Apply temporal decay to existing trace
        currentTrace *= pImpl->tDiscount;
        // Add new STDP contribution
        currentTrace += totalDelta;
        synapse->setEligibilityTrace(currentTrace);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Calculate reward-modulated weight change
    // Scale by eligibility trace and reward strength
    float currentTrace = synapse->getEligibilityTrace();
    float modulatedDelta = delta * pImpl->rewardStrength * currentTrace;
    
    // Apply weight change with bounds
    float newWeight = synapse->getWeight() + modulatedDelta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
    
    // Decay eligibility trace
    synapse->setEligibilityTrace(currentTrace * pImpl->tDiscount);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setRewardStrength(float strength) {
    pImpl->rewardStrength = std::clamp(strength, 0.0f, 10.0f);
}

float RewardModulatedSTDP::getRewardStrength() const {
    return pImpl->rewardStrength;
}

void RewardModulatedSTDP::setTDDiscount(float discount) {
    pImpl->tDiscount = std::clamp(discount, 0.1f, 1.0f);
}

float RewardModulatedSTDP::getTDDiscount() const {
    return pImpl->tDiscount;
}

void RewardModulatedSTDP::resetEligibilityTrace(Synapse* synapse) {
    if (synapse) {
        synapse->setEligibilityTrace(0.0f);
    }
}

float RewardModulatedSTDP::getEligibilityTrace(Synapse* synapse) const {
    if (synapse) {
        return synapse->getEligibilityTrace();
    }
    return 0.0f;
}

void RewardModulatedSTDP::configure(float ltpWeight, float ltdWeight, float tau,
                                   float rewardStrength, float tDiscount) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    setRewardStrength(rewardStrength);
    setTDDiscount(tDiscount);
}

} // namespace nlm
