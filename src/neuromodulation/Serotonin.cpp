#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // Serotonin reduces plasticity, stabilizing neural connections
    // This helps consolidate learned behaviors and prevent overfitting
    return std::max(0.1f, 1.0f - pImpl->level * 0.8f);
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSocialReward(float reward) {
    // TODO PHASE 2: Implement real social reward signaling
    // PLACEHOLDER: Burst of serotonin on social reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 0.5f);
}

void Serotonin::signalImpulsivityChange(float delta) {
    // TODO PHASE 2: Implement real impulsivity signaling
    // PLACEHOLDER: Serotonin responds to impulsivity changes
    pImpl->level = std::max(0.0f, pImpl->level + delta * pImpl->releaseRate * 0.3f);
}

} // namespace nlm
