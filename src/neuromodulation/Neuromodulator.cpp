#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError;
    float eligibilityTrace;
    float tonicLevel;
    float phasicLevel;
    
    // Statistics
    float totalEvents;
    float averageEffect;
    float peakEffect;
    float baselineEffect;
    float lastUpdateTime;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , predictionError(0.0f), eligibilityTrace(0.0f), tonicLevel(0.05f), phasicLevel(0.0f)
        , totalEvents(0.0f), averageEffect(0.0f), peakEffect(0.0f), baselineEffect(0.05f)
        , lastUpdateTime(0.0f) {}
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
    // Enhanced plasticity factor with interaction terms
    // Tonic dopamine sets baseline plasticity, phasic modulates
    float tonicContribution = 0.3f * pImpl->tonicLevel;
    float phasicContribution = 0.7f * pImpl->phasicLevel;
    float predictionErrorModulation = 0.5f * std::abs(pImpl->predictionError);
    
    float factor = tonicContribution + phasicContribution + predictionErrorModulation;
    factor = std::clamp(factor, 0.1f, 2.0f);  // More dynamic range
    
    // Update statistics
    pImpl->totalEvents++;
    float effect = factor - 0.3f;  // Normalize around 0.3 baseline
    pImpl->averageEffect = (pImpl->averageEffect * (pImpl->totalEvents - 1) + effect) / pImpl->totalEvents;
    pImpl->peakEffect = std::max(pImpl->peakEffect, effect);
    pImpl->baselineEffect = std::min(pImpl->baselineEffect, effect);
    
    return factor;
}

void Dopamine::update(float rewardPredictionError, float novelty, 
                     float attention, float dt) {
    // Store prediction error
    pImpl->predictionError = rewardPredictionError;
    
    // Update eligibility trace
    pImpl->eligibilityTrace = 0.95f * pImpl->eligibilityTrace + 0.05f * std::abs(rewardPredictionError);
    
    // Compute phasic response with temporal derivative
    float predictedValue = pImpl->baseline + pImpl->predictionError;
    float currentValue = pImpl->level;
    float temporalDerivative = (predictedValue - currentValue) / dt;
    
    // Enhanced phasic response with modulation by novelty and attention
    float noveltyModulation = 1.0f + 0.5f * novelty;
    float attentionModulation = 1.0f + 0.3f * attention;
    
    // Update phasic component with more realistic dynamics
    pImpl->phasicLevel = std::clamp(pImpl->phasicLevel + 
        temporalDerivative * dt * noveltyModulation * attentionModulation, 
        0.0f, 1.0f);
    
    // Decay phasic response
    pImpl->phasicLevel = std::max(0.0f, pImpl->phasicLevel - 0.05f * dt);
    
    // Blend tonic and phasic components
    pImpl->level = 0.3f * pImpl->tonicLevel + 0.7f * pImpl->phasicLevel;
    
    // Update time
    pImpl->lastUpdateTime += dt;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->phasicLevel = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->lastUpdateTime = 0.0f;
}

Dopamine::ModulationStats Dopamine::getStats() const {
    ModulationStats stats;
    stats.totalEvents = pImpl->totalEvents;
    stats.averageEffect = pImpl->averageEffect;
    stats.peakEffect = pImpl->peakEffect;
    stats.baselineEffect = pImpl->baselineEffect;
    return stats;
}

void Dopamine::signalReward(float reward, float temporalDifference) {
    // Enhanced reward signaling with temporal difference learning
    float rewardModulation = reward * temporalDifference;
    
    // Ensure positive contribution but allow small negative
    rewardModulation = std::max(-0.5f, std::min(2.0f, rewardModulation));
    
    // Apply to both tonic and phasic components
    pImpl->tonicLevel = std::clamp(pImpl->tonicLevel + rewardModulation * 0.3f, 0.05f, 1.0f);
    pImpl->phasicLevel = std::clamp(pImpl->phasicLevel + rewardModulation * 0.7f, 0.0f, 1.0f);
    
    // Update current level
    pImpl->level = 0.3f * pImpl->tonicLevel + 0.7f * pImpl->phasicLevel;
}

void Dopamine::signalRewardPredictionError(float error, float eligibility) {
    // Reward prediction error with eligibility trace modulation
    pImpl->predictionError = error;
    
    // Update eligibility trace
    pImpl->eligibilityTrace = eligibility;
    
    // Phasic response proportional to prediction error
    float phasicResponse = error * eligibility * 0.5f;
    
    // Modulate both tonic and phasic based on sign
    if (error > 0.0f) {
        // Positive prediction error (better than expected): increase tonic
        pImpl->tonicLevel = std::min(1.0f, pImpl->tonicLevel + 0.3f * phasicResponse);
        pImpl->phasicLevel = std::min(1.0f, pImpl->phasicLevel + phasicResponse);
    } else {
        // Negative prediction error (worse than expected): decrease tonic, but not below baseline
        pImpl->tonicLevel = std::max(0.05f, pImpl->tonicLevel + 0.3f * phasicResponse);
        pImpl->phasicLevel = std::max(0.0f, pImpl->phasicLevel + phasicResponse);
    }
    
    // Update current level
    pImpl->level = 0.3f * pImpl->tonicLevel + 0.7f * pImpl->phasicLevel;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

} // namespace nlm
