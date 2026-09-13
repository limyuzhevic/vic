#pragma once

#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilance;
    float stressResponse;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.03f), releaseRate(1.0f), arousalLevel(0.0f),
             vigilance(0.0f), stressResponse(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized (Arousal and Vigilance)");
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for alertness and attention
    // Higher NE increases plasticity during alert states
    float arousalEffect = pImpl->arousalLevel * 0.4f;
    float vigilanceEffect = pImpl->vigilance * 0.3f;
    float stressEffect = pImpl->stressResponse * 0.2f;
    return 0.5f + arousalEffect + vigilanceEffect + stressEffect;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Smooth arousal and vigilance
    pImpl->arousalLevel += (pImpl->level - pImpl->arousalLevel) * 0.1f;
    pImpl->vigilance += pImpl->level * 0.05f;
    pImpl->stressResponse += pImpl->arousalLevel * 0.02f;
}

// Biological mechanisms for NE
void Norepinephrine::increaseArousal(float stimulusIntensity) {
    // Increase arousal in response to stimuli
    float arousalBoost = stimulusIntensity * pImpl->releaseRate * 0.7f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalBoost);
}

void Norepinephrine::maintainVigilance(float duration) {
    // Maintain vigilance for extended periods
    // NE sustains alertness during prolonged tasks
    float vigilanceMaintenance = duration * pImpl->releaseRate * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceMaintenance);
}

void Norepinephrine::respondToStress(float stressLevel) {
    // Stress response: increases NE for fight-or-flight
    float stressResponse = stressLevel * pImpl->releaseRate * 0.8f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressResponse);
    pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + stressLevel * 0.5f);
}

void Norepinephrine::enhanceSensoryProcessing() {
    // Enhance sensory processing during high arousal
    if (pImpl->level > 0.5f) {
        pImpl->vigilance = std::min(1.0f, pImpl->vigilance + 0.2f);
    }
}

} // namespace nlm
