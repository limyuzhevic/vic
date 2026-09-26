#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilanceLevel;
    float focusStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             arousalLevel(1.0f), vigilanceLevel(1.0f), focusStrength(1.0f) {}
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
    // NE enhances arousal and focus: Higher levels increase alertness and selective attention
    float arousalEffect = 0.5f + 0.5f * pImpl->arousalLevel;
    float vigilanceEffect = 0.5f + 0.5f * pImpl->vigilanceLevel;
    float focusEffect = 0.5f + 0.5f * pImpl->focusStrength;
    return std::min(3.0f, arousalEffect * vigilanceEffect * focusEffect);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal based on current level
    pImpl->arousalLevel = 0.5f + 0.5f * pImpl->level;
    
    // Vigilance affects threat detection and alertness
    pImpl->vigilanceLevel = 0.3f + 0.7f * pImpl->level;
    
    // Focus strength for selective attention
    pImpl->focusStrength = 0.6f + 0.4f * pImpl->level;
}

void Norepinephrine::signalAlertness(float intensity) {
    // Increase alertness for the given intensity
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
}

void Norepinephrine::enhanceVigilance(float duration) {
    // Enhance vigilance for the given duration
    pImpl->vigilanceLevel = std::min(2.0f, pImpl->vigilanceLevel + duration * 0.2f);
}

} // namespace nlm
