#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float learningRate;      // Learning rate for reward modulation
    float traceDecay;        // Eligibility trace decay rate
    float rewardScale;       // Reward signal scaling factor
    float minWeight;         // Minimum synaptic weight
    float maxWeight;         // Maximum synaptic weight
    
    Impl() : learningRate(0.001f), traceDecay(0.01f), rewardScale(1.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
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
    
    float totalDelta = 0.0f;
    
    // Compute STDP weight change based on spike timing
    float tau = 20.0f;  // STDP time constant
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (dt > 0) {
                // Pre before post: potentiation
                float delta = 0.01f * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: depression
                float delta = -0.012f * std::exp(dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    // Apply eligibility trace modulation
    float eligibilityTrace = synapse->getEligibilityTrace();
    float modulatedDelta = totalDelta * eligibilityTrace * pImpl->learningRate;
    
    // Apply reward modulation factor (will be set by external reward signal)
    modulatedDelta *= pImpl->rewardScale;
    
    applyWeightChange(synapse, modulatedDelta);
    
    // Decay eligibility trace
    synapse->decayEligibilityTrace(pImpl->traceDecay);
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

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::setTraceDecay(float decay) {
    pImpl->traceDecay = std::clamp(decay, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getTraceDecay() const {
    return pImpl->traceDecay;
}

void RewardModulatedSTDP::setRewardScale(float scale) {
    pImpl->rewardScale = std::clamp(scale, 0.0f, 10.0f);
}

float RewardModulatedSTDP::getRewardScale() const {
    return pImpl->rewardScale;
}

void RewardModulatedSTDP::configure(float learningRate, float traceDecay, float rewardScale) {
    setLearningRate(learningRate);
    setTraceDecay(traceDecay);
    setRewardScale(rewardScale);
}

} // namespace nlm