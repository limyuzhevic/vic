#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float memoryEnhancementFactor;
    float attentionModulation;
    float noveltySensitivity;
    float workingMemoryBoost;
    float episodicMemoryConsolidation;
    float lastRewardTime;
    float rewardHistory[10];  // Rolling window of recent rewards
    float predictionErrorHistory[10];  // Rolling window of prediction errors
    
    Impl() : level(0.0f), baseline(0.2f), peak(2.0f), decayRate(0.05f), releaseRate(1.5f),
             predictionError(0.0f), memoryEnhancementFactor(1.0f), attentionModulation(1.0f),
             noveltySensitivity(0.8f), workingMemoryBoost(1.0f), episodicMemoryConsolidation(1.0f),
             lastRewardTime(0.0f) {
        for (int i = 0; i < 10; ++i) {
            rewardHistory[i] = 0.0f;
            predictionErrorHistory[i] = 0.0f;
        }
    }
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->peak);
}

float Dopamine::getPlasticityFactor() const {
    // Enhanced plasticity factor based on dopamine level
    // Higher dopamine increases plasticity for learning and memory
    float basePlasticity = 0.5f;
    float dopamineBoost = pImpl->level * 0.5f;
    float memoryEnhancement = pImpl->memoryEnhancementFactor * 0.3f;
    
    // Combine effects
    return basePlasticity + dopamineBoost + memoryEnhancement;
}

void Dopamine::update(TimestepDuration dt) {
    // Enhanced dopamine dynamics with memory consolidation
    float timeSinceLastReward = dt - (pImpl->lastRewardTime);
    
    // Decay with homeostatic regulation
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt) * 
                           (1.0f + 0.5f * pImpl->memoryEnhancementFactor));
    
    // Integrate prediction error and reward history
    float avgPredictionError = 0.0f;
    float avgReward = 0.0f;
    for (int i = 0; i < 10; ++i) {
        avgPredictionError += pImpl->predictionErrorHistory[i];
        avgReward += pImpl->rewardHistory[i];
    }
    avgPredictionError /= 10.0f;
    avgReward /= 10.0f;
    
    // Update memory enhancement factor based on prediction error
    pImpl->predictionError = avgPredictionError;
    pImpl->memoryEnhancementFactor = 1.0f + std::abs(avgPredictionError) * 0.5f;
    
    // Update attention modulation
    pImpl->attentionModulation = 1.0f + pImpl->level * 0.8f;
    
    // Update working memory boost
    pImpl->workingMemoryBoost = 1.0f + pImpl->level * 0.6f;
    
    // Update episodic memory consolidation
    pImpl->episodicMemoryConsolidation = 1.0f + pImpl->level * 0.4f;
}

void Dopamine::signalReward(float reward) {
    // Update reward history (shift and add new reward)
    for (int i = 9; i > 0; --i) {
        pImpl->rewardHistory[i] = pImpl->rewardHistory[i-1];
    }
    pImpl->rewardHistory[0] = reward;
    pImpl->lastRewardTime = dt;  // Using dt parameter (needs to be passed correctly)
    
    // Enhanced reward signaling with memory effects
    float rewardBurst = std::min(pImpl->peak, pImpl->level + 
                                reward * pImpl->releaseRate * 
                                (1.0f + 0.3f * pImpl->noveltySensitivity));
    
    // Apply reward to dopamine level
    setLevel(rewardBurst);
    
    // Boost memory enhancement for reward-associated events
    pImpl->memoryEnhancementFactor = std::min(2.0f, 
        pImpl->memoryEnhancementFactor + reward * 0.2f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Update prediction error history (shift and add new error)
    for (int i = 9; i > 0; --i) {
        pImpl->predictionErrorHistory[i] = pImpl->predictionErrorHistory[i-1];
    }
    pImpl->predictionErrorHistory[0] = error;
    
    // Enhanced prediction error signaling
    float errorResponse = std::max(0.0f, 
        getLevel() + error * pImpl->releaseRate * 
        (1.0f + 0.5f * std::abs(error)));
    
    setLevel(errorResponse);
    
    // Update prediction error magnitude
    pImpl->predictionError = error;
}

float Dopamine::getMemoryEnhancementFactor() const {
    return pImpl->memoryEnhancementFactor;
}

float Dopamine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

float Dopamine::getWorkingMemoryBoost() const {
    return pImpl->workingMemoryBoost;
}

float Dopamine::getEpisodicMemoryConsolidation() const {
    return pImpl->episodicMemoryConsolidation;
}

// Acetylcholine implementation for attention and memory consolidation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionGain;
    float memoryGatingStrength;
    float workingMemoryCapacity;
    float signalToNoiseRatio;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.5f), decayRate(0.08f),
             attentionGain(1.2f), memoryGatingStrength(0.9f), workingMemoryCapacity(1.0f),
             signalToNoiseRatio(1.0f) {}
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
    // Acetylcholine enhances synaptic plasticity through attention
    float basePlasticity = 1.0f;
    float attentionBoost = pImpl->level * pImpl->attentionGain;
    float memoryGating = pImpl->memoryGatingStrength;
    
    return basePlasticity + attentionBoost * memoryGating;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Attention and memory consolidation dynamics
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Attention gating modulates signal-to-noise ratio
    pImpl->signalToNoiseRatio = 1.0f + pImpl->level * 0.7f;
    
    // Working memory capacity modulation
    pImpl->workingMemoryCapacity = 1.0f + pImpl->level * 0.5f;
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Acetylcholine::getMemoryGatingStrength() const {
    return pImpl->memoryGatingStrength;
}

