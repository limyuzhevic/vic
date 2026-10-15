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

void Dopamine::signalCuriosity(float curiosity) {
    // Signal curiosity-driven exploration
    float curiositySignal = curiosity * 0.5f * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + curiositySignal);
    
    // Curiosity increases attention and plasticity
    pImpl->attentionModulation += curiosity * pImpl->learningRate * 0.3f;
}

float Dopamine::getAttentionModulation() const {
    return pImpl->attentionModulation;
}

void Acetylcholine::signalAttention(float attention) {
    // Signal attention allocation
    float burst = attention * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Create attention modulation effect
    pImpl->attentionStrength += attention * 0.5f;
}

void Acetylcholine::signalNovelty(float novelty) {
    // Signal novelty detection (increases attention)
    float noveltySignal = novelty * 0.8f * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + noveltySignal);
    
    // Novelty increases attention strength
    pImpl->attentionStrength += novelty * 0.3f;
}

void Acetylcholine::signalSensoryInput(float inputStrength) {
    // Signal sensory processing (increases ACh release)
    float sensorySignal = inputStrength * pImpl->releaseRate * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + sensorySignal);
    
    // Enhance working memory
    pImpl->workingMemoryBoost += inputStrength * 0.2f;
}

float Acetylcholine::getAttentionStrength() const {
    return pImpl->attentionStrength;
}

float Acetylcholine::getMemoryConsolidation() const {
    return pImpl->memoryConsolidation;
}

float Acetylcholine::getWorkingMemoryBoost() const {
    return pImpl->workingMemoryBoost;
}

void Norepinephrine::signalArousal(float arousal) {
    // Signal arousal and alertness
    float burst = arousal * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Create arousal modulation effect
    pImpl->arousalLevel += arousal * 0.5f;
}

void Norepinephrine::signalStress(float stress) {
    // Signal stress response
    float stressSignal = stress * pImpl->releaseRate * 0.7f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressSignal);
    
    // High stress increases stress level
    pImpl->stressLevel += stress * 0.3f;
}

void Norepinephrine::signalWarning(float warning) {
    // Signal threat or warning
    float warningSignal = warning * pImpl->releaseRate * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + warningSignal);
    
    // Warning increases arousal and processing speed
    pImpl->arousalLevel += warning * 0.4f;
    pImpl->processingSpeed += warning * 0.2f;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

float Norepinephrine::getProcessingSpeed() const {
    return pImpl->processingSpeed;
}

void Serotonin::signalMood(float mood) {
    // Signal mood state (positive or negative)
    float moodSignal = mood * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + moodSignal);
    
    // Create mood effect
    pImpl->moodLevel += mood * 0.4f;
}

void Serotonin::signalReward(float reward) {
    // Signal reward (increases serotonin for positive outcomes)
    float rewardSignal = reward * pImpl->releaseRate * 0.6f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardSignal);
    
    // Enhance reward sensitivity
    pImpl->rewardSensitivity += reward * 0.3f;
    
    // Improve social cohesion after reward
    pImpl->socialCohesion += reward * 0.2f;
}

void Serotonin::signalPunishment(float punishment) {
    // Signal punishment or negative outcome
    float punishmentSignal = punishment * pImpl->releaseRate * 0.8f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + punishmentSignal);
    
    // Punishment reduces impulsivity
    pImpl->impulsivityLevel += punishment * 0.5f;
}

void Serotonin::signalSocialBonding(float bonding) {
    // Signal social bonding and connection
    float bondingSignal = bonding * pImpl->releaseRate * 0.4f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + bondingSignal);
    
    // Strong social bonding increases social cohesion
    pImpl->socialCohesion += bonding * 0.6f;
}

float Serotonin::getMoodLevel() const {
    return pImpl->moodLevel;
}

float Serotonin::getImpulsivityLevel() const {
    return pImpl->impulsivityLevel;
}

float Serotonin::getSocialCohesion() const {
    return pImpl->socialCohesion;
}

float Serotonin::getRewardSensitivity() const {
    return pImpl->rewardSensitivity;
}

} // namespace nlm

