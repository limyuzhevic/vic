#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    // Reward prediction error computation
    float currentRPE;
    float predictedReward;
    float integratedReward;
    
    // Eligibility traces for credit assignment
    std::unordered_map<uint64_t, float> eligibilityTraces;
    float eligibilityDecayRate;
    
    // Dopamine dynamics components
    float tonicLevel;
    float phasicLevel;
    float baseline;
    float gain;
    
    // Reward history
    std::vector<float> rewardHistory;
    std::vector<float> errorHistory;
    
    // Bursting control
    bool isBursting;
    float firingRate;
    
    // Plasticity modulation multipliers
    float ltpMultiplier;
    float ltdMultiplier;
    
    Impl() : 
        currentRPE(0.0f),
        predictedReward(0.0f),
        integratedReward(0.0f),
        eligibilityDecayRate(0.05f),
        tonicLevel(0.1f),
        phasicLevel(0.0f),
        baseline(0.0f),
        gain(1.0f),
        isBursting(false),
        firingRate(0.0f),
        ltpMultiplier(1.0f),
        ltdMultiplier(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine neuromodulator initialized");
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->tonicLevel + pImpl->phasicLevel;
}

void Dopamine::setLevel(float level) {
    // Split into tonic and phasic components
    pImpl->tonicLevel = level * 0.8f;  // Mostly tonic
    pImpl->phasicLevel = level * 0.2f; // Some phasic
    NLM_LOG_DEBUG("Dopamine level set to: " + std::to_string(level));
}

float Dopamine::getPlasticityFactor() const {
    // Factor based on both tonic and phasic components
    float basePlasticity = 1.0f;
    
    // Phasic dopamine (positive for LTP, negative for LTD)
    if (pImpl->phasicLevel > 0.0f) {
        basePlasticity *= pImpl->phasicLevel * pImpl->getLTPMultiplier();
    } else if (pImpl->phasicLevel < 0.0f) {
        basePlasticity *= (1.0f + pImpl->phasicLevel * pImpl->getLTDMultiplier());
    }
    
    return std::max(0.0f, basePlasticity);
}

void Dopamine::update(TimestepDuration dt) {
    // Decay eligibility traces
    for (auto& pair : pImpl->eligibilityTraces) {
        pair.second *= (1.0f - pImpl->eligibilityDecayRate * dt);
    }
    
    // Decay phasic component over time
    pImpl->phasicLevel *= (1.0f - 0.01f * dt);
}

void Dopamine::signalReward(float reward, float predictedReward) {
    // Compute reward prediction error (RPE)
    pImpl->currentRPE = reward - predictedReward;
    pImpl->predictedReward = reward;
    
    // Update phasic dopamine signal
    if (pImpl->currentRPE > 0.0f) {
        // Positive prediction error: unexpected reward (LTP signal)
        pImpl->phasicLevel = pImpl->currentRPE * pImpl->gain;
    } else if (pImpl->currentRPE < 0.0f) {
        // Negative prediction error: reward prediction error (LTD signal)
        pImpl->phasicLevel = pImpl->currentRPE * pImpl->gain * 0.5f; // Reduced magnitude
    }
    
    // Record history
    pImpl->rewardHistory.push_back(reward);
    pImpl->errorHistory.push_back(pImpl->currentRPE);
    
    // Limit history size
    if (pImpl->rewardHistory.size() > 100) {
        pImpl->rewardHistory.erase(pImpl->rewardHistory.begin());
        pImpl->errorHistory.erase(pImpl->errorHistory.begin());
    }
    
    NLM_LOG_DEBUG("Dopamine: RPE = " + std::to_string(pImpl->currentRPE) +
                  ", phasic signal = " + std::to_string(pImpl->phasicLevel));
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->currentRPE = error;
    
    // Update phasic signal based on error magnitude
    float errorMagnitude = std::abs(error);
    pImpl->phasicLevel = (error > 0.0f ? 1.0f : -1.0f) * 
                       std::min(errorMagnitude, 1.0f) * pImpl->gain;
    
    // Bursting for large prediction errors
    pImpl->isBursting = errorMagnitude > 0.5f;
    
    NLM_LOG_DEBUG("Dopamine: Prediction error = " + std::to_string(error));
}

float Dopamine::getPredictionError() const {
    return pImpl->currentRPE;
}

void Dopamine::accumulateEligibility(NeuronId neuron, float trace) {
    uint64_t neuronId = neuron.value;
    pImpl->eligibilityTraces[neuronId] += trace;
}

void Dopamine::decayEligibility(float decayRate) {
    float rate = decayRate > 0.0f ? decayRate : pImpl->eligibilityDecayRate;
    
    for (auto& pair : pImpl->eligibilityTraces) {
        pair.second *= (1.0f - rate);
    }
}

float Dopamine::getEligibility(NeuronId neuron) const {
    auto it = pImpl->eligibilityTraces.find(neuron.value);
    return (it != pImpl->eligibilityTraces.end()) ? it->second : 0.0f;
}

void Dopamine::clearEligibility() {
    pImpl->eligibilityTraces.clear();
    NLM_LOG_DEBUG("Dopamine eligibility traces cleared");
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
    pImpl->tonicLevel = baseline;
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

void Dopamine::setGain(float gain) {
    pImpl->gain = gain;
}

float Dopamine::getGain() const {
    return pImpl->gain;
}

void Dopamine::integrateReward(float reward, float timestamp) {
    // Simple integration: weighted average
    if (pImpl->rewardHistory.empty()) {
        pImpl->integratedReward = reward;
    } else {
        pImpl->integratedReward = pImpl->integratedReward * 0.9f + reward * 0.1f;
    }
}

float Dopamine::getIntegratedReward() const {
    return pImpl->integratedReward;
}

void Dopamine::setPhasicComponent(float phasic) {
    pImpl->phasicLevel = phasic;
}

void Dopamine::setTonicComponent(float tonic) {
    pImpl->tonicLevel = tonic;
}

float Dopamine::getPhasic() const {
    return pImpl->phasicLevel;
}

float Dopamine::getTonic() const {
    return pImpl->tonicLevel;
}

void Dopamine::setBursting(bool bursting) {
    pImpl->isBursting = bursting;
    if (bursting) {
        pImpl->firingRate = 20.0f; // Hz
    }
}

bool Dopamine::isBursting() const {
    return pImpl->isBursting;
}

float Dopamine::getFiringRate() const {
    return pImpl->firingRate;
}

void Dopamine::setLTPMultiplier(float multiplier) {
    pImpl->ltpMultiplier = multiplier;
}

void Dopamine::setLTDMultiplier(float multiplier) {
    pImpl->ltdMultiplier = multiplier;
}

float Dopamine::getLTPMultiplier() const {
    return pImpl->ltpMultiplier;
}

float Dopamine::getLTDMultiplier() const {
    return pImpl->ltdMultiplier;
}

} // namespace nlm
