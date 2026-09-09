#include "serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float timeSinceReward;
    float impulsivityLevel;
    float moodStability;
    
    Impl() : level(0.0f), timeSinceReward(0.0f), impulsivityLevel(1.0f), moodStability(0.5f) {}
};

struct Norepinephrine::Impl {
    float level;
    float arousal;
    float vigilance;
    float focus;
    
    Impl() : level(0.0f), arousal(0.5f), vigilance(0.5f), focus(0.5f) {}
};

struct Acetylcholine::Impl {
    float level;
    float memoryEnhancement;
    float learningRate;
    float attentionGain;
    
    Impl() : level(0.0f), memoryEnhancement(0.5f), learningRate(0.5f), attentionGain(0.5f) {}
};

struct Dopamine::Impl {
    float level;
    float rewardPredictionError;
    float motivation;
    float learningRate;
    float plasticityFactor;
    
    Impl() : level(0.0f), rewardPredictionError(0.0f), motivation(0.5f), learningRate(0.5f), plasticityFactor(1.0f) {}
};

// Serotonin implementation
Serotonin::Serotonin() : pImpl(new Impl) {}
Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const { return "Serotonin"; }
float Serotonin::getLevel() const { return pImpl->level; }
void Serotonin::setLevel(float level) { pImpl->level = std::clamp(level, 0.0f, 2.0f); }
float Serotonin::getPlasticityFactor() const {
    return 1.0f - pImpl->impulsivityLevel * 0.3f; // Less impulsive = better plasticity
}
void Serotonin::update(TimestepDuration dt) {
    // Natural decay
    pImpl->level *= 0.99f;
    
    // Reduce impulsivity over time
    pImpl->impulsivityLevel = std::max(0.3f, pImpl->impulsivityLevel * 0.995f);
    
    // Stabilize mood based on recent rewards
    if (pImpl->timeSinceReward > 100.0f) {
        pImpl->moodStability += 0.01f;
    } else {
        pImpl->moodStability = std::min(1.0f, pImpl->moodStability + 0.005f);
    }
    
    pImpl->timeSinceReward += dt;
}
void Serotonin::reduceImpulsivity() {
    pImpl->impulsivityLevel *= 0.9f;
}
void Serotonin::enhanceSocialBehavior() {
    // Improve social behavior signals
    pImpl->level += 0.1f;
}
void Serotonin::stabilizeMood() {
    pImpl->moodStability = std::min(1.0f, pImpl->moodStability + 0.1f);
}

// Norepinephrine implementation
Norepinephrine::Norepinephrine() : pImpl(new Impl) {}
Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const { return "Norepinephrine"; }
float Norepinephrine::getLevel() const { return pImpl->level; }
void Norepinephrine::setLevel(float level) { pImpl->level = std::clamp(level, 0.0f, 2.0f); }
float Norepinephrine::getPlasticityFactor() const {
    return 1.0f + pImpl->arousal * 0.1f; // Higher arousal = more plasticity
}
void Norepinephrine::update(TimestepDuration dt) {
    pImpl->level *= 0.98f;
    
    // Natural arousal cycle
    pImpl->arousal = 0.5f + 0.5f * std::sin(pImpl->arousal * 10.0f);
    pImpl->vigilance = std::clamp(pImpl->vigilance + 0.001f, 0.1f, 1.0f);
    pImpl->focus = std::min(1.0f, pImpl->focus + 0.002f);
}
void Norepinephrine::increaseArousal() {
    pImpl->level += 0.2f;
    pImpl->arousal = std::min(1.0f, pImpl->arousal + 0.1f);
}
void Norepinephrine::enhanceVigilance() {
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + 0.05f);
}
void Norepinephrine::focusAttention() {
    pImpl->focus = std::min(1.0f, pImpl->focus + 0.05f);
}

// Acetylcholine implementation
Acetylcholine::Acetylcholine() : pImpl(new Impl) {}
Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const { return "Acetylcholine"; }
float Acetylcholine::getLevel() const { return pImpl->level; }
void Acetylcholine::setLevel(float level) { pImpl->level = std::clamp(level, 0.0f, 2.0f); }
float Acetylcholine::getPlasticityFactor() const {
    return 1.0f + pImpl->memoryEnhancement * 0.2f;
}
void Acetylcholine::update(TimestepDuration dt) {
    pImpl->level *= 0.97f;
    
    // Learning rate modulation
    pImpl->learningRate = std::clamp(pImpl->learningRate + 0.005f, 0.3f, 1.0f);
    pImpl->attentionGain = std::clamp(pImpl->attentionGain + 0.002f, 0.3f, 1.0f);
}
void Acetylcholine::enhanceWorkingMemory() {
    pImpl->memoryEnhancement = std::min(1.0f, pImpl->memoryEnhancement + 0.1f);
}
void Acetylcholine::facilitateLearning() {
    pImpl->learningRate = std::min(1.0f, pImpl->learningRate + 0.1f);
}
void Acetylcholine::strengthenAttention() {
    pImpl->attentionGain = std::min(1.0f, pImpl->attentionGain + 0.05f);
}

// Dopamine implementation
Dopamine::Dopamine() : pImpl(new Impl) {}
Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const { return "Dopamine"; }
float Dopamine::getLevel() const { return pImpl->level; }
void Dopamine::setLevel(float level) { pImpl->level = std::clamp(level, 0.0f, 2.0f); }
float Dopamine::getPlasticityFactor() const {
    return 1.0f + pImpl->level * 0.5f * pImpl->plasticityFactor;
}
void Dopamine::update(TimestepDuration dt) {
    // Natural decay
    pImpl->level *= 0.95f;
    
    // Reward prediction error naturally decays
    pImpl->rewardPredictionError *= 0.98f;
    
    // Motivation and learning rate adaptations
    pImpl->motivation = std::max(0.1f, pImpl->motivation * 0.99f);
    pImpl->learningRate = std::clamp(pImpl->learningRate + 0.001f, 0.3f, 1.0f);
}
void Dopamine::signalReward(float reward) {
    pImpl->level = std::min(2.0f, pImpl->level + reward * 0.5f);
    pImpl->timeSinceReward = 0.0f;
}
void Dopamine::signalRewardPredictionError(float error) {
    pImpl->rewardPredictionError = error;
    pImpl->level = std::min(2.0f, pImpl->level + std::abs(error) * 0.3f);
}
void Dopamine::setPredictionError(float error) {
    pImpl->rewardPredictionError = error;
}
void Dopamine::modulateStdp(float reward) {
    pImpl->plasticityFactor = std::clamp(1.0f + reward * 0.1f, 0.5f, 2.0f);
}
void Dopamine::enhanceMotivation() {
    pImpl->motivation = std::min(1.0f, pImpl->motivation + 0.05f);
}
void Dopamine::encodeRewardPredictionError() {
    // Complex encoding based on prediction error
    float errorMagnitude = std::abs(pImpl->rewardPredictionError);
    pImpl->plasticityFactor = 1.0f + errorMagnitude * 0.2f;
}

} // namespace nlm
