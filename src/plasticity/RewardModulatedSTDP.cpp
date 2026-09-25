#include "../../plasticity/STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float baseLTPWeight;      // Base long-term potentiation weight
    float baseLTDWeight;      // Base long-term depression weight
    float timeConstant;       // STDP time constant (ms)
    float rewardModulator;    // Reward modulation factor
    float eligibilityTrace;   // Eligibility trace for reward modulation
    
    Impl() : baseLTPWeight(0.01f), baseLTDWeight(0.012f), timeConstant(20.0f),
             rewardModulator(1.0f), eligibilityTrace(0.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    // Reward-Modulated STDP (R-STDP)
    // Combines spike-timing dependent plasticity with reward signals
    //
    // Mathematical formulation:
    // Δw = F_reward(r) * STDP(Δt)
    //
    // Where:
    //   F_reward(r) = reward function of received reward
    //   STDP(Δt) = standard spike-timing dependent plasticity
    //
    // Biological inspiration:
    //   - Dopamine-mediated reward prediction errors
    //   - Reinforcement learning in cortico-striatal circuits
    //   - Reward timing precision in learning
    //   - Combines temporal and motivational signals
    //
    // Limitations:
    //   - Simplified reward function
    //   - Assumes reward is available at synapse
    //   - Doesn't account for exploration vs exploitation
    //
    
    if (preSpikes.empty() || postSpikes.empty() || !synapse) {
        return;
    }
    
    // Calculate base STDP weight change
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->baseLTPWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -pImpl->baseLTDWeight * std::exp(dt / tau);  // dt is negative
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes)
        }
    }
    
    // Modulate by reward signal
    float rewardModulation = pImpl->rewardModulator;
    
    // Update eligibility trace
    pImpl->eligibilityTrace += totalDelta * dt;
    
    // Apply reward-modulated weight change
    float finalDelta = totalDelta * rewardModulation + pImpl->eligibilityTrace;
    
    // Apply weight change
    applyWeightChange(synapse, finalDelta);
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // R-STDP typically operates within standard bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setRewardModulator(float modulator) {
    pImpl->rewardModulator = std::clamp(modulator, 0.0f, 10.0f);
}

float RewardModulatedSTDP::getRewardModulator() const {
    return pImpl->rewardModulator;
}

void RewardModulatedSTDP::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

float RewardModulatedSTDP::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void RewardModulatedSTDP::configure(float ltpWeight, float ltdWeight, float tau) {
    pImpl->baseLTPWeight = std::clamp(ltpWeight, 0.0f, 0.1f);
    pImpl->baseLTDWeight = std::clamp(ltdWeight, 0.0f, 0.1f);
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

} // namespace nlm
