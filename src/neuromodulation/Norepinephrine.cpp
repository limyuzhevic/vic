#include "Norepinephrine.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float decayRate;
    float arousalRate;
    float locusCoeruleusInput;
    
    Impl() : level(0.0f), baseline(0.05f), decayRate(0.1f), arousalRate(3.0f), locusCoeruleusInput(1.0f) {}
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
    // PLACEHOLDER: NE modulates arousal and attention
    return 1.0f + pImpl->level * 0.3f;  // NE increases arousal and plasticity for novel stimuli
}

void Norepinephrine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real norepinephrine dynamics
    // PLACEHOLDER: Decay with moderate time constant
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalNovelty(float noveltySignal) {
    // TODO PHASE 2: Implement norepinephrine signaling for novelty detection
    // PLACEHOLDER: NE bursts with novel stimuli
    pImpl->level = std::min(1.0f, pImpl->level + noveltySignal * pImpl->arousalRate);
}

void Norepinephrine::signalArousal(float arousalLevel) {
    // TODO PHASE 2: Implement norepinephrine effects on arousal and vigilance
    // PLACEHOLDER: NE increases arousal and alertness
    pImpl->level = std::min(1.0f, pImpl->level + arousalLevel * pImpl->arousalRate * 0.5f);
}

} // namespace nlm