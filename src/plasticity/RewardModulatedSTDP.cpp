// RewardModulatedSTDP implementation
#include "STDP.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct RewardModulatedSTDP::Impl {
    float ltpWeight;              // Base A+ for potentiation
    float ltdWeight;              // Base A- for depression
    float timeConstant;           // Tau for exponential window (ms)
    float minWeight;              // Minimum synaptic weight
    float maxWeight;              // Maximum synaptic weight
    float rewardPredictionError;  // R-PE signal (-1 to +1)
    float eligibilityTrace;       // Synaptic eligibility trace
    float traceDecayRate;         // Trace decay rate
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), rewardPredictionError(0.0f),
             eligibilityTrace(0.0f), traceDecayRate(0.1f) {}
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
    
    // Update eligibility trace with spike activity
    float dtFloat = static_cast<float>(dt);
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= std::exp(-pImpl->traceDecayRate * dtFloat);
    
    // Add to eligibility trace from pre-post spike pairs
    float spikeContribution = 0.0f;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: positive contribution
                spikeContribution += std::exp(-dt / pImpl->timeConstant);
            } else if (dt < 0) {
                // Post before pre: negative contribution
                spikeContribution -= std::exp(dt / pImpl->timeConstant);
            }
        }
    }
    
    pImpl->eligibilityTrace += spikeContribution * pImpl->ltpWeight * dtFloat;
    
    // Apply reward modulation if RPE is available
    if (std::abs(pImpl->rewardPredictionError) > 1e-6f) {
        float delta = pImpl->eligibilityTrace * pImpl->rewardPredictionError * dtFloat;
        
        // Apply weight change with bounds
        applyWeightChange(synapse, delta);
        
        // Decay eligibility trace after application
        pImpl->eligibilityTrace *= 0.5f;
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

void RewardModulatedSTDP::setRewardPredictionError(float rpe) {
    pImpl->rewardPredictionError = std::clamp(rpe, -1.0f, 1.0f);
}

float RewardModulatedSTDP::getRewardPredictionError() const {
    return pImpl->rewardPredictionError;
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

void RewardModulatedSTDP::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float RewardModulatedSTDP::getMinWeight() const {
    return pImpl->minWeight;
}

void RewardModulatedSTDP::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float RewardModulatedSTDP::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
