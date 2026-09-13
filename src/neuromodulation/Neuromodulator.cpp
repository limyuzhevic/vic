#pragma once

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Add implementations for all neuromodulators that need to be defined in the .cpp file

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float error;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f), error(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

void Dopamine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Dopamine system initialized (Reward and Reinforcement)");
}

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
    pImpl->error = error;
}

float Dopamine::getError() const {
    return pImpl->error;
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionMod;
    float memoryConsolidation;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.05f), releaseRate(1.0f), attentionMod(0.0f),
             memoryConsolidation(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized (Attention and Memory)");
}

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
    float attentionEffect = pImpl->attentionMod * 0.5f;
    float memoryEffect = pImpl->memoryConsolidation * 0.3f;
    return 0.5f + attentionEffect + memoryEffect;
}

void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    pImpl->attentionMod += (pImpl->level - pImpl->attentionMod) * 0.1f;
    pImpl->memoryConsolidation += pImpl->level * 0.05f;
}

void Acetylcholine::enhanceAttention(float stimulusStrength) {
    float attentionBoost = stimulusStrength * pImpl->releaseRate * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionBoost);
}

void Acetylcholine::initiateMemoryConsolidation() {
    pImpl->memoryConsolidation = pImpl->level * 0.8f;
}

void Acetylcholine::modulateWorkingMemory() {
    if (pImpl->brain && pImpl->brain->getWorkingMemory()) {
        float modulation = pImpl->level * 0.3f;
    }
}

void Acetylcholine::signalAttentionShift() {
    if (pImpl->level > 0.5f) {
        pImpl->memoryConsolidation = pImpl->level * 0.3f;
    }
}

void Acetylcholine::decayMemoryTrace() {
    pImpl->memoryConsolidation *= 0.95f;
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilance;
    float stressResponse;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.03f), releaseRate(1.0f), arousalLevel(0.0f),
             vigilance(0.0f), stressResponse(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine system initialized (Arousal and Vigilance)");
}

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
    float arousalEffect = pImpl->arousalLevel * 0.4f;
    float vigilanceEffect = pImpl->vigilance * 0.3f;
    float stressEffect = pImpl->stressResponse * 0.2f;
    return 0.5f + arousalEffect + vigilanceEffect + stressEffect;
}

void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    pImpl->arousalLevel += (pImpl->level - pImpl->arousalLevel) * 0.1f;
    pImpl->vigilance += pImpl->level * 0.05f;
    pImpl->stressResponse += pImpl->arousalLevel * 0.02f;
}

void Norepinephrine::increaseArousal(float stimulusIntensity) {
    float arousalBoost = stimulusIntensity * pImpl->releaseRate * 0.7f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalBoost);
}

void Norepinephrine::maintainVigilance(float duration) {
    float vigilanceMaintenance = duration * pImpl->releaseRate * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + vigilanceMaintenance);
}

void Norepinephrine::respondToStress(float stressLevel) {
    float stressResponse = stressLevel * pImpl->releaseRate * 0.8f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressResponse);
    pImpl->stressResponse = std::min(1.0f, pImpl->stressResponse + stressLevel * 0.5f);
}

void Norepinephrine::enhanceSensoryProcessing() {
    if (pImpl->level > 0.5f) {
        pImpl->vigilance = std::min(1.0f, pImpl->vigilance + 0.2f);
    }
}

// Serotonin implementation
struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float impulsivity;
    float socialBehavior;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f),
             decayRate(0.04f), releaseRate(1.0f), moodLevel(0.0f),
             impulsivity(0.0f), socialBehavior(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Serotonin system initialized (Mood and Social Behavior)");
}

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
    float moodEffect = pImpl->moodLevel * 0.4f;
    float impulsivityEffect = pImpl->impulsivity * 0.3f;
    float socialEffect = pImpl->socialBehavior * 0.2f;
    return 0.5f - moodEffect - impulsivityEffect + socialEffect;
}

void Serotonin::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    pImpl->moodLevel += (pImpl->level - pImpl->moodLevel) * 0.15f;
    pImpl->impulsivity += pImpl->moodLevel * 0.02f;
    pImpl->socialBehavior += pImpl->level * 0.03f;
}

void Serotonin::improveMood(float positiveInput) {
    float moodBoost = positiveInput * pImpl->releaseRate * 0.6f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodBoost);
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - positiveInput * 0.3f);
}

void Serotonin::reduceImpulsivity() {
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - pImpl->level * 0.5f);
}

void Serotonin::modulateSocialBehavior(float socialContext) {
    if (socialContext > 0.5f) {
        pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + socialContext * 0.4f);
    }
}

void Serotonin::regulateAggression(float threatLevel) {
    float aggressionResponse = threatLevel * pImpl->releaseRate * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + aggressionResponse);
    pImpl->impulsivity = std::min(1.0f, pImpl->impulsivity + threatLevel * 0.3f);
}

} // namespace nlm
