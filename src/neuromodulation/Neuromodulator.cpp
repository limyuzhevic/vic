#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// ================================================
// ACETYLCHOLINE: Attention and Memory Consolidation
// ================================================

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionBoost;
    float memoryConsolidationBoost;
    float cholinergicTone;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             attentionBoost(0.8f), memoryConsolidationBoost(0.6f), cholinergicTone(0.5f) {}
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
    // ACh modulates plasticity in two ways:
    // 1. Increases synaptic strength (learning)
    // 2. Reduces forgetting (memory consolidation)
    float attentionFactor = pImpl->level * pImpl->attentionBoost;
    float consolidationFactor = pImpl->level * pImpl->memoryConsolidationBoost;
    return 1.0f + attentionFactor - 0.2f * (1.0f - consolidationFactor);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics:
    // 1. Decays towards baseline
    // 2. Increases with attentional demands
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Cholinergic tone modulates overall brain state
    pImpl->cholinergicTone = std::max(0.1f, 1.0f - pImpl->level * 0.5f);
}

void Acetylcholine::signalAttention(float attention) {
    // Boost ACh with attentional demands
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate * 2.0f);
}

void Acetylcholine::signalMemoryConsolidation() {
    // ACh promotes memory consolidation during encoding
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f * pImpl->releaseRate);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->cholinergicTone = 0.5f;
}

// ================================================
// NORADRENALINE: Arousal and Vigilance  
// ================================================

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float vigilanceLevel;
    float stressLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(2.0f), decayRate(0.03f), releaseRate(1.5f),
             arousalLevel(0.5f), vigilanceLevel(0.5f), stressLevel(0.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE has inverted U-shaped effect on plasticity:
    // - Low NE: reduced plasticity
    // - Optimal NE: enhanced plasticity
    // - High NE: reduced plasticity (stress)
    float normalizedLevel = pImpl->level / 2.0f; // Normalize to [0, 1]
    float plasticity = std::exp(-std::pow((normalizedLevel - 0.5f) / 0.3f, 2)) * 2.0f;
    return std::clamp(plasticity, 0.5f, 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // NE dynamics:
    // 1. Decays towards baseline
    // 2. Increases with stress and novelty
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update arousal and vigilance
    pImpl->arousalLevel = std::min(1.0f, pImpl->level / 2.0f);
    pImpl->vigilanceLevel = std::max(0.1f, 1.0f - std::pow(pImpl->level / 2.0f, 2));
}

void Norepinephrine::signalStress(float stress) {
    // NE increases with stress
    pImpl->level = std::min(pImpl->peak, pImpl->level + stress * pImpl->releaseRate * 2.0f);
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + stress * 0.3f);
}

void Norepinephrine::signalNovelty(float novelty) {
    // NE increases with novel stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * pImpl->releaseRate);
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalLevel = 0.5f;
    pImpl->vigilanceLevel = 0.5f;
    pImpl->stressLevel = 0.0f;
}

// ================================================
// SEROTONIN: Mood, Impulsivity, and Social Behavior
// ================================================

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float moodLevel;
    float impulsivityLevel;
    float socialBondingLevel;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.5f), decayRate(0.08f), releaseRate(1.2f),
             moodLevel(0.5f), impulsivityLevel(0.5f), socialBondingLevel(0.5f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 1.5f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin has complex effects:
    // - High serotonin: reduced impulsivity, enhanced habit learning
    // - Low serotonin: increased impulsivity, enhanced goal-directed learning
    float normalizedLevel = pImpl->level / 1.5f; // Normalize to [0, 1]
    
    // High serotonin reduces plasticity for impulsive behaviors
    // but may increase plasticity for habitual behaviors
    float plasticity = 1.0f - normalizedLevel * 0.5f + normalizedLevel * 0.2f;
    return std::clamp(plasticity, 0.5f, 1.5f);
}

void Serotonin::update(TimestepDuration dt) {
    // Serotonin dynamics:
    // 1. Decays towards baseline
    // 2. Increases with social interaction and positive experiences
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update mood and social components
    pImpl->moodLevel = std::min(1.0f, pImpl->level);
    pImpl->socialBondingLevel = std::max(0.1f, pImpl->level * 0.8f);
    pImpl->impulsivityLevel = std::max(0.1f, 1.0f - pImpl->level * 0.7f);
}

void Serotonin::signalPositiveExperience(float reward) {
    // Serotonin increases with positive experiences
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 1.5f);
}

void Serotonin::signalSocialBonding(float socialCohesion) {
    // Serotonin increases with social bonding
    pImpl->level = std::min(pImpl->peak, pImpl->level + socialCohesion * pImpl->releaseRate);
    pImpl->socialBondingLevel = std::min(1.0f, pImpl->socialBondingLevel + socialCohesion * 0.3f);
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->moodLevel = 0.5f;
    pImpl->impulsivityLevel = 0.5f;
    pImpl->socialBondingLevel = 0.5f;
}

// ================================================
// DOPAMINE: Enhanced Reward and Reinforcement Learning
// ================================================

struct Dopamine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float rewardHistory;
    float explorationBias;
    
    Impl() : level(0.0f), baseline(0.0f), peak(2.0f), decayRate(0.1f), releaseRate(1.0f),
             predictionError(0.0f), rewardHistory(0.0f), explorationBias(0.5f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine follows a prediction error rule:
    // - Positive prediction error: increases plasticity
    // - Negative prediction error: decreases plasticity
    float rewardPrediction = pImpl->predictionError;
    
    // Standard dopamine learning rule
    float plasticity = 1.0f + rewardPrediction * 0.5f;
    
    // Apply exploration bias
    plasticity *= (1.0f + pImpl->explorationBias * 0.2f);
    
    return std::clamp(plasticity, 0.5f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Dopamine dynamics:
    // 1. Decays towards baseline
    // 2. Updated by reward prediction error
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Maintain running average of reward history
    pImpl->rewardHistory = (pImpl->rewardHistory * 0.9f) + (pImpl->level * 0.1f);
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward prediction
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
    pImpl->rewardHistory = (pImpl->rewardHistory * 0.9f) + reward * 0.1f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine encodes reward prediction error
    pImpl->predictionError = error;
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    
    // Update exploration bias based on prediction error
    if (error > 0.5f) {
        pImpl->explorationBias = std::min(1.0f, pImpl->explorationBias + 0.1f);
    } else if (error < -0.5f) {
        pImpl->explorationBias = std::max(0.0f, pImpl->explorationBias - 0.1f);
    }
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->rewardHistory = 0.0f;
    pImpl->explorationBias = 0.5f;
}

} // namespace nlm
