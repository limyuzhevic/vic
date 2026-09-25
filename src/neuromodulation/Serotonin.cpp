#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// ========== SEROTONIN IMPLEMENTATION ==========

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float impulseControl;
    float moodStability;
    float socialBehavior;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f), releaseRate(1.0f),
             impulseControl(1.0f), moodStability(1.0f), socialBehavior(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() {
    delete pImpl;
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin stabilizes learning, reducing impulsivity
    return 1.0f / (1.0f + pImpl->level * 0.5f);
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline, moderate decay rate
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::reduceImpulsivity(float factor) {
    pImpl->impulseControl = factor;
    setLevel(std::min(1.0f, pImpl->level + 0.1f * factor));
}

void Serotonin::stabilizeMood(float stability) {
    pImpl->moodStability = stability;
    setLevel(std::min(1.0f, pImpl->level + 0.08f * stability));
}

} // namespace nlm
