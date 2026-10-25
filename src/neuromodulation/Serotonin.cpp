#include "Serotonin.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;           // Current serotonin level
    float mood;            // Mood state (0.0 to 1.0)
    float impulsivity;     // Impulsivity level (0.0 to 1.0)
    float socialDrive;     // Social motivation (0.0 to 1.0)
    
    Impl() : level(0.0f), mood(0.5f), impulsivity(0.0f), socialDrive(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    // Clamp level to reasonable range
    pImpl->level = std::clamp(level, 0.0f, 10.0f);
    
    // Update mood based on level
    pImpl->mood = std::min(pImpl->level / 10.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood, impulsivity, and social behavior
    // Moderate levels support flexible learning
    float moodEffect = (pImpl->mood - 0.5f) * 0.3f;  // Mood affects plasticity
    float impulsivityEffect = -pImpl->impulsivity * 0.2f;  // Impulsivity reduces plasticity
    
    return 1.0f + moodEffect + impulsivityEffect;
}

void Serotonin::update(TimestepDuration dt) {
    // Gradually decay toward baseline
    pImpl->level += (0.1f - pImpl->level) * dt * 0.01f;
    pImpl->mood = std::min(pImpl->level / 10.0f, 1.0f);
}

void Serotonin::signalSocialInteraction(float interactionStrength) {
    // Social interactions boost serotonin
    pImpl->socialDrive += interactionStrength * 0.1f;
    pImpl->socialDrive = std::min(pImpl->socialDrive, 1.0f);
    
    // Increase serotonin level
    setLevel(pImpl->level + interactionStrength * 0.2f);
}

float Serotonin::getSocialDrive() const {
    return pImpl->socialDrive;
}

} // namespace nlm