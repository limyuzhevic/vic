#include "../plasticity/RewardModulatedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float baseLTPWeight;
    float baseLTDWeight;
    float timeConstant;
    float learningRate;
    Neuromodulator* neuromodulator;
    float rewardThreshold;
    float accumulatedPreSpikeTime;
    std::vector<Timestamp> recentPreSpikes;
    
    Impl() : baseLTPWeight(0.01f), baseLTDWeight(0.005f), timeConstant(20.0f),
             learningRate(0.1f), neuromodulator(nullptr), rewardThreshold(0.5f),
             accumulatedPreSpikeTime(0.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    if (!synapse) return;
    
    // OPTIMIZATION: Pre-calculate common values to avoid repeated calculations
    float timeConstant = pImpl->timeConstant;
    float baseLTPWeight = pImpl->baseLTPWeight;
    float baseLTDWeight = pImpl->baseLTDWeight;
    float learningRate = pImpl->learningRate;
    bool enablePlasticity = isPlasticityEnabled();
    
    // Store previous spike times (optimization: use references instead of copies when possible)
    pImpl->recentPreSpikes = preSpikes;
    
    // Apply STDP with reward modulation
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);
            
            // Standard STDP
            float weightChange = 0.0f;
            
            if (deltaT > 0) {  // Post after pre: LTP
                weightChange = baseLTPWeight * std::exp(-deltaT / timeConstant);
            } else {  // Pre after post: LTD
                weightChange = -baseLTDWeight * std::exp(deltaT / timeConstant);
            }
            
            // Apply reward gating
            if (enablePlasticity) {
                weightChange *= learningRate * pImpl->neuromodulator->getLevel();
                applyWeightChange(synapse, weightChange);
            }
        }
    }
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, 0.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setBaseLTPWeight(float weight) {
    pImpl->baseLTPWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaseLTPWeight() const {
    return pImpl->baseLTPWeight;
}

void RewardModulatedSTDP::setBaseLTDWeight(float weight) {
    pImpl->baseLTDWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getBaseLTDWeight() const {
    return pImpl->baseLTDWeight;
}

void RewardModulatedSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::max(1.0f, tau);
}

float RewardModulatedSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void RewardModulatedSTDP::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLearningRate() const {
    return pImpl->learningRate;
}

void RewardModulatedSTDP::connectToNeuromodulator(Neuromodulator* neuromodulator) {
    pImpl->neuromodulator = neuromodulator;
}

void RewardModulatedSTDP::setRewardThreshold(float threshold) {
    pImpl->rewardThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

bool RewardModulatedSTDP::isPlasticityEnabled() const {
    if (!pImpl->neuromodulator) return false;
    return pImpl->neuromodulator->getLevel() >= pImpl->rewardThreshold;
}

} // namespace nlm