#include "Neuromodulator.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances signal-to-noise ratio and plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalVigilance(float vigilanceLevel) {
    // Burst of norepinephrine during alertness/vigilance
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceLevel * pImpl->releaseRate);
}

void Norepinephrine::signalStress(float stressLevel) {
    // NE responds to stress, enhancing arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * pImpl->releaseRate * 0.7f);
}

void Norepinephrine::enhanceSignalToNoise(float signalStrength) {
    // NE enhances signal-to-noise ratio for focused processing
    pImpl->level = std::min(pImpl->peak, pImpl->level + signalStrength * pImpl->releaseRate * 0.5f);
}

} // namespace nlm