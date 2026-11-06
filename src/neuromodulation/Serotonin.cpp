#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float mood;
    float impulsivity;
    float socialDrive;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f), releaseRate(1.0f), mood(1.0f), impulsivity(1.0f), socialDrive(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates mood and impulsivity
    // Higher serotonin reduces impulsivity and stabilizes mood
    return 1.0f - 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: regulates mood and social behavior
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalReward() {
    // Serotonin release on reward prediction
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.4f * pImpl->releaseRate);
}

void Serotonin::signalSatiety() {
    // Serotonin increase with satiety (fullness, content)
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f * pImpl->releaseRate);
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

void Serotonin::setMood(float mood) {
    pImpl->mood = std::clamp(mood, 0.1f, 2.0f);
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

void Serotonin::setImpulsivity(float impulsivity) {
    pImpl->impulsivity = std::clamp(impulsivity, 0.1f, 2.0f);
}

float Serotonin::getSocialDrive() const {
    return pImpl->socialDrive;
}

void Serotonin::setSocialDrive(float drive) {
    pImpl->socialDrive = std::clamp(drive, 0.1f, 2.0f);
}

} // namespace nlm