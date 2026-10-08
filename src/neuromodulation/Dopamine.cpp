// Enhanced Dopamine implementation
// Comprehensive dopamine neuromodulator with multiple integration effects

#include "Dopamine.hpp"
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
    
    // Enhanced dopamine state tracking
    float phasicComponent;
    float tonicComponent;
    float explorationBias;
    float rewardSensitivity;
    float predictionErrorPrecision;
    
    Impl() : 
        level(0.0f), baseline(0.05f), peak(1.0f), 
        decayRate(0.1f), releaseRate(1.0f),
        phasicComponent(0.0f), tonicComponent(0.05f),
        explorationBias(0.5f), rewardSensitivity(1.0f),
        predictionErrorPrecision(0.8f) {}
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
    // Incorporates both tonic and phasic dopamine effects
    float basePlasticity = 0.5f;
    
    // Tonic dopamine modulates background plasticity
    float tonicEffect = pImpl->tonicComponent * 0.3f;
    
    // Phasic dopamine signals reward prediction error
    float phasicEffect = pImpl->phasicComponent * 0.5f;
    
    // Reward sensitivity modulates plasticity
    float rewardEffect = pImpl->rewardSensitivity * 0.4f;
    
    // Prediction error precision affects learning rate
    float errorPrecision = pImpl->predictionErrorPrecision * 0.3f;
    
    return basePlasticity + tonicEffect + phasicEffect + rewardEffect + errorPrecision;
}

void Dopamine::update(TimestepDuration dt) {
    // Enhanced dopamine dynamics with multiple components
    float dtFloat = static_cast<float>(dt);
    
    // Decay tonic dopamine towards baseline
    pImpl->tonicComponent = std::max(pImpl->baseline, 
        pImpl->tonicComponent - pImpl->decayRate * 0.5f * dtFloat);
    
    // Phasic component decays faster
    pImpl->phasicComponent = std::max(0.0f, 
        pImpl->phasicComponent - pImpl->decayRate * 2.0f * dtFloat);
    
    // Overall level is weighted sum of tonic and phasic
    pImpl->level = pImpl->tonicComponent + pImpl->phasicComponent * 0.3f;
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling with exploration bias
    float burstMagnitude = std::min(1.0f, reward * pImpl->rewardSensitivity);
    
    // Phasic dopamine burst
    pImpl->phasicComponent = std::min(pImpl->phasicComponent + burstMagnitude * pImpl->releaseRate, 1.0f);
    
    // Update exploration bias based on reward history
    if (reward > 0.7f) {
        pImpl->explorationBias = std::min(1.0f, pImpl->explorationBias + 0.1f);
    } else if (reward < 0.3f) {
        pImpl->explorationBias = std::max(0.0f, pImpl->explorationBias - 0.05f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling with precision weighting
    float errorMagnitude = std::abs(error) * pImpl->predictionErrorPrecision;
    
    if (error > 0) {
        // Positive prediction error (better than expected)
        pImpl->phasicComponent = std::min(pImpl->phasicComponent + errorMagnitude * pImpl->releaseRate, 1.0f);
        pImpl->predictionErrorPrecision = std::min(1.0f, pImpl->predictionErrorPrecision + 0.05f);
    } else {
        // Negative prediction error (worse than expected)
        pImpl->phasicComponent = std::max(0.0f, pImpl->phasicComponent - errorMagnitude * pImpl->releaseRate);
        pImpl->predictionErrorPrecision = std::max(0.1f, pImpl->predictionErrorPrecision - 0.02f);
    }
}

// Enhanced dopamine methods implementation
float Dopamine::getLearningRateModulation() const {
    // Dopamine modulates learning rate based on reward prediction error
    float phasicContribution = pImpl->phasicComponent * 0.6f;
    float tonicContribution = pImpl->tonicComponent * 0.2f;
    float precisionContribution = pImpl->predictionErrorPrecision * 0.2f;
    return phasicContribution + tonicContribution + precisionContribution;
}

float Dopamine::getMotivationModulation() const {
    // Motivation is driven by reward sensitivity and current dopamine level
    float rewardSensitivity = pImpl->rewardSensitivity * getLevel() * 0.7f;
    float baselineMotivation = pImpl->baseline * 0.3f;
    return rewardSensitivity + baselineMotivation;
}

float Dopamine::getActionSelectionModulation() const {
    // Action selection bias influenced by exploration vs exploitation
    return pImpl->explorationBias * 0.6f + getLevel() * 0.4f;
}

float Dopamine::getWorkingMemoryModulation() const {
    // Dopamine enhances working memory encoding and maintenance
    float encodingBoost = getLevel() * 0.5f;
    float maintenanceBoost = pImpl->tonicComponent * 0.3f;
    return 1.0f + encodingBoost + maintenanceBoost;
}

float Dopamine::getPredictionErrorPrecision() const {
    return pImpl->predictionErrorPrecision;
}

float Dopamine::getCreditAssignmentModulation() const {
    // Credit assignment is enhanced by dopamine for temporal difference learning
    return getLevel() * pImpl->rewardSensitivity * 0.8f;
}

float Dopamine::getHabitFormationModulation() const {
    // Habit formation is influenced by sustained dopamine levels
    return pImpl->tonicComponent * 0.6f + getLevel() * 0.4f;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
    pImpl->tonicComponent = pImpl->baseline;
}

void Dopamine::setPhasicStrength(float strength) {
    pImpl->phasicComponent = std::clamp(strength, 0.0f, 1.0f);
}

void Dopamine::setAdaptationRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.01f, 1.0f);
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getPhasicComponent() const {
    return pImpl->phasicComponent;
}

float Dopamine::getTonicComponent() const {
    return pImpl->tonicComponent;
}

} // namespace nlm