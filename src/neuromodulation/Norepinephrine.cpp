#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousal;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f), arousal(0.5f) {}
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
    // TODO PHASE 2: Implement real norepinephrine-modulated plasticity factor
    // PLACEHOLDER: Higher norepinephrine enhances arousal and working memory
    return 0.5f + 0.5f * pImpl->level * pImpl->arousal;
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real norepinephrine dynamics
    // PLACEHOLDER: Decay towards baseline with slower rate
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float intensity) {
    // TODO PHASE 2: Implement real arousal signaling
    // PLACEHOLDER: Burst of norepinephrine on arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
    pImpl->arousal = std::min(1.0f, pImpl->arousal + intensity * 0.1f);
}

void Norepinephrine::signalVigilance(float vigilanceLevel) {
    // TODO PHASE 2: Implement real vigilance signaling
    // PLACEHOLDER: Norepinephrine enhances vigilance and focus
    pImpl->level = std::max(pImpl->level, vigilanceLevel * pImpl->releaseRate * 0.8f);
}

} // namespace nlm
