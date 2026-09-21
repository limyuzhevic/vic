#include "Serotonin.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.5f), baseline(0.3f), peak(1.2f), decayRate(0.06f), releaseRate(0.6f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 1.5f);
}

float Serotonin::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real serotonin-modulated plasticity factor
    // Serotonin modulates mood, impulsivity, and social behavior
    // Higher 5-HT reduces impulsivity and enhances learning from negative outcomes
    return 1.0f + std::max(-0.3f, 0.4f * pImpl->level);
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: Decay towards baseline, release on reward prediction errors
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    // TODO PHASE 2: Implement mood signaling
    // PLACEHOLDER: Serotonin release affects mood and social behavior
    pImpl->level = std::min(pImpl->peak, pImpl->level + mood * pImpl->releaseRate);
}

void Serotonin::signalSocialBehavior(float socialSignal) {
    // TODO PHASE 2: Implement social behavior signaling
    // PLACEHOLDER: Serotonin modulates social learning and cooperation
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialSignal * pImpl->releaseRate * 0.8f);
}

void Serotonin::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

} // namespace nlm