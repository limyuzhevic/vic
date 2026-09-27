#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// Dopamine implementation
struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}
Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const { return "DA"; }
float Dopamine::getLevel() const { return pImpl->level; }
void Dopamine::setLevel(float level) { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
float Dopamine::getPlasticityFactor() const { return 0.5f + 0.5f * pImpl->level; }
void Dopamine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}
void Dopamine::signalReward(float reward) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}
void Dopamine::signalRewardPredictionError(float error) {
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionLevel;
    float workingMemoryModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             attentionLevel(0.0f), workingMemoryModulation(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}
Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const { return "ACh"; }
float Acetylcholine::getLevel() const { return pImpl->level; }
void Acetylcholine::setLevel(float level) { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
float Acetylcholine::getPlasticityFactor() const {
    return 0.5f + 0.5f * pImpl->level;
}
void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->attentionLevel = std::max(0.0f, pImpl->attentionLevel - pImpl->decayRate * static_cast<float>(dt) * 2.0f);
    pImpl->workingMemoryModulation = std::max(0.0f, pImpl->workingMemoryModulation - pImpl->decayRate * static_cast<float>(dt) * 1.5f);
}
void Acetylcholine::signalAttention(float attention) {
    pImpl->attentionLevel = std::min(1.0f, pImpl->attentionLevel + attention * pImpl->releaseRate);
}
void Acetylcholine::modulateWorkingMemory(float memoryStrength) {
    pImpl->workingMemoryModulation = std::min(1.0f, pImpl->workingMemoryModulation + memoryStrength * pImpl->releaseRate);
}
float Acetylcholine::getAttentionLevel() const { return pImpl->attentionLevel; }

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float alertnessLevel;
    float noveltyGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f),
             arousalLevel(0.0f), alertnessLevel(0.0f), noveltyGain(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}
Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const { return "NE"; }
float Norepinephrine::getLevel() const { return pImpl->level; }
void Norepinephrine::setLevel(float level) { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
float Norepinephrine::getPlasticityFactor() const {
    return 0.5f + 0.5f * pImpl->level;
}
void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - pImpl->decayRate * static_cast<float>(dt) * 1.2f);
    pImpl->alertnessLevel = std::max(0.0f, pImpl->alertnessLevel - pImpl->decayRate * static_cast<float>(dt) * 1.5f);
    pImpl->noveltyGain = std::max(0.0f, pImpl->noveltyGain - pImpl->decayRate * static_cast<float>(dt) * 1.0f);
}
void Norepinephrine::signalNovelty(float novelty) {
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + novelty * pImpl->releaseRate);
    pImpl->alertnessLevel = std::min(1.0f, pImpl->alertnessLevel + novelty * pImpl->releaseRate * 0.8f);
}
void Norepinephrine::signalAlertness(float alertness) {
    pImpl->alertnessLevel = std::min(1.0f, pImpl->alertnessLevel + alertness * pImpl->releaseRate);
}
float Norepinephrine::getArousalLevel() const { return pImpl->arousalLevel; }
float Norepinephrine::getAlertnessLevel() const { return pImpl->alertnessLevel; }
float Norepinephrine::getNoveltyGain() const { return pImpl->noveltyGain; }

} // namespace nlm
