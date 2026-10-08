#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalModifier;
    float attentionBias;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             arousalModifier(1.0f), attentionBias(1.0f) {}
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

float Norepinephrine::getArousalModifier() const {
    // Norepinephrine modulates arousal - increases vigilance
    return 1.0f + pImpl->level * 0.5f;
}

float Norepinephrine::getAttentionBias() const {
    // Norepinephrine biases attention towards salient stimuli
    return 1.0f + pImpl->level * 0.8f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalNovelty(float novelty) {
    // Norepinephrine responds to novel stimuli
    // Increases level for high novelty
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate);
}

void Norepinephrine::signalStress(float stress) {
    // Norepinephrine responds to stress
    // Increases level for stress
    pImpl->level = std::min(pImpl->peak, pImpl->level + stress * pImpl->releaseRate * 0.8f);
}

void Norepinephrine::signalReward(float reward) {
    // Norepinephrine responds to reward (modulates arousal)
    pImpl->level = std::max(0.0f, pImpl->level + reward * pImpl->releaseRate * 0.3f);
}

float Norepinephrine::getExcitabilityModifier() const {
    // Norepinephrine increases neural excitability
    return 1.0f + pImpl->level * 0.4f;
}

} // namespace nlm