#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float socialCohesionBoost;
    float behavioralFlexibilityBoost;
    float impulseControlBoost;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.04f)
        , releaseRate(1.0f)
        , socialCohesionBoost(0.3f)
        , behavioralFlexibilityBoost(0.2f)
        , impulseControlBoost(0.1f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized - mood and social behavior");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getSocialCohesion() const {
    // Serotonin enhances social bonding and group cohesion
    return pImpl->level;
}

float Serotonin::getBehavioralFlexibility() const {
    // Serotonin modulates behavioral flexibility and switching
    // Higher serotonin = more flexible behavior when levels are moderate
    return 0.5f + pImpl->level * 0.4f;
}

float Serotonin::getImpulseControl() const {
    // Serotonin enhances impulse control and inhibition
    return 0.3f + 0.7f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSocialReward(float intensity) {
    // Signal social reward (approval, acceptance)
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
    NLM_LOG_DEBUG("Serotonin signals social reward: " + std::to_string(intensity));
}

void Serotonin::signalSocialPredictionError(float error) {
    // Signal prediction error for social learning
    // Negative error (unexpected punishment) reduces level
    if (error < 0) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level - std::abs(error) * pImpl->releaseRate * 0.5f);
    }
    NLM_LOG_DEBUG("Serotonin signals social prediction error: " + std::to_string(error));
}

void Serotonin::signalMoodChange(float delta) {
    // Signal mood change (general emotional state)
    pImpl->level = std::clamp(pImpl->level + delta, pImpl->baseline, pImpl->peak);
    NLM_LOG_DEBUG("Serotonin signals mood change: " + std::to_string(delta));
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm
