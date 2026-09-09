#include "PlasticityRule.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    float rewardDecay;    // Reward decay rate
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), rewardDecay(0.1f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse || !synapse->isEnabled() || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // R-STDP (Reward-modulated STDP)
    // Combines STDP timing with reward prediction error
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        synapse->addToWeight(totalDelta);
        
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
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

void RewardModulatedSTDP::setRewardDecay(float decay) {
    pImpl->rewardDecay = std::clamp(decay, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getRewardDecay() const {
    return pImpl->rewardDecay;
}

} // namespace nlm