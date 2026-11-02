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

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm

// Acetylcholine: Attention and memory consolidation
struct Acetylcholine::Impl {
    float level;
    float attentionWeight;
    float memoryWeight;
    float decayRate;
    
    Impl() : level(0.0f), attentionWeight(0.5f), memoryWeight(0.3f), decayRate(0.1f) {}
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
    // Acetylcholine modulates plasticity: enhances LTP, reduces LTD
    return 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionLevel) {
    pImpl->level = std::min(1.0f, pImpl->level + attentionLevel * 0.5f);
}

void Acetylcholine::enhanceMemoryConsolidation(float strength) {
    // Acetylcholine strongly promotes memory consolidation
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.8f);
}

// Norepinephrine: Arousal and vigilance
struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float locusCoeruleusActivity;
    float decayRate;
    
    Impl() : level(0.0f), arousalLevel(0.5f), locusCoeruleusActivity(1.0f), decayRate(0.15f) {}
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
    // Norepinephrine has complex effects: enhances some learning, impairs others
    return 1.0f + pImpl->level * 0.3f - pImpl->level * pImpl->level * 0.2f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // More complex dynamics with locus coeruleus activity
    float targetLevel = pImpl->arousalLevel;
    pImpl->level += (targetLevel - pImpl->level) * pImpl->locusCoeruleusActivity * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Norepinephrine::signalStress(float stressLevel) {
    pImpl->level = std::min(1.0f, pImpl->level + stressLevel * 0.7f);
}

void Norepinephrine::increaseVigilance(float intensity) {
    pImpl->level = std::min(1.0f, pImpl->level + intensity * 0.6f);
}

// Serotonin: Mood, impulsivity, and social behavior
struct Serotonin::Impl {
    float level;
    float wellBeingLevel;
    float impulsivityLevel;
    float socialCohesionLevel;
    float decayRate;
    
    Impl() : level(0.0f), wellBeingLevel(0.5f), impulsivityLevel(0.3f), 
             socialCohesionLevel(0.8f), decayRate(0.08f) {}
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
    // Serotonin modulates learning based on mood and context
    return 1.0f + pImpl->wellBeingLevel * 0.2f - pImpl->impulsivityLevel * 0.3f;
}

void Serotonin::update(TimestepDuration dt) {
    // Complex social and emotional dynamics
    float targetLevel = pImpl->wellBeingLevel * pImpl->socialCohesionLevel;
    
    // Decay with occasional bursts from social interactions
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Can be influenced by social interactions (placeholder)
    // In real implementation, this would respond to social learning events
}

void Serotonin::signalWellBeing(float wellBeing) {
    pImpl->level = std::min(1.0f, pImpl->level + wellBeing * 0.6f);
}

void Serotonin::modulateSocialBehavior(float socialCohesion) {
    pImpl->socialCohesionLevel = std::max(0.1f, socialCohesion);
}

void Serotonin::regulateImpulsivity(float controlLevel) {
    pImpl->impulsivityLevel = std::max(0.1f, controlLevel);
}

} // namespace nlm
