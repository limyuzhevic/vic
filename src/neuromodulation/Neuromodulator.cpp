#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityWeight;  // Enhanced plasticity factor
    float excitabilityWeight; // Enhanced neural excitability
    float memoryWeight;       // Memory consolidation factor
    float attentionWeight;    // Attention modulation factor
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , plasticityWeight(1.0f), excitabilityWeight(0.5f), memoryWeight(1.0f), attentionWeight(0.8f) {}
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
    // Enhanced dopamine-modulated plasticity factor
    // Higher dopamine not only increases baseline plasticity but specifically affects different processes
    return 0.5f + 
           0.3f * pImpl->level +           // Basic plasticity enhancement
           0.2f * pImpl->level * pImpl->level + // Non-linear effect for strong signals
           0.1f * (1.0f - pImpl->level) * pImpl->plasticityWeight; // Modulate with plastic weight
}

void Dopamine::update(TimestepDuration dt) {
    // More sophisticated dopamine dynamics
    // Decay with baseline maintenance and activity-dependent recovery
    pImpl->baseline = 0.05f;  // Maintain baseline level for tonic signaling
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling
    // Strong reward produces rapid, sustained increase in dopamine
    float rewardLevel = std::min(1.0f, reward * 2.0f);  // Amplify reward signal
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardLevel * pImpl->releaseRate * 2.0f);
    
    // Update plasticity weight based on reward
    pImpl->plasticityWeight = 1.0f + rewardLevel * 0.5f;  // Stronger rewards enhance plasticity
}

void Dopamine::signalRewardPredictionError(float error) {
    // More sophisticated prediction error signaling
    // Positive prediction error (better than expected) increases dopamine
    // Negative prediction error (worse than expected) decreases dopamine
    float predictionErrorEffect = std::clamp(error * 2.0f, -0.5f, 1.0f);  // Scale and clamp error
    
    pImpl->level = std::max(0.0f, pImpl->level + predictionErrorEffect * pImpl->releaseRate * 1.5f);
    
    // Update attention weight based on prediction error
    if (error > 0.5f) {
        pImpl->attentionWeight = std::min(2.0f, pImpl->attentionWeight + 0.2f);  // Pay more attention to surprising events
    }
}

void Dopamine::enhanceLearning(float novelty, float predictionError) {
    // Integrate dopamine with novelty and prediction error for sophisticated learning modulation
    // This creates a three-factor learning signal
    float learningSignal = pImpl->level * (1.0f + novelty) * (1.0f + predictionError);
    
    // Apply to plasticity weight with saturation
    pImpl->plasticityWeight = std::min(2.0f, pImpl->plasticityWeight * (1.0f + learningSignal * 0.1f));
}

float Dopamine::getExcitabilityModulation() const {
    // Dopamine's effect on neural excitability
    return pImpl->level * pImpl->excitabilityWeight;
}

float Dopamine::getMemoryModulation() const {
    // Dopamine's effect on memory consolidation
    // High dopamine enhances memory consolidation during sleep/rest periods
    return pImpl->level * pImpl->memoryWeight;
}

float Dopamine::getAttentionModulation() const {
    // Dopamine's effect on attention
    return pImpl->level * pImpl->attentionWeight;
}
