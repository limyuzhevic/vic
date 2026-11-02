#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.15f), releaseRate(1.0f), moodBoost(1.2f) {}
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
    // PLACEHOLDER: Higher serotonin reduces plasticity, stabilizing memory
    // 5-HT promotes forgetting, preventing overwrite and interference
    return 0.8f - 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real serotonin dynamics
    // PLACEHOLDER: Decay towards baseline, modulated by mood and social factors
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalMood(float mood) {
    // TODO PHASE 2: Implement real mood signaling
    // PLACEHOLDER: Serotonin responds to reward prediction errors
    pImpl->level = std::min(pImpl->peak, pImpl->level + mood * pImpl->releaseRate);
}

void Serotonin::signalSatisfaction(float satisfaction) {
    // TODO PHASE 2: Implement real satisfaction signaling
    // PLACEHOLDER: 5-HT signals state satisfaction and promotes consolidation
    pImpl->level = std::max(pImpl->level, satisfaction * 0.8f);
}

void Serotonin::signalImpulseControl(float impulse) {
    // TODO PHASE 2: Implement real impulse control signaling
    // PLACEHOLDER: Serotonin supports cognitive control and reduces impulsivity
    pImpl->level = std::min(pImpl->peak, pImpl->level + impulse * 0.5f * pImpl->releaseRate);
}

} // namespace nlm
