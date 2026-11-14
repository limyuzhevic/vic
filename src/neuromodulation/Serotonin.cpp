#include "Serotonin.hpp"
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f), releaseRate(1.0f) {}
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
    // PLACEHOLDER: Higher serotonin increases mood and social behavior
    return 0.5f + 0.5f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float moodLevel) {
    // TODO PHASE 2: Implement real mood signaling
    // PLACEHOLDER: Burst of serotonin for mood
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodLevel * pImpl->releaseRate);
}

} // namespace nlm
