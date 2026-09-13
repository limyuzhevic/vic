#include "EnhancedNeuromodulation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float arousalSensitivity_;
    float stressSensitivity_;
    
    Impl() : arousalSensitivity_(1.0f), stressSensitivity_(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return state_.tonicLevel + state_.phasicLevel;
}

void Norepinephrine::setLevel(float level) {
    state_.tonicLevel = level * 0.7f;
    state_.phasicLevel = level * 0.3f;
}

float Norepinephrine::getPhasicLevel() const {
    return state_.phasicLevel;
}

float Norepinephrine::getTonicLevel() const {
    return state_.tonicLevel;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity, especially for arousal-related processes
    return 1.0f + 0.3f * (state_.tonicLevel + state_.phasicLevel);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE tonic level decays moderately
    float tonicDecay = 0.02f * static_cast<float>(dt);
    state_.tonicLevel = std::max(0.0f, state_.tonicLevel - tonicDecay);
    
    // Phasic NE decays quickly
    float phasicDecay = state_.phasicAdaptation * static_cast<float>(dt);
    state_.phasicLevel = std::max(0.0f, state_.phasicLevel - phasicDecay);
    
    // Adapt baseline
    float activityAdaptation = state_.baselineAdaptation * static_cast<float>(dt);
    state_.adaptiveBaseline = state_.adaptiveBaseline * (1.0f - activityAdaptation) +
                               (state_.tonicLevel + state_.phasicLevel) * activityAdaptation;
}

void Norepinephrine::phasicResponse(float magnitude) {
    state_.phasicLevel += magnitude * pImpl->arousalSensitivity_;
    state_.phasicLevel = std::min(1.0f, state_.phasicLevel);
}

const NeuromodulatorState& Norepinephrine::getState() const {
    return state_;
}

void Norepinephrine::increaseArousal(float amount) {
    state_.phasicLevel += amount * pImpl->arousalSensitivity_;
    state_.phasicLevel = std::min(1.0f, state_.phasicLevel);
}

void Norepinephrine::decreaseArousal(float amount) {
    state_.tonicLevel = std::max(0.0f, state_.tonicLevel - amount * pImpl->arousalSensitivity_);
}

void Norepinephrine::triggerStressResponse(float intensity) {
    state_.phasicLevel += intensity * pImpl->stressSensitivity_;
    state_.phasicLevel = std::min(1.0f, state_.phasicLevel);
}

} // namespace nlm
