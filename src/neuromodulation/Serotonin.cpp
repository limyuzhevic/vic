#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"

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
    // TODO PHASE 2: Implement real serotonin-modulated plasticity factor
    // PLACEHOLDER: Higher 5-HT decreases impulsivity, promotes exploration
    return 0.5f + 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalSocialContext(float contextStrength) {
    // TODO PHASE 2: Implement social context signaling
    // PLACEHOLDER: Serotonin responds to social cues and context
    pImpl->level = std::min(pImpl->peak, pImpl->level + contextStrength * pImpl->releaseRate);
}

void Serotonin::reduceImpulsivity(float reduction) {
    // TODO PHASE 2: Implement impulsivity reduction
    // PLACEHOLDER: Serotonin reduces impulsive responses, increases behavioral control
    pImpl->level = std::max(pImpl->level, reduction * 0.5f);
}

} // namespace nlm
