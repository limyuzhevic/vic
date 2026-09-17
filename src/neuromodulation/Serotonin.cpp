#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float impulsivity;  // Modulates risk-taking and exploration
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f), impulsivity(0.5f) {}
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
    // Serotonin modulates mood, punishment sensitivity, and social behavior
    return 1.0f - pImpl->level * 0.3f + pImpl->impulsivity * 0.2f;  // High 5-HT = more conservative
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: moderate decay, responds to reward prediction errors
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalReward(float reward) {
    // Serotonin responds to reward prediction errors (negative errors)
    if (reward < 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    }
}

void Serotonin::signalPunishment(float punishment) {
    // Serotonin responds to punishment
    if (punishment > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + punishment * pImpl->releaseRate * 0.7f);
    }
}

void Serotonin::signalSocialFeedback(float socialFeedback) {
    // Serotonin modulates social behavior and acceptance
    pImpl->impulsivity = std::clamp(socialFeedback, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialFeedback * pImpl->releaseRate * 0.5f);
}

} // namespace nlm