#pragma once

#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float stressSensitivity;
    float challengeWeight;
    float arousalBaseline;
    float decayRate;
    float recoveryRate;
    float peakLevel;
    float signalToNoiseBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , stressSensitivity(1.0f)
        , challengeWeight(0.7f)
        , arousalBaseline(0.05f)
        , decayRate(0.02f)
        , recoveryRate(0.1f)
        , peakLevel(1.0f)
        , signalToNoiseBoost(2.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Norepinephrine::update(float stressLevel, float challenge, TimestepDuration dt) {
    // NE responds to stress and challenge
    float stressResponse = stressLevel * pImpl->stressSensitivity;
    float challengeResponse = challenge * pImpl->challengeWeight;
    
    // Strong increase for high stress/challenge
    if (stressLevel > 0.7f || challenge > 0.8f) {
        // Emergency response - surge to peak
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + stressResponse * 2.0f);
    } else {
        // Normal response
        float target = pImpl->arousalBaseline + challengeResponse;
        pImpl->level += (target - pImpl->level) * 0.2f;
    }
    
    // Natural recovery when calm
    if (stressLevel < 0.2f && challenge < 0.3f) {
        pImpl->level = std::max(pImpl->arousalBaseline, 
                              pImpl->level - pImpl->recoveryRate * static_cast<float>(dt));
    }
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity for important/high-salience learning
    // High NE promotes focused, efficient learning
    float baseFactor = 1.0f + pImpl->level * 0.8f;
    
    // Bonus for high arousal
    if (pImpl->level > 0.6f) {
        baseFactor *= 1.3f;
    }
    
    return baseFactor;
}

void Norepinephrine::enhanceSignalToNoise(NeuronId targetNeuron) {
    // NE enhances signal-to-noise ratio, making key neurons more prominent
    if (pImpl->brain) {
        pImpl->brain->injectCurrent(targetNeuron, 8.0f * pImpl->level);
    }
}

void Norepinephrine::increase alertness(float intensity) {
    setLevel(std::min(pImpl->level + intensity * pImpl->stressSensitivity, pImpl->peakLevel));
}

void Norepinephrine::triggerFightOrFlightResponse() {
    // Emergency surge for survival situations
    setLevel(pImpl->peakLevel);
    if (pImpl->brain) {
        // Increase arousal across the system
        pImpl->brain->injectCurrentToNeurons(NeuronType::Internal, 10.0f);
    }
}

void Norepinephrine::setStressSensitivity(float sensitivity) {
    pImpl->stressSensitivity = std::clamp(sensitivity, 0.0f, 3.0f);
}

void Norepinephrine::setChallengeWeight(float weight) {
    pImpl->challengeWeight = std::clamp(weight, 0.0f, 2.0f);
}

void Norepinephrine::setArousalBaseline(float baseline) {
    pImpl->arousalBaseline = std::clamp(baseline, 0.0f, 0.5f);
}

float Norepinephrine::getStressSensitivity() const {
    return pImpl->stressSensitivity;
}

float Norepinephrine::getChallengeWeight() const {
    return pImpl->challengeWeight;
}

float Norepinephrine::getArousalBaseline() const {
    return pImpl->arousalBaseline;
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->arousalBaseline;
}

} // namespace nlm