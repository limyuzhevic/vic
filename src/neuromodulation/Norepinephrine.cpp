#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), arousal(0.0f) {}
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
    // Norepinephrine modulates arousal and attention - increases plasticity
    return 1.0f + 0.3f * pImpl->level + pImpl->arousal * 0.5f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Norepinephrine increases with stress/arousal, decays towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->arousal = std::max(0.0f, pImpl->arousal - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalStress(float stress) {
    // Norepinephrine responds to stress and arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + stress * pImpl->releaseRate);
    pImpl->arousal = std::min(1.0f, pImpl->arousal + stress * 0.5f);
}

void Norepinephrine::signalAttention(float attention) {
    // Norepinephrine increases with attention and focus
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate * 0.5f);
}

} // namespace nlm