#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

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

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: burst of dopamine proportional to reward prediction error
    // More reward -> larger dopamine burst
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling: dopamine encodes RPE
    // Positive error (better than expected) -> increase dopamine
    // Negative error (worse than expected) -> decrease dopamine
    float delta = error * pImpl->releaseRate * 2.0f;
    pImpl->level = std::clamp(pImpl->level + delta, 0.0f, pImpl->peak);
}
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.1f), peak(0.5f), decayRate(0.05f), releaseRate(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity: low levels enhance LTP, high levels enhance LTD
    // This helps with behavioral flexibility and adaptation
    if (pImpl->level < 0.3f) {
        return 1.5f + pImpl->level;  // Enhance LTP
    } else {
        return 1.0f - (pImpl->level - 0.3f);  // Enhance LTD
    }
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: baseline decay with occasional bursts
    // Represents mood regulation and behavioral inhibition
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Serotonin::signalInhibition(float inhibition) {
    // Real serotonin signaling: inhibition and mood regulation
    // Negative events increase serotonin to promote avoidance
    pImpl->level = std::min(pImpl->peak, pImpl->level + inhibition * pImpl->releaseRate * 1.5f);
}
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), attentionGain(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates plasticity: arousal enhances learning
    // Higher arousal (NE) increases plasticity and attention
    float attentionMod = 1.0f + pImpl->attentionGain * pImpl->level;
    return std::clamp(attentionMod, 0.1f, 3.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Real norepinephrine dynamics: fast arousal signal
    // Represents alertness and attention modulation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalAttention(float attention) {
    // Real norepinephrine signaling: attention and arousal
    // Novel or surprising events increase NE to enhance attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate * 2.0f);
}

void Norepinephrine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::clamp(gain, 0.0f, 2.0f);
}
}

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.08f), releaseRate(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine modulates plasticity: attention and memory consolidation
    // ACh enhances learning during attention and consolidates memories
    float memoryMod = 0.5f + pImpl->level;  // More ACh = better memory consolidation
    return std::clamp(memoryMod, 0.5f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Real acetylcholine dynamics: attentional and memory modulation
    // Represents attention allocation and memory consolidation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // Real acetylcholine signaling: attention and memory encoding
    // Pay attention to important events, consolidate into memory
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate * 1.5f);
}

void Acetylcholine::signalMemory(float memoryStrength) {
    // Real acetylcholine signaling: memory consolidation
    // Strengthen memory traces through acetylcholine signaling
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryStrength * pImpl->releaseRate * 0.8f);
}
