#pragma once

#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float memoryStrength;
    float consolidationFactor;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f), memoryStrength(0.8f), consolidationFactor(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::enhanceMemoryConsolidation(float strength) {
    // Dopaminergic enhancement of memory consolidation
    pImpl->consolidationFactor = 1.0f + strength * pImpl->memoryStrength;
    
    NLM_LOG_INFO("DA enhancing memory consolidation: strength=" + std::to_string(strength) + 
                 " consolidationFactor=" + std::to_string(pImpl->consolidationFactor));
}

void Dopamine::modulateSTDP(float predictedError) {
    // Dopamine modulates STDP based on prediction error
    float modulation = 1.0f + predictedError * 0.2f;
    
    NLM_LOG_INFO("DA modulating STDP: predictionError=" + std::to_string(predictedError) + 
                 " modulation=" + std::to_string(modulation));
}

} // namespace nlm
