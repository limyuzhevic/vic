#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <memory>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Reward prediction error components
    float expectedReward;
    float predictionError;
    float learningRate;
    
    // Temporal dynamics
    float eligibilityTrace;
    float eligibilityDecay;
    
    // Phasic and tonic components
    float tonicLevel;
    float phasicLevel;
    float phasicTime;
    float phasicDecay;
    
    // Enhanced neuromodulation features
    float burstProbability;
    float burstSize;
    float rewardRange;
    float explorationBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             expectedReward(0.0f), predictionError(0.0f), learningRate(0.01f),
             eligibilityTrace(0.0f), eligibilityDecay(0.95f),
             tonicLevel(0.0f), phasicLevel(0.0f), phasicTime(0.0f), phasicDecay(0.8f),
             burstProbability(0.1f), burstSize(0.5f), rewardRange(1.0f),
             explorationBoost(0.0f) {}
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
    // Enhanced plasticity factor with more sophisticated dynamics
    float basePlasticity = 0.5f;
    
    // Combine phasic and tonic components
    float phasicContribution = pImpl->phasicLevel * 0.7f;
    float tonicContribution = pImpl->tonicLevel * 0.3f;
    
    // Prediction error modulation
    float errorModulation = 1.0f + std::abs(pImpl->predictionError) * 0.5f;
    
    // Activity-dependent modulation
    float activityModulation = 1.0f + pImpl->eligibilityTrace * 0.3f;
    
    // Exploration boost
    float explorationModulation = 1.0f + pImpl->explorationBoost;
    
    float plasticity = (basePlasticity + phasicContribution + tonicContribution) *
                       errorModulation * activityModulation * explorationModulation;
    
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Enhanced dopamine dynamics with more realistic temporal properties
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= pImpl->eligibilityDecay;
    
    // Decay phasic component with exponential decay
    if (pImpl->phasicTime > 0.0f) {
        pImpl->phasicLevel *= pImpl->phasicDecay;
        pImpl->phasicTime -= static_cast<float>(dt);
    }
    
    // Decay toward baseline with time constant
    float decay = 1.0f - std::pow(pImpl->phasicDecay, static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decay;
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling with prediction error computation
    
    // Compute reward prediction error
    pImpl->predictionError = reward - pImpl->expectedReward;
    
    // Update expected reward with learning
    pImpl->expectedReward = pImpl->expectedReward + 
                          pImpl->learningRate * pImpl->predictionError;
    
    // Normalize reward to range
    float normalizedReward = (reward + pImpl->rewardRange) / (2.0f * pImpl->rewardRange);
    
    // Update eligibility trace (credit assignment)
    pImpl->eligibilityTrace += 1.0f;  // Increment on reward
    
    // Generate phasic response (burst)
    if (pImpl->burstProbability > 0.0f) {
        float burstMagnitude = pImpl->burstSize * normalizedReward;
        
        // Add to both tonic and phasic components
        pImpl->tonicLevel = std::min(pImpl->tonicLevel + burstMagnitude * 0.1f, 1.0f);
        pImpl->phasicLevel = std::min(pImpl->phasicLevel + burstMagnitude, 1.0f);
        pImpl->phasicTime = 0.5f;  // Duration of phasic response
        
        // Update level with burst contribution
        pImpl->level += burstMagnitude * 0.8f;
    }
    
    // Exploration boost for unexpected rewards
    if (std::abs(pImpl->predictionError) > 0.5f) {
        pImpl->explorationBoost = std::min(0.3f, std::abs(pImpl->predictionError));
    } else {
        pImpl->explorationBoost *= 0.9f;  // Decay exploration boost
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct reward prediction error signaling
    pImpl->predictionError = error;
    
    // Update expected reward
    pImpl->expectedReward = pImpl->expectedReward + 
                          pImpl->learningRate * error;
    
    // Generate phasic response based on prediction error
    float errorMagnitude = std::min(1.0f, std::abs(error));
    float direction = error > 0.0f ? 1.0f : -1.0f;
    
    // Update eligibility trace
    pImpl->eligibilityTrace += errorMagnitude * direction;
    
    // Generate phasic response
    pImpl->phasicLevel = std::min(pImpl->phasicLevel + errorMagnitude * 0.5f, 1.0f);
    pImpl->phasicTime = 0.5f;
    
    // Update level
    pImpl->level += error * pImpl->releaseRate;
    
    // Exploration boost for prediction errors
    if (std::abs(error) > 0.3f) {
        pImpl->explorationBoost = std::min(0.2f, std::abs(error) * 0.5f);
    }
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->expectedReward = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->phasicLevel = 0.0f;
    pImpl->phasicTime = 0.0f;
    pImpl->explorationBoost = 0.0f;
}

float Dopamine::getExpectedReward() const {
    return pImpl->expectedReward;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

float Dopamine::getExplorationBoost() const {
    return pImpl->explorationBoost;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

void Dopamine::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, rate);
}

void Dopamine::setRewardRange(float range) {
    pImpl->rewardRange = std::max(0.1f, range);
}

void Dopamine::setExplorationEnabled(bool enabled) {
    pImpl->explorationBoost = enabled ? 0.1f : 0.0f;
}

Dopamine::Dopamine(const Dopamine&) = default;
Dopamine& Dopamine::operator=(const Dopamine&) = default;

} // namespace nlm
