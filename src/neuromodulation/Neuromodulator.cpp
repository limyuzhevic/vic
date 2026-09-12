#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Acetylcholine Implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float attentionLevel;
    float memoryConsolidation;
    float decayRate;
    float attentionStrength;
    
    Impl() : level(0.0f), baseline(0.05f), attentionLevel(0.5f), memoryConsolidation(0.5f),
             decayRate(0.1f), attentionStrength(1.0f) {}
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
    // ACh modulates plasticity based on attention
    return 0.5f + 0.5f * pImpl->attentionLevel;
}

void Acetylcholine::update(TimestepDuration dt, float predictionError, float reward, float novelty) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Increase attention based on prediction error
    if (predictionError > 0.1f) {
        pImpl->attentionLevel = std::min(1.0f, pImpl->attentionLevel + predictionError * pImpl->attentionStrength * 0.1f);
    }
    
    // Novelty triggers attention increase
    if (novelty > 0.5f) {
        pImpl->attentionLevel = std::min(1.0f, pImpl->attentionLevel + novelty * 0.2f);
    }
    
    // Decay attention and memory consolidation
    pImpl->attentionLevel *= (1.0f - pImpl->decayRate);
    pImpl->memoryConsolidation *= (1.0f - pImpl->decayRate);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionLevel = 0.5f;
    pImpl->memoryConsolidation = 0.5f;
}

void Acetylcholine::modulateAttention(float attentionLevel) {
    pImpl->attentionLevel = std::clamp(attentionLevel, 0.0f, 1.0f);
    // Increase ACh level to support attention
    pImpl->level = std::min(1.0f, pImpl->level + attentionLevel * 0.3f);
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attentionLevel;
}

void Acetylcholine::consolidateMemory(float consolidationStrength) {
    pImpl->memoryConsolidation = std::min(1.0f, consolidationStrength);
    // ACh level increases with memory consolidation demand
    pImpl->level = std::min(1.0f, pImpl->level + consolidationStrength * 0.2f);
}

float Acetylcholine::getMemoryConsolidation() const {
    return pImpl->memoryConsolidation;
}

// Dopamine Implementation
struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    bool burstMode;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.1f),
             releaseRate(1.0f), learningRate(1.0f), burstMode(false) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Higher dopamine increases plasticity for reward prediction errors
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt, float predictionError, float reward, float novelty) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Respond to prediction errors (reward prediction error)
    if (std::abs(predictionError) > 0.01f) {
        pImpl->level = std::max(0.0f, pImpl->level + predictionError * pImpl->releaseRate * 0.5f);
    }
    
    // Burst mode for large prediction errors
    if (std::abs(predictionError) > 0.5f) {
        pImpl->burstMode = true;
        pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * 2.0f);
    } else {
        pImpl->burstMode = false;
    }
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->burstMode = false;
}

void Dopamine::signalReward(float reward) {
    // Reward signals increase dopamine
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signals modulate dopamine
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::signalPrediction(float prediction) {
    // Prediction strength modulates dopamine
    pImpl->level = std::min(1.0f, pImpl->level + prediction * 0.1f);
}

void Dopamine::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 2.0f);
}

float Dopamine::getLearningRate() const {
    return pImpl->learningRate;
}

bool Dopamine::isBurstMode() const {
    return pImpl->burstMode;
}

void Dopamine::setBurstMode(bool enable) {
    pImpl->burstMode = enable;
}

// Norepinephrine Implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float arousalLevel;
    float focusLevel;
    float stressLevel;
    float decayRate;
    float stressDecayRate;
    
    Impl() : level(0.0f), baseline(0.02f), arousalLevel(0.5f), focusLevel(0.5f),
             stressLevel(0.0f), decayRate(0.05f), stressDecayRate(0.02f) {}
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
    // NE modulates plasticity based on arousal and stress
    float stressFactor = pImpl->stressLevel * 0.5f;
    float arousalFactor = pImpl->arousalLevel * 0.3f;
    return 0.5f + stressFactor + arousalFactor;
}

