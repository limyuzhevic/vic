#include "RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float rewardModulator; // Reward prediction error
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f), rewardModulator(1.0f) {}
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
     * Extends standard STDP with reward prediction error modulation
     * 
     * Mathematical formulation:
     * Δw = reward * (standard STDP delta)
     * 
     * Where reward is the prediction error signal (dopamine level)
     * 
     * Biological inspiration:
     * - Dopaminergic modulation of synaptic plasticity
     * - Reinforcement learning in neural systems
     * - Dopamine influences synaptic eligibility traces
     * 
     * Key features:
     * - Potentiation only occurs with positive reward
     * - Depression can occur regardless of reward
     * - Synaptic changes are gated by neuromodulatory signals
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate standard STDP weight change
    float baseDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: standard potentiation
                baseDelta += pImpl->ltpWeight * std::exp(-dt / tau);
            } else if (dt < 0) {
                // Post before pre: standard depression
                baseDelta -= pImpl->ltdWeight * std::exp(dt / tau);
            }
        }
    }
    
    // Apply reward modulation
    float rewardModulation = pImpl->rewardModulator;
    
    // R-STDP gating: only potentation is reward-dependent
    if (baseDelta > 0) {
        // Potentiation requires positive reward
        if (rewardModulation > 1.0f) {
            // Strong positive reward enhances potentiation
            float modulatedDelta = baseDelta * rewardModulation;
            synapse->addToWeight(modulatedDelta);
        }
        // Reward == 1.0: normal potentiation
        // Reward < 1.0: reduced or no potentiation
    } else {
        // Depression can occur regardless of reward
        // but is also modulated by reward magnitude
        float modulatedDelta = baseDelta * (0.5f + 0.5f * rewardModulation);
        synapse->addToWeight(modulatedDelta);
    }
    
    // Update eligibility trace for reward-modulated learning
    float currentTrace = synapse->getEligibilityTrace();
    synapse->setEligibilityTrace(currentTrace + baseDelta * rewardModulation);
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::configure(float ltpWeight, float ltdWeight, float tau) {
    pImpl->ltpWeight = std::clamp(ltpWeight, 0.0f, 1.0f);
    pImpl->ltdWeight = std::clamp(ltdWeight, 0.0f, 1.0f);
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

void RewardModulatedSTDP::setRewardModulator(float modulator) {
    pImpl->rewardModulator = std::clamp(modulator, 0.0f, 5.0f);
}

float RewardModulatedSTDP::getRewardModulator() const {
    return pImpl->rewardModulator;
}

} // namespace nlm