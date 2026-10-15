// RewardModulatedSTDP.cpp implementation

#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float rewardGain;                 // Reward modulation gain
    float rewardSignal;               // Current reward signal
    float temporalDifference;         // Temporal difference error
    float accumulationDecay;          // Reward accumulation decay rate
    float eligibilityDecay;            // Eligibility trace decay rate
    bool enableAccumulation;          // Enable reward accumulation
    bool enableEligibilityTrace;      // Enable eligibility traces for reward
    float accumulatedReward;          // Accumulated reward signal
    float eligibilityTrace;            // Eligibility trace for reward-modulated learning
    STDP* baseSTDP;                   // Pointer to base STDP implementation
    
    Impl() : rewardGain(1.0f), rewardSignal(0.0f), temporalDifference(0.0f),
             accumulationDecay(0.95f), eligibilityDecay(0.98f),
             enableAccumulation(true), enableEligibilityTrace(true),
             accumulatedReward(0.0f), eligibilityTrace(0.0f), baseSTDP(nullptr) {}
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
     * 
     * 1. Basic STDP calculation (delegated to base implementation):
     *    Δw_STDP = A+ * exp(-Δt / τ+) for pre-before-post
     *    Δw_STDP = -A- * exp(Δt / τ-) for post-before-pre
     * 
     * 2. Reward modulation:
     *    Δw_R = Δw_STDP * (1 + G * (r + λ * δ))
     *    
     *    Where:
     *      G = rewardGain (modulation strength)
     *      r = rewardSignal (extrinsic reward)
     *      λ = reward sensitivity (baseline)
     *      δ = temporalDifference (prediction error)
     *    
     * 3. Eligibility traces:
     *    e(t) = e(t-1) * e_decay + Σ spike_activity
     *    Δw_final = e(t) * Δw_R
     *    
     * 4. Reward accumulation:
     *    R_acc(t) = R_acc(t-1) * decay + r(t)
     *    
     * Biological inspiration:
     *   - Dopamine neurons encode reward prediction errors
     *   - R-STDP integrates reward signals with spike timing
     *   - Eligibility traces maintain synaptic eligibility for delayed reward
     *   - Uses basal ganglia pathways for reward integration
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Update eligibility trace with spike activity
    updateEligibilityTrace(preSpikes, postSpikes, dt);
    
    // Calculate basic STDP from base implementation if available
    float baseDelta = 0.0f;
    if (pImpl->baseSTDP) {
        // Create a temporary synapse for base STDP calculation
        // Note: This is a simplified approach - in real implementations,
        // the base STDP and R-STDP would share synapse state
        pImpl->baseSTDP->update(synapse, preSpikes, postSpikes, dt);
        baseDelta = synapse->getWeight(); // Get the weight change
        // Reset weight change for next calculation
        synapse->setWeight(synapse->getWeight() - baseDelta);
    }
    
    // Apply reward modulation to base STDP
    float rewardModulatedDelta = calculateRewardModulatedSTDP(baseDelta);
    
    // Apply eligibility trace modulation
    if (pImpl->enableEligibilityTrace) {
        rewardModulatedDelta *= pImpl->eligibilityTrace;
    }
    
    // Apply reward-modulated weight change
    applyWeightChange(synapse, rewardModulatedDelta);
    
    // Update reward accumulation
    updateRewardAccumulation();
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change with bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f); // Standard bounds
    synapse->setWeight(newWeight);
}

void RewardModulatedSTDP::updateEligibilityTrace(const std::vector<Timestamp>& preSpikes,
                                                 const std::vector<Timestamp>& postSpikes,
                                                 TimestepDuration dt) {
    // Update eligibility trace based on spike activity
    float spikeContribution = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_val = static_cast<float>(postTime - preTime);
            if (std::abs(dt_val) < 50.0f) {  // Within 50ms window
                spikeContribution += 1.0f / (1.0f + std::abs(dt_val) / 25.0f);
            }
        }
    }
    
    // Update eligibility trace with decay and new spikes
    pImpl->eligibilityTrace = pImpl->eligibilityTrace * pImpl->eligibilityDecay + spikeContribution;
}

void RewardModulatedSTDP::updateRewardAccumulation() {
    // Simple reward accumulation with decay
    pImpl->accumulatedReward = pImpl->accumulatedReward * pImpl->accumulationDecay + pImpl->rewardSignal;
}

float RewardModulatedSTDP::calculateRewardModulatedSTDP(float baseDelta) {
    // Apply reward modulation to base STDP
    float modulation = 1.0f + pImpl->rewardGain * (pImpl->rewardSignal + pImpl->temporalDifference);
    return baseDelta * modulation;
}

void RewardModulatedSTDP::setRewardGain(float gain) {
    pImpl->rewardGain = std::clamp(gain, 0.0f, 10.0f);
}

float RewardModulatedSTDP::getRewardGain() const {
    return pImpl->rewardGain;
}

void RewardModulatedSTDP::setRewardSignal(float signal) {
    pImpl->rewardSignal = std::clamp(signal, -1.0f, 1.0f);
}

float RewardModulatedSTDP::getRewardSignal() const {
    return pImpl->rewardSignal;
}

void RewardModulatedSTDP::setTemporalDifference(float td) {
    pImpl->temporalDifference = std::clamp(td, -1.0f, 1.0f);
}

float RewardModulatedSTDP::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

void RewardModulatedSTDP::enableAccumulation(bool enable) {
    pImpl->enableAccumulation = enable;
}

void RewardModulatedSTDP::enableEligibilityTrace(bool enable) {
    pImpl->enableEligibilityTrace = enable;
}

void RewardModulatedSTDP::setAccumulationDecay(float decay) {
    pImpl->accumulationDecay = std::clamp(decay, 0.5f, 0.999f);
}

void RewardModulatedSTDP::setEligibilityDecay(float decay) {
    pImpl->eligibilityDecay = std::clamp(decay, 0.5f, 0.999f);
}

bool RewardModulatedSTDP::isAccumulationEnabled() const {
    return pImpl->enableAccumulation;
}

bool RewardModulatedSTDP::isEligibilityTraceEnabled() const {
    return pImpl->enableEligibilityTrace;
}

float RewardModulatedSTDP::getAccumulationDecay() const {
    return pImpl->accumulationDecay;
}

float RewardModulatedSTDP::getEligibilityDecay() const {
    return pImpl->eligibilityDecay;
}

void RewardModulatedSTDP::setBaseSTDP(STDP* stdp) {
    pImpl->baseSTDP = stdp;
}

} // namespace nlm