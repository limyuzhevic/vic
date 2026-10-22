#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Novelty detection threshold
    float detectionThreshold;
    
    // Time-dependent adaptation
    float adaptationTime;
    float currentAdaptation;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(3.0f)
        , detectionThreshold(0.3f)
        , adaptationTime(0.0f)
        , currentAdaptation(0.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

const char* Novelty::getName() const {
    return "NOVELTY";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Novelty::getPlasticityFactor() const {
    // Novelty increases plasticity for rapid learning of new experiences
    return 0.8f + 0.6f * pImpl->level;
}

void Novelty::update(TimestepDuration dt) {
    // Time-dependent adaptation to maintain optimal sensitivity
    pImpl->adaptationTime += static_cast<float>(dt);
    float targetAdaptation = 0.5f + 0.3f * std::sin(pImpl->adaptationTime * 0.1f);
    pImpl->currentAdaptation += (targetAdaptation - pImpl->currentAdaptation) * 0.05f;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Adapt sensitivity
    pImpl->detectionThreshold = std::clamp(0.3f + 0.2f * pImpl->currentAdaptation, 0.2f, 0.5f);
}

void Novelty::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->currentAdaptation = 0.0f;
    pImpl->adaptationTime = 0.0f;
}

} // namespace nlm
