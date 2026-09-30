#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float attentionGain;
    float workingMemoryModulator;
    
    Impl() : level(0.0f), baseline(0.02f), peak(1.0f),
             decayRate(0.08f), releaseRate(3.0f),
             arousalLevel(1.0f), attentionGain(1.0f),
             workingMemoryModulator(1.0f) {}
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
    // Norepinephrine modulates arousal and attention
    // Higher NE increases attention gain and learning under certain conditions
    // but can impair memory if too high (too much arousal)
    float arousalMod = 1.0f + pImpl->level * 0.5f;
    float attentionMod = 1.0f + pImpl->attentionGain * pImpl->level * 0.8f;
    float wmMod = pImpl->workingMemoryModulator * (1.0f - pImpl->level * 0.3f);
    
    float factor = arousalMod * attentionMod * wmMod;
    return std::clamp(factor, 0.5f, 3.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update associated modulators
    pImpl->arousalLevel = std::clamp(1.0f + pImpl->level * 1.5f, 1.0f, 3.0f);
    pImpl->attentionGain = std::clamp(1.0f + pImpl->level * 0.8f, 1.0f, 2.0f);
    pImpl->workingMemoryModulator = std::clamp(1.5f - pImpl->level * 0.4f, 0.8f, 2.0f);
}

void Norepinephrine::signalArousal(float arousalDelta) {
    // NE responds to novel stimuli and unexpected events
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalDelta * pImpl->releaseRate);
}

void Norepinephrine::enhanceAttention(float baseAttention) {
    // NE enhances attention gain based on current level
    float enhancement = 1.0f + pImpl->level * pImpl->attentionGain * 0.6f;
    return baseAttention * enhancement;
}

void Norepinephrine::boostWorkingMemory(float baseCapacity) {
    // Modulate working memory capacity based on NE level
    float capacityMod = baseCapacity * pImpl->workingMemoryModulator;
}

} // namespace nlm