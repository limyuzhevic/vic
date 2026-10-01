#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float plasticityFactor;
    float rewardPredictionError;
    float recentReward;
    SimulationStep lastRewardTime;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), rewardPredictionError(0.0f), 
             recentReward(0.0f), lastRewardTime(0) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}
Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const { return "Dopamine"; }
float Dopamine::getLevel() const { return pImpl->level; }
void Dopamine::setLevel(float level) { pImpl->level = std::clamp(level, -1.0f, 1.0f); }

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity: positive = learning enhancement, negative = learning suppression
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Dopamine level decays over time
    pImpl->level *= (1.0f - dt * 0.1f);
    
    // Reward prediction error decays
    pImpl->rewardPredictionError *= (1.0f - dt * 0.05f);
}

void Dopamine::signalReward(float reward) {
    // Record reward with temporal integration
    pImpl->recentReward = reward;
    pImpl->level = std::clamp(reward * 2.0f, -1.0f, 1.0f);  // Scale reward to dopamine range
    pImpl->lastRewardTime = 0;  // Reset for tracking
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error drives learning
    pImpl->rewardPredictionError = std::clamp(error, -1.0f, 1.0f);
    
    // Modulate level based on prediction error
    pImpl->level += error * 0.1f;
    pImpl->level = std::clamp(pImpl->level, -1.0f, 1.0f);
}

struct Acetylcholine::Impl {
    float level;
    float attentionStrength;
    float memoryConsolidationStrength;
    float interferenceStrength;
    SimulationStep lastAttentionUpdate;
    
    Impl() : level(0.0f), attentionStrength(0.0f), memoryConsolidationStrength(0.0f),
             interferenceStrength(0.0f), lastAttentionUpdate(0) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}
Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const { return "ACh"; }
float Acetylcholine::getLevel() const { return pImpl->level; }
void Acetylcholine::setLevel(float level) { pImpl->level = std::clamp(level, 0.0f, 1.0f); }

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for memory formation
    return 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Attention strength follows level with some decay
    pImpl->attentionStrength = std::max(0.0f, pImpl->attentionStrength - dt * 0.05f);
    pImpl->memoryConsolidationStrength = std::max(0.0f, pImpl->memoryConsolidationStrength - dt * 0.02f);
    pImpl->interferenceStrength = std::max(0.0f, pImpl->interferenceStrength - dt * 0.01f);
}

void Acetylcholine::enhanceAttention(float strength) {
    // Increase attentional focus
    pImpl->attentionStrength = std::min(1.0f, pImpl->attentionStrength + strength * 0.5f);
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.2f);
}

void Acetylcholine::promoteMemoryConsolidation(float strength) {
    // Enhance memory consolidation processes
    pImpl->memoryConsolidationStrength = std::min(1.0f, pImpl->memoryConsolidationStrength + strength * 0.8f);
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.3f);
}

void Acetylcholine::reduceMemoryInterference(float strength) {
    // Reduce interference between memory traces
    pImpl->interferenceStrength = std::max(0.0f, pImpl->interferenceStrength - strength * 0.7f);
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.1f);
}

struct Norepinephrine::Impl {
    float level;
    float arousalLevel;
    float workingMemoryStrength;
    float attentionSwitching;
    float mindWandering;
    
    Impl() : level(0.0f), arousalLevel(0.0f), workingMemoryStrength(0.0f),
             attentionSwitching(0.0f), mindWandering(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}
Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const { return "NE"; }
float Norepinephrine::getLevel() const { return pImpl->level; }
void Norepinephrine::setLevel(float level) { pImpl->level = std::clamp(level, 0.0f, 2.0f); }

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity for arousal and learning
    return 1.0f + pImpl->level * 0.3f;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Arousal follows level with decay
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - dt * 0.08f);
    pImpl->workingMemoryStrength = std::max(0.0f, pImpl->workingMemoryStrength - dt * 0.03f);
    pImpl->attentionSwitching = std::max(0.0f, pImpl->attentionSwitching - dt * 0.1f);
    pImpl->mindWandering = std::max(0.0f, pImpl->mindWandering - dt * 0.05f);
}

void Norepinephrine::increaseArousal(float strength) {
    // Increase overall arousal and alertness
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + strength * 0.7f);
    pImpl->level = std::min(2.0f, pImpl->level + strength * 0.3f);
}

void Norepinephrine::enhanceWorkingMemory(float strength) {
    // Boost working memory capacity and maintenance
    pImpl->workingMemoryStrength = std::min(1.0f, pImpl->workingMemoryStrength + strength * 0.6f);
    pImpl->level = std::min(2.0f, pImpl->level + strength * 0.2f);
}

void Norepinephrine::modulateAttentionSwitching(float strength) {
    // Enhance ability to switch between tasks/attention
    pImpl->attentionSwitching = std::min(1.0f, pImpl->attentionSwitching + strength * 0.5f);
    pImpl->level = std::min(2.0f, pImpl->level + strength * 0.2f);
}

void Norepinephrine::reduceMindWandering(float strength) {
    // Reduce mind-wandering and improve focus
    pImpl->mindWandering = std::max(0.0f, pImpl->mindWandering - strength * 0.8f);
    pImpl->level = std::min(2.0f, pImpl->level + strength * 0.1f);
}

struct Serotonin::Impl {
    float level;
    float moodLevel;
    float impulsivityLevel;
    float socialBehavior;
    float behavioralFlexibility;
    
    Impl() : level(0.0f), moodLevel(0.0f), impulsivityLevel(0.0f),
             socialBehavior(0.0f), behavioralFlexibility(0.0f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}
Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const { return "5-HT"; }
float Serotonin::getLevel() const { return pImpl->level; }
void Serotonin::setLevel(float level) { pImpl->level = std::clamp(level, 0.0f, 1.5f); }

float Serotonin::getPlasticityFactor() const {
    // 5-HT modulates plasticity for mood and behavioral regulation
    return 1.0f + pImpl->level * 0.2f;
}

void Serotonin::update(TimestepDuration dt) {
    // Mood level follows level with decay
    pImpl->moodLevel = std::max(0.0f, pImpl->moodLevel - dt * 0.06f);
    pImpl->impulsivityLevel = std::max(0.0f, pImpl->impulsivityLevel - dt * 0.04f);
    pImpl->socialBehavior = std::max(0.0f, pImpl->socialBehavior - dt * 0.05f);
    pImpl->behavioralFlexibility = std::max(0.0f, pImpl->behavioralFlexibility - dt * 0.07f);
}

void Serotonin::improveMood(float strength) {
    // Enhance mood regulation and positive affect
    pImpl->moodLevel = std::min(1.0f, pImpl->moodLevel + strength * 0.8f);
    pImpl->level = std::min(1.5f, pImpl->level + strength * 0.4f);
}

void Serotonin::reduceImpulsivity(float strength) {
    // Reduce impulsive behavior and increase patience
    pImpl->impulsivityLevel = std::max(0.0f, pImpl->impulsivityLevel - strength * 0.7f);
    pImpl->level = std::min(1.5f, pImpl->level + strength * 0.3f);
}

void Serotonin::modulateSocialBehavior(float strength) {
    // Enhance social behavior and cooperation
    pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + strength * 0.6f);
    pImpl->level = std::min(1.5f, pImpl->level + strength * 0.2f);
}

void Serotonin::enhanceBehavioralFlexibility(float strength) {
    // Increase behavioral flexibility and adaptation
    pImpl->behavioralFlexibility = std::min(1.0f, pImpl->behavioralFlexibility + strength * 0.5f);
    pImpl->level = std::min(1.5f, pImpl->level + strength * 0.2f);
}

} // namespace nlm
