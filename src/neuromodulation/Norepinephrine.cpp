#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

class Brain;

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Vigilance and arousal parameters
    float vigilanceGain;
    float stressResponse;
    float fightFlightReadiness;
    
    // State tracking
    bool isHighAlert;
    float lastStressEvent;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.02f)
        , peak(1.0f)
        , decayRate(0.08f)
        , releaseRate(1.5f)
        , vigilanceGain(1.3f)
        , stressResponse(0.5f)
        , fightFlightReadiness(0.5f)
        , isHighAlert(false)
        , lastStressEvent(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update alertness state
    pImpl->isHighAlert = (pImpl->level > 0.15f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for salient events
    return 1.0f + pImpl->stressResponse * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update vigilance if brain is available
    if (pImpl->brain) {
        updateVigilance(*pImpl->brain);
    }
}

void Norepinephrine::signalStress(float stressLevel, float threatLevel) {
    // Increase level based on stress and threat
    float boost = stressLevel * 0.4f + threatLevel * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
    pImpl->lastStressEvent = static_cast<float>(getCurrentTime());
}

void Norepinephrine::signalVigilance(float arousal, float novelty) {
    // Boost level for increased arousal and novelty
    float boost = arousal * 0.3f + novelty * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost * pImpl->releaseRate);
}

void Norepinephrine::setHighAlert(bool isHighAlert) {
    pImpl->isHighAlert = isHighAlert;
    if (isHighAlert) {
        pImpl->level = std::min(pImpl->peak, pImpl->level * 1.5f);
    }
}

bool Norepinephrine::isHighAlert() const {
    return pImpl->isHighAlert;
}

float Norepinephrine::getVigilanceGain() const {
    return pImpl->vigilanceGain + pImpl->level * 0.5f;
}

float Norepinephrine::getFightFlightReadiness() const {
    return pImpl->fightFlightReadiness + pImpl->stressResponse * pImpl->level;
}

void Norepinephrine::updateVigilance(Brain& brain) {
    // Apply vigilance modulation to sensory processing
    // Increases sensitivity to relevant stimuli
    // This would modify sensory neuron thresholds in the brain
}

void Norepinephrine::registerVigilanceHandler(void (Brain::*handler)()) {
    // Placeholder for vigilance signal registration
}

float Norepinephrine::getCurrentTime() const {
    // This should be replaced with actual time from simulation clock
    return 0.0f;
}

} // namespace nlm
