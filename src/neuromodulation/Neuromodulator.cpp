#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// ======= ACETYCHOLINE =======

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionBoost;
    float memoryConsolidationBoost;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), 
             attentionBoost(0.5f), memoryConsolidationBoost(0.8f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Real implementation: ACh enhances attention and memory consolidation
    return 1.0f + pImpl->level * pImpl->attentionBoost;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline with some stabilization
    float targetLevel = (pImpl->baseline > 0.0f) ? pImpl->baseline : 0.1f;
    pImpl->level = std::max(targetLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float strength) {
    // Real implementation: ACh boosts attentional selection
    float oldLevel = pImpl->level;
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->attentionBoost);
}

void Acetylcholine::boostMemoryConsolidation() {
    // Real implementation: ACh enhances memory consolidation
    // This affects synaptic strengthening and memory stabilization
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f * pImpl->memoryConsolidationBoost);
}

// ======= NOREPINEPHRINE =======

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalBoost;
    float vigilanceBoost;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f),
             arousalBoost(0.6f), vigilanceBoost(0.7f) {}
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
    // Real implementation: NE increases arousal and vigilance
    return 1.0f + pImpl->level * (pImpl->arousalBoost + pImpl->vigilanceBoost) * 0.5f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    float targetLevel = (pImpl->baseline > 0.0f) ? pImpl->baseline : 0.1f;
    pImpl->level = std::max(targetLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// ======= SEROTONIN =======

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodBoost;
    float impulsivityModulation;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.08f),
             moodBoost(0.4f), impulsivityModulation(0.5f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Real implementation: 5-HT affects mood and impulsivity
    return 1.0f + pImpl->level * pImpl->moodBoost;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline with slower decay
    float targetLevel = (pImpl->baseline > 0.0f) ? pImpl->baseline : 0.05f;
    pImpl->level = std::max(targetLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// ======= DOPAMINE =======

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
    // Real implementation: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
