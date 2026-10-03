#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

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
    return 0.5f + 0.5f * pImpl->level + 0.1f * (pImpl->predictionError ? pImpl->predictionError->getPredictionError() : 0.0f);
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

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionStrength;
    float memoryConsolidation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(0.8f), 
              attentionStrength(0.5f), memoryConsolidation(0.0f) {}
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
    // ACh enhances plasticity for attention-relevant synapses
    // Attention modulation: stronger attention → more plasticity
    float attentionMod = pImpl->attentionStrength * pImpl->level;
    // Memory consolidation: higher ACh improves long-term memory
    float consolidationMod = 1.0f + pImpl->memoryConsolidation * 0.3f;
    return 1.0f * (1.0f + attentionMod) * consolidationMod;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Slowly decay memory consolidation
    pImpl->memoryConsolidation = std::max(0.0f, pImpl->memoryConsolidation - 0.01f * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float attentionStrength) {
    pImpl->attentionStrength = std::clamp(attentionStrength, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->releaseRate * attentionStrength);
}

void Acetylcholine::consolidateMemory(float memoryStrength) {
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation + memoryStrength * 0.5f);
}

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float stressResponse;
    float workingMemoryCapacity;
    float vigilance;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.08f), releaseRate(0.7f), 
              arousalLevel(0.5f), stressResponse(0.0f), workingMemoryCapacity(1.0f), vigilance(0.5f) {}
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
    // NE modulates plasticity based on arousal and vigilance
    // Stress response: high NE increases plasticity for salient events
    float stressMod = 1.0f + pImpl->stressResponse * 0.2f;
    // Vigilance: high vigilance enhances plasticity for attention-relevant tasks
    float vigilanceMod = 1.0f + pImpl->vigilance * 0.15f;
    return 1.0f * stressMod * vigilanceMod;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Stress response decays over time
    pImpl->stressResponse = std::max(0.0f, pImpl->stressResponse - 0.02f * static_cast<float>(dt));
    
    // Vigilance drifts towards baseline
    pImpl->vigilance = 0.5f + (pImpl->vigilance - 0.5f) * 0.95f;
}

void Norepinephrine::increaseArousal(float arousal) {
    pImpl->arousalLevel = std::clamp(arousal, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->releaseRate * arousal * 0.8f);
}

void Norepinephrine::triggerStressResponse(float stress) {
    pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + stress * 0.3f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + stress * pImpl->releaseRate * 1.2f);
}

void Norepinephrine::enhanceVigilance(float vigilance) {
    pImpl->vigilance = std::clamp(vigilance, 0.0f, 1.0f);
}

void Norepinephrine::adjustWorkingMemoryCapacity(float capacity) {
    pImpl->workingMemoryCapacity = std::clamp(capacity, 0.5f, 2.0f);
}

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodState;
    float impulsivity;
    float socialBehavior;
    float anxiety;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.06f), releaseRate(0.6f), 
              moodState(0.5f), impulsivity(0.5f), socialBehavior(0.5f), anxiety(0.0f) {}
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
    // 5-HT modulates plasticity based on mood and social behavior
    // Impulsivity: high impulsivity increases plasticity for novel experiences
    float impulsivityMod = 1.0f - pImpl->impulsivity * 0.1f; // Higher impulsivity = more plasticity
    // Social behavior: social engagement modulates plasticity
    float socialMod = 1.0f + pImpl->socialBehavior * 0.1f;
    return 1.0f * impulsivityMod * socialMod;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Anxiety decays over time
    pImpl->anxiety = std::max(0.0f, pImpl->anxiety - 0.01f * static_cast<float>(dt));
    
    // Mood state drifts towards baseline
    pImpl->moodState = 0.5f + (pImpl->moodState - 0.5f) * 0.95f;
}

void Serotonin::improveMood(float mood) {
    pImpl->moodState = std::clamp(mood, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->releaseRate * mood * 0.8f);
}

void Serotonin::reduceImpulsivity(float control) {
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - control * 0.3f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + control * pImpl->releaseRate * 0.5f);
}

void Serotonin::enhanceSocialBehavior(float social) {
    pImpl->socialBehavior = std::clamp(social, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + social * pImpl->releaseRate * 0.7f);
}

void Serotonin::reduceAnxiety(float anxiety) {
    pImpl->anxiety = std::max(0.0f, pImpl->anxiety - anxiety);
}

} // namespace nlm
