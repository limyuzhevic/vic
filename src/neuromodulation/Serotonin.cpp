#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float mood;
    float impulsivity;
    float socialBehavior;
    float decayRate;
    std::vector<float> history;
    
    Impl() : brain(nullptr), level(0.0f), mood(0.5f), impulsivity(0.5f), socialBehavior(0.5f), decayRate(0.1f) {}
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
    // Serotonin modulates mood and impulse control, affecting plasticity
    float factor = 1.0f;
    if (pImpl->mood > 0.5f) {
        factor += (pImpl->mood - 0.5f) * 0.2f;
    }
    if (pImpl->impulsivity < 0.5f) {
        factor += (0.5f - pImpl->impulsivity) * 0.15f;
    }
    return factor;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay serotonin
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->history.push_back(pImpl->level);
}

void Serotonin::signalReward(float reward) {
    // Serotonin responds to reward and social feedback
    pImpl->level = std::min(1.0f, pImpl->level + reward * 0.2f);
}

void Serotonin::enhanceSocialBehavior(float socialStrength) {
    // Boost social behavior
    pImpl->level = std::min(1.0f, pImpl->level + socialStrength * 0.3f);
}

void Serotonin::setImpulsivity(float impulsivityLevel) {
    pImpl->impulsivity = impulsivityLevel;
}

void Serotonin::clearHistory() {
    pImpl->history.clear();
}

} // namespace nlm