void Norepinephrine::update(TimestepDuration dt, float predictionError, float reward, float novelty) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Increase arousal based on novelty and stress
    if (novelty > 0.3f) {
        pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + novelty * 0.2f);
    }
    
    // Stress builds up with negative reward
    if (reward < -0.1f) {
        pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + std::abs(reward) * 0.3f);
    }
    
    // Stress decays naturally
    if (pImpl->stressLevel > 0.0f) {
        pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - pImpl->stressDecayRate * static_cast<float>(dt));
    }
    
    // Predicton error affects focus
    if (predictionError > 0.0f) {
        pImpl->focusLevel = std::min(1.0f, pImpl->focusLevel + predictionError * 0.3f);
    } else {
        pImpl->focusLevel = std::max(0.1f, pImpl->focusLevel - 0.1f);
    }
    
    // Update NE level based on arousal and stress
    float arousalEffect = pImpl->arousalLevel * 0.5f;
    float stressEffect = pImpl->stressLevel * 0.3f;
    pImpl->level = std::min(1.0f, pImpl->baseline + arousalEffect + stressEffect);
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->arousalLevel = 0.5f;
    pImpl->focusLevel = 0.5f;
    pImpl->stressLevel = 0.0f;
}

void Norepinephrine::increaseArousal(float amount) {
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + amount);
    // This will affect NE level in update()
}

void Norepinephrine::decreaseArousal(float amount) {
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - amount);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

void Norepinephrine::setFocusLevel(float focus) {
    pImpl->focusLevel = std::clamp(focus, 0.0f, 1.0f);
}

float Norepinephrine::getFocusLevel() const {
    return pImpl->focusLevel;
}

void Norepinephrine::triggerStress(float stressLevel) {
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + stressLevel);
    // This will affect NE level in update()
}

bool Norepinephrine::isStressed() const {
    return pImpl->stressLevel > 0.1f;
}

// Serotonin Implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    MoodType mood;
    float impulsivityLevel;
    float socialBehavior;
    float decayRate;
    
    Impl() : level(0.0f), baseline(0.01f), mood(MoodType::Neutral), 
             impulsivityLevel(0.5f), socialBehavior(0.5f), decayRate(0.05f) {}
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
    // Serotonin modulates plasticity based on mood and impulsivity
    float impulsivityFactor = pImpl->impulsivityLevel * 0.4f;
    float socialFactor = pImpl->socialBehavior * 0.3f;
    
    // Low serotonin (depression-like) increases plasticity
    if (pImpl->level < 0.1f) {
        return 1.0f + impulsivityFactor + socialFactor;
    }
    
    return 0.5f + impulsivityFactor + socialFactor;
}

void Serotonin::update(TimestepDuration dt, float predictionError, float reward, float novelty) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Mood affects serotonin
    switch (pImpl->mood) {
        case MoodType::Happy:
        case MoodType::Content:
            pImpl->level = std::min(1.0f, pImpl->level + 0.1f);
            break;
        case MoodType::Sad:
        case MoodType::Depressed:
            pImpl->level = std::min(1.0f, pImpl->level - 0.05f);
            break;
        case MoodType::Anxious:
            pImpl->level = std::min(1.0f, pImpl->level + 0.15f);
            break;
        case MoodType::Calm:
            pImpl->level = std::min(1.0f, pImpl->level + 0.05f);
            break;
        default:
            break; // Neutral
    }
    
    // Reward affects social behavior
    if (reward > 0.5f) {
        pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + 0.1f);
    }
    
    // Novelty affects impulsivity
    if (novelty > 0.5f) {
        pImpl->impulsivityLevel = std::min(1.0f, pImpl->impulsivityLevel + 0.1f);
    }
    
    // Decay modifiers
    pImpl->socialBehavior *= (1.0f - pImpl->decayRate * 0.5f);
    pImpl->impulsivityLevel *= (1.0f - pImpl->decayRate);
}

void Serotonin::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->mood = MoodType::Neutral;
    pImpl->impulsivityLevel = 0.5f;
    pImpl->socialBehavior = 0.5f;
}

void Serotonin::setMood(MoodType mood) {
    pImpl->mood = mood;
    // This will affect serotonin level in update()
}

MoodType Serotonin::getMood() const {
    return pImpl->mood;
}

void Serotonin::modulateImpulsivity(float level) {
    pImpl->impulsivityLevel = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

void Serotonin::setSocialBehavior(float socialFactor) {
    pImpl->socialBehavior = std::clamp(socialFactor, 0.0f, 1.0f);
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

} // namespace nlm