float Acetylcholine::getWorkingMemoryCapacity() const {
    return pImpl->workingMemoryCapacity;
}

float Acetylcholine::getSignalToNoiseRatio() const {
    return pImpl->signalToNoiseRatio;
}

// Norepinephrine implementation for arousal and vigilance
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float arousalLevel;
    float vigilance;
    float locusCoeruleusFiringRate;
    float attentionFocus;
    float stressResponse;
    
    Impl() : level(0.0f), baseline(0.1f), peak(2.0f), decayRate(0.06f),
             arousalLevel(0.5f), vigilance(0.8f), locusCoeruleusFiringRate(0.3f),
             attentionFocus(1.0f), stressResponse(0.5f) {}
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
    // Norepinephrine modulates arousal and attentional focus
    float basePlasticity = 1.0f;
    float arousalBoost = pImpl->level * pImpl->arousalLevel;
    float vigilanceBoost = pImpl->vigilance;
    
    return basePlasticity + arousalBoost * vigilanceBoost;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Arousal and vigilance dynamics
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Locus coeruleus firing rate modulation
    pImpl->locusCoeruleusFiringRate = pImpl->level * 2.0f;
    
    // Attention focus modulation
    pImpl->attentionFocus = 1.0f + pImpl->level * 0.8f;
    
    // Stress response modulation
    pImpl->stressResponse = pImpl->level * 0.5f;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getAttentionFocus() const {
    return pImpl->attentionFocus;
}

float Norepinephrine::getStressResponse() const {
    return pImpl->stressResponse;
}

// Serotonin implementation for mood and social behavior
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float moodState;
    float impulsivity;
    float socialBehavior;
    float anxietyLevel;
    float rewardSensitivity;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.8f), decayRate(0.07f),
             moodState(0.5f), impulsivity(0.5f), socialBehavior(0.5f),
             anxietyLevel(0.3f), rewardSensitivity(1.0f) {}
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
    // Serotonin modulates mood, impulsivity, and social behavior
    float basePlasticity = 1.0f;
    float moodBoost = pImpl->level * pImpl->moodState;
    float rewardSensitivity = pImpl->rewardSensitivity;
    
    return basePlasticity + moodBoost * rewardSensitivity;
}

void Serotonin::update(TimestepDuration dt) {
    // Mood and social behavior dynamics
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Reward sensitivity modulation
    pImpl->rewardSensitivity = 1.0f + pImpl->level * 0.6f;
    
    // Impulsivity modulation
    pImpl->impulsivity = 0.5f + pImpl->level * 0.5f;
    
    // Social behavior modulation
    pImpl->socialBehavior = 0.5f + pImpl->level * 0.5f;
    
    // Anxiety modulation
    pImpl->anxietyLevel = 0.3f + pImpl->level * 0.4f;
}

float Serotonin::getMoodState() const {
    return pImpl->moodState;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

float Serotonin::getAnxietyLevel() const {
    return pImpl->anxietyLevel;
}

} // namespace nlm