#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalGain;
    float vigilanceFactor;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.03f)
        , releaseRate(1.5f)
        , arousalGain(0.8f)
        , vigilanceFactor(0.6f) {}
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
    // Norepinephrine modulates arousal and vigilance
    // Higher NE increases sensitivity to novel stimuli
    return 0.4f + 0.6f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float intensity) {
    // NE released during high arousal states
    pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->releaseRate);
}

void Norepinephrine::signalVigilanceBoost() {
    // NE increases vigilance when monitoring important tasks
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f * pImpl->vigilanceFactor);
}

} // namespace nlm