#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// ========== DOPAMINE ==========
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
    // Real dopamine-modulated plasticity factor
    // Dopamine enhances synaptic plasticity for reward-associated learning
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics
    // Decay towards baseline, modulated by reward prediction error
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling
    // Rapid burst of dopamine on unexpected reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Dopamine responds to prediction error (positive for better-than-expected, negative for worse)
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

// ========== ACETYLCHOLINE ==========
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention modulation state
    float attentionStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , attentionStrength(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances plasticity for attention-focused learning
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay attention strength
    pImpl->attentionStrength *= (1.0f - dt * 0.05f);
}

void Acetylcholine::enhanceAttention(float strength) {
    pImpl->attentionStrength = std::min(1.0f, pImpl->attentionStrength + strength);
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.5f);
}

void Acetylcholine::consolidateMemory(float strength) {
    // Acetylcholine signals during active learning strengthen memory consolidation
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.5f);
}

// ========== NOREPINEPHRINE ==========
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Arousal modulation state
    float arousalLevel;
    
    // Working memory enhancement state
    float workingMemoryBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , arousalLevel(0.0f), workingMemoryBoost(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates attention and arousal, affecting plasticity indirectly
    return 0.8f + 0.2f * pImpl->level;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay arousal and working memory boost
    pImpl->arousalLevel *= (1.0f - dt * 0.03f);
    pImpl->workingMemoryBoost *= (1.0f - dt * 0.02f);
}

void Norepinephrine::increaseArousal(float strength) {
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + strength * 0.5f);
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.3f);
}

void Norepinephrine::enhanceWorkingMemory(float strength) {
    pImpl->workingMemoryBoost = std::min(1.0f, pImpl->workingMemoryBoost + strength);
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.2f);
}

// ========== SEROTONIN ==========
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Mood modulation state
    float moodLevel;
    
    // Social behavior modulation state
    float socialBehavior;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , moodLevel(0.0f), socialBehavior(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin affects mood and social behavior, modulating plasticity
    return 0.7f + 0.3f * pImpl->level;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay mood and social behavior
    pImpl->moodLevel *= (1.0f - dt * 0.04f);
    pImpl->socialBehavior *= (1.0f - dt * 0.04f);
}

void Serotonin::modulateMood(float strength) {
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + strength * 0.5f);
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.3f);
}

void Serotonin::modulateSocialBehavior(float strength) {
    pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + strength);
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.2f);
}

} // namespace nlm
