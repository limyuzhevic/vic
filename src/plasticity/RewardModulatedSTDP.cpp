#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float rewardWeight;      // Weight of reward signal on plasticity
    float traceDecayRate;    // Decay rate of eligibility trace (per ms)
    float maxTrace;          // Maximum eligibility trace value
    
    Impl() : rewardWeight(0.1f), traceDecayRate(0.01f), maxTrace(1.0f) {}
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
    
    // Compute standard STDP eligibility trace
    float eligibility = 0.0f;
    float tau = 20.0f;  // STDP time constant
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: positive eligibility
                eligibility += std::exp(-dt / tau);
            } else if (dt < 0) {
                // Post before pre: negative eligibility
                eligibility -= std::exp(dt / tau);
            }
        }
    }
    
    // Scale by synaptic efficacy and clip to bounds
    eligibility = std::clamp(eligibility, -pImpl->maxTrace, pImpl->maxTrace);
    synapse->setEligibilityTrace(eligibility);
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get current eligibility trace
    float eligibility = synapse->getEligibilityTrace();
    
    // Apply reward-modulated weight change: delta_weight = reward * eligibility * weight
    float weightChange = pImpl->rewardWeight * eligibility * synapse->getWeight();
    
    // Apply scaled delta with bounds
    float newWeight = synapse->getWeight() + weightChange;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
    
    // Reset eligibility trace after application
    synapse->setEligibilityTrace(0.0f);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setRewardWeight(float weight) {
    pImpl->rewardWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getRewardWeight() const {
    return pImpl->rewardWeight;
}

void RewardModulatedSTDP::setTraceDecay(float decayRate) {
    pImpl->traceDecayRate = std::clamp(decayRate, 0.001f, 0.1f);
}

float RewardModulatedSTDP::getTraceDecay() const {
    return pImpl->traceDecayRate;
}

} // namespace nlm
