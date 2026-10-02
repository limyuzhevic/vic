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
    // 5-HT reduces impulsivity and stabilizes behavior
    // Higher 5-HT reduces plasticity (stabilizes)
    return 1.0f - 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // TODO: Implement real 5-HT dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodSignal) {
    // TODO: Implement mood signaling
    // PLACEHOLDER: Burst of 5-HT on mood change
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodSignal * pImpl->releaseRate);
}

void Serotonin::signalImpulsivity(float impulsivity) {
    // TODO: Implement impulsivity signaling
    // PLACEHOLDER: 5-HT modulates impulsivity
    pImpl->level = std::max(0.0f, pImpl->level + (1.0f - impulsivity) * pImpl->releaseRate * 0.5f);
}

} // namespace nlm