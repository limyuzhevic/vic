#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(0.5f), decayRate(0.05f), releaseRate(1.0f) {}
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
    // NE modulates arousal and attention, affects plasticity
    return 0.6f + 0.4f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Norepinephrine::getAttentionGain() const {
    // NE increases signal-to-noise ratio in cortical areas
    return 1.0f + 0.5f * pImpl->level;
}

void Norepinephrine::signalSalientEvent(float saliency) {
    // Salient events trigger NE release, increasing alertness
    pImpl->level = std::min(pImpl->peak, pImpl->level + saliency * pImpl->releaseRate);
}

} // namespace nlm
