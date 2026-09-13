#include "EnhancedNeuromodulation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float moodSensitivity_;
    float impulseControlSensitivity_;
    float socialBehaviorSensitivity_;
    
    Impl() : moodSensitivity_(1.0f), impulseControlSensitivity_(1.0f), socialBehaviorSensitivity_(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return state_.tonicLevel + state_.phasicLevel;
}

void Serotonin::setLevel(float level) {
    state_.tonicLevel = level * 0.6f;
    state_.phasicLevel = level * 0.4f;
}

float Serotonin::getPhasicLevel() const {
    return state_.phasicLevel;
}

float Serotonin::getTonicLevel() const {
    return state_.tonicLevel;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity for mood-related processes
    return 1.0f + 0.2f * (state_.tonicLevel + state_.phasicLevel);
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin tonic level decays moderately
    float tonicDecay = 0.015f * static_cast<float>(dt);
    state_.tonicLevel = std::max(0.0f, state_.tonicLevel - tonicDecay);
    
    // Phasic serotonin decays quickly
    float phasicDecay = state_.phasicAdaptation * static_cast<float>(dt);
    state_.phasicLevel = std::max(0.0f, state_.phasicLevel - phasicDecay);
    
    // Adapt baseline
    float activityAdaptation = state_.baselineAdaptation * static_cast<float>(dt);
    state_.adaptiveBaseline = state_.adaptiveBaseline * (1.0f - activityAdaptation) +
                               (state_.tonicLevel + state_.phasicLevel) * activityAdaptation;
}

void Serotonin::phasicResponse(float magnitude) {
    state_.phasicLevel += magnitude * pImpl->moodSensitivity_;
    state_.phasicLevel = std::min(1.0f, state_.phasicLevel);
}

const NeuromodulatorState& Serotonin::getState() const {
    return state_;
}

void Serotonin::increaseMood(float amount) {
    state_.phasicLevel += amount * pImpl->moodSensitivity_;
    state_.phasicLevel = std::min(1.0f, state_.phasicLevel);
}

void Serotonin::decreaseMood(float amount) {
    state_.tonicLevel = std::max(0.0f, state_.tonicLevel - amount * pImpl->moodSensitivity_);
}

void Serotonin::increaseImpulseControl(float amount) {
    state_.phasicLevel += amount * pImpl->impulseControlSensitivity_;
    state_.phasicLevel = std::min(1.0f, state_.phasicLevel);
}

void Serotonin::decreaseImpulseControl(float amount) {
    state_.tonicLevel = std::max(0.0f, state_.tonicLevel - amount * pImpl->impulseControlSensitivity_);
}

void Serotonin::increaseSocialBehavior(float amount) {
    state_.phasicLevel += amount * pImpl->socialBehaviorSensitivity_;
    state_.phasicLevel = std::min(1.0f, state_.phasicLevel);
}

void Serotonin::decreaseSocialBehavior(float amount) {
    state_.tonicLevel = std::max(0.0f, state_.tonicLevel - amount * pImpl->socialBehaviorSensitivity_);
}

} // namespace nlm
