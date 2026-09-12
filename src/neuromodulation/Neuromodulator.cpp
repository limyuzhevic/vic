#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

// ========== Dopamine Implementation (complete) ==========

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

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

// ========== Acetylcholine Implementation ==========

struct Acetylcholine::Impl {
    float level;
    float attentionBoost;
    float memoryConsolidation;
    
    Impl() : level(0.0f), attentionBoost(1.0f), memoryConsolidation(1.0f) {}
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
    pImpl->level = level;
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine modulates plasticity for memory consolidation
    return pImpl->memoryConsolidation;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: slow rise/fall, supports attention and memory
    pImpl->level = std::min(1.0f, pImpl->level + 0.05f * dt);
    pImpl->level = std::max(0.0f, pImpl->level - 0.1f * dt);
}

void Acetylcholine::enhanceAttention(float amount) {
    pImpl->attentionBoost = std::min(2.0f, pImpl->attentionBoost + amount);
}

void Acetylcholine::consolidateMemory() {
    pImpl->memoryConsolidation = std::min(2.0f, pImpl->memoryConsolidation + 0.1f);
}

// ========== Serotonin Implementation ==========

struct Serotonin::Impl {
    float level;
    float mood;
    float impulsivity;
    float socialBehavior;
    
    Impl() : level(0.0f), mood(0.5f), impulsivity(0.5f), socialBehavior(0.5f) {}
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
    pImpl->level = level;
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity, especially for social learning
    return pImpl->socialBehavior;
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics: affects mood and social behavior
    pImpl->level = std::min(1.0f, pImpl->level + 0.03f * dt);
    pImpl->level = std::max(0.0f, pImpl->level - 0.15f * dt);
}

void Serotonin::setMood(float mood) {
    pImpl->mood = mood;
}

void Serotonin::setImpulsivity(float impulsivity) {
    pImpl->impulsivity = impulsivity;
}

void Serotonin::enhanceSocialLearning(float amount) {
    pImpl->socialBehavior = std::min(2.0f, pImpl->socialBehavior + amount);
}

// ========== Norepinephrine Implementation ==========

struct Norepinephrine::Impl {
    float level;
    float arousal;
    float alertness;
    float vigilance;
    
    Impl() : level(0.0f), arousal(0.5f), alertness(0.5f), vigilance(0.5f) {}
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
    pImpl->level = level;
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates plasticity for arousal and vigilance
    return pImpl->arousal * pImpl->vigilance;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Norepinephrine dynamics: increases with stress/arousal, affects focus
    pImpl->level = std::min(1.0f, pImpl->level + 0.08f * dt);
    pImpl->level = std::max(0.0f, pImpl->level - 0.05f * dt);
}

void Norepinephrine::increaseArousal(float amount) {
    pImpl->arousal = std::min(2.0f, pImpl->arousal + amount);
}

void Norepinephrine::increaseAlertness(float amount) {
    pImpl->alertness = std::min(2.0f, pImpl->alertness + amount);
}

void Norepinephrine::setVigilance(float vigilance) {
    pImpl->vigilance = vigilance;
}

} // namespace nlm
