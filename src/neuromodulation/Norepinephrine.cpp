// Norepinephrine.cpp
// Implementation of Norepinephrine neuromodulator for arousal and vigilance

#include "Norepinephrine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float vigilance;
    float baseline;
    float peak;
    float decayRate;
    float recoveryRate;
    
    Impl() : level(0.0f), arousalLevel(0.5f), vigilance(0.5f),
             baseline(0.0f), peak(1.0f), decayRate(0.1f), recoveryRate(0.05f) {}
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
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates arousal and vigilance
    // Higher NE increases arousal and attentional focus
    // Moderate NE enhances learning, very high NE causes stress
    if (pImpl->level < 0.3f) {
        // Low NE: reduced arousal
        return 0.8f + pImpl->level * 0.5f;
    } else if (pImpl->level < 0.7f) {
        // Moderate NE: optimal learning
        return 1.3f - (pImpl->level - 0.3f) * 0.5f;
    } else {
        // High NE: stress, reduced learning
        return 1.0f - (pImpl->level - 0.7f) * 0.5f;
    }
}

void Norepinephrine::update(TimestepDuration dt) {
    // Natural decay and recovery to baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Recover arousal and vigilance
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + pImpl->recoveryRate * static_cast<float>(dt));
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + pImpl->recoveryRate * static_cast<float>(dt));
}

void Norepinephrine::signalAlert(float intensity) {
    // Alerting stimuli increase norepinephrine
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * 2.0f);
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + intensity * 0.5f);
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + intensity * 0.5f);
}

void Norepinephrine::signalStress(float intensity) {
    // Stress increases norepinephrine dramatically
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * 3.0f);
}

void Norepinephrine::setArousalLevel(float arousal) {
    pImpl->arousalLevel = std::clamp(arousal, 0.0f, 1.0f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

void Norepinephrine::setVigilance(float vigilance) {
    pImpl->vigilance = std::clamp(vigilance, 0.0f, 1.0f);
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getAttentionalFocus() const {
    // Attentional focus is related to arousal and vigilance
    return pImpl->arousalLevel * pImpl->vigilance;
}

} // namespace nlm
