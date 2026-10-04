#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Phasic and tonic components
    float phasic;
    float tonic;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , phasic(0.0f), tonic(0.0f) {}
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
    // More biologically realistic: dopamine modulates plasticity
    // - Low dopamine (0.0-0.3): decreased plasticity
    // - Medium dopamine (0.3-0.7): normal plasticity
    // - High dopamine (0.7-1.0): increased plasticity for learning
    float plasticityMod = 1.0f;
    
    if (pImpl->level < 0.3f) {
        plasticityMod = 0.5f + pImpl->level;  // Increasing from low baseline
    } else if (pImpl->level > 0.7f) {
        plasticityMod = 1.0f + (pImpl->level - 0.7f) * 2.0f;  // Enhancing learning
    }
    
    return std::clamp(plasticityMod, 0.2f, 3.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // More realistic dopamine dynamics
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->tonic = std::max(pImpl->baseline, pImpl->tonic - pImpl->decayRate * dtFloat);
    pImpl->phasic = std::max(0.0f, pImpl->phasic - pImpl->decayRate * dtFloat * 2.0f);
    
    // Combine phasic and tonic components
    pImpl->level = pImpl->tonic + pImpl->phasic * 0.5f;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // More realistic reward signaling
    // Immediate phasic response to reward
    if (reward > 0.0f) {
        pImpl->phasic = std::max(pImpl->phasic, reward * 1.5f);
    } else if (reward < 0.0f) {
        // Negative prediction error
        pImpl->phasic = std::min(pImpl->phasic, reward * 0.8f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    // Positive error (surprise): increase level
    // Negative error (disappointment): decrease level
    pImpl->phasic = std::max(0.0f, pImpl->phasic + error * pImpl->releaseRate * 0.8f);
}

float Dopamine::getRewardPredictionError() const {
    // Compute RPE as difference from baseline
    return std::max(0.0f, pImpl->level - pImpl->baseline);
}

// Acetylcholine implementations
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float decayRate;
    
    // Attention strength
    float attentionStrength;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.05f), attentionStrength(1.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->attentionStrength = 0.5f + pImpl->level * 0.5f;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates attention and memory consolidation
    return 1.0f + pImpl->level * 0.5f;  // Enhance with level
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
}

void Acetylcholine::modulateAttention(float strength) {
    pImpl->attentionStrength = std::clamp(strength, 0.0f, 1.0f);
}

void Acetylcholine::modulateMemoryConsolidation(float strength) {
    // Higher ACh enhances memory consolidation
    pImpl->level = std::min(pImpl->level + strength * 0.2f, 1.0f);
}

// Norepinephrine implementations
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float decayRate;
    
    // Arousal and alertness
    float arousalLevel;
    float vigilance;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.08f), arousalLevel(1.0f), vigilance(1.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->arousalLevel = 0.5f + pImpl->level * 0.5f;
    pImpl->vigilance = 1.0f + pImpl->level;
}

float Norepinephrine::getPlasticityFactor() const {
    // NE affects arousal and attention
    return 1.0f + pImpl->level * 0.3f;
}

void Norepinephrine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
}

void Norepinephrine::modulateArousal(float strength) {
    pImpl->arousalLevel = std::clamp(strength, 0.0f, 1.0f);
}

void Norepinephrine::modulateVigilance(float strength) {
    pImpl->vigilance = std::clamp(strength, 1.0f, 2.0f);
}

// Serotonin implementations
struct Serotonin::Impl {
    float level;
    float baseline;
    float decayRate;
    
    // Mood and impulsivity
    float moodLevel;
    float impulsivity;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.06f), moodLevel(1.0f), impulsivity(1.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->moodLevel = 0.5f + pImpl->level * 0.5f;
    pImpl->impulsivity = std::clamp(1.0f + pImpl->level, 0.5f, 2.0f);
}

float Serotonin::getPlasticityFactor() const {
    // 5-HT affects mood and impulsivity
    return 1.0f + pImpl->level * 0.2f;
}

void Serotonin::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
}

void Serotonin::modulateMood(float strength) {
    pImpl->moodLevel = std::clamp(strength, 0.0f, 1.0f);
}

void Serotonin::modulateImpulsivity(float strength) {
    pImpl->impulsivity = std::clamp(strength, 0.5f, 2.0f);
}

} // namespace nlm
