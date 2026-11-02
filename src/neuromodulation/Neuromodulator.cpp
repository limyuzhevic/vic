#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Enhanced neuromodulation parameters
    float predictionErrorHistory[10];  // Rolling window of prediction errors
    size_t historyIndex;
    float eligibilityTrace;  // For reward-modulated spike timing dependent plasticity
    float stressLevel;      // For stress-related modulation
    float attentionGain;    // For attention modulation
    
    Impl() 
        : level(0.0f) 
        , baseline(0.0f) 
        , peak(1.0f) 
        , decayRate(0.1f) 
        , releaseRate(1.0f)
        , historyIndex(0)
        , eligibilityTrace(0.0f)
        , stressLevel(0.0f)
        , attentionGain(1.0f) {
        // Initialize prediction error history
        for (int i = 0; i < 10; ++i) {
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
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Enhanced dopamine-modulated plasticity factor
    // Incorporates eligibility trace, stress level, and attention gain
    float baseFactor = 0.5f;
    float eligibilityFactor = std::min(0.5f, pImpl->eligibilityTrace);
    float stressFactor = 1.0f - std::tanh(pImpl->stressLevel) * 0.3f;
    float attentionFactor = 1.0f + 0.2f * pImpl->attentionGain;
    
    // Smooth, biologically plausible update
    return baseFactor * eligibilityFactor * stressFactor * attentionFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Enhanced dopamine dynamics with temporal filtering
    float timeFactor = static_cast<float>(dt) * 0.001f;  // Convert to biological time scale
    
    // Decay towards baseline with adaptive rate
    float adaptiveDecay = pImpl->decayRate * (1.0f + pImpl->stressLevel * 0.5f);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - adaptiveDecay * timeFactor);
    
    // Adaptive attention modulation based on prediction error history
    float avgPredictionError = 0.0f;
    for (int i = 0; i < 10; ++i) {
        avgPredictionError += pImpl->predictionErrorHistory[i];
    }
    avgPredictionError /= 10.0f;
    
    // Adjust attention gain based on prediction error
    pImpl->attentionGain = std::clamp(1.0f + avgPredictionError * 0.5f, 0.5f, 2.0f);
}

void Dopamine::signalReward(float reward) {
    // Enhanced reward signaling with eligibility trace modulation
    float rewardSignal = std::clamp(reward, -1.0f, 1.0f);
    
    // Update eligibility trace (reward-modulated spike timing dependent plasticity)
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + rewardSignal * 0.5f);
    
    // Burst of dopamine with saturation and refractory period
    float burst = rewardSignal * pImpl->releaseRate * pImpl->eligibilityTrace;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Add to prediction error history
    pImpl->predictionErrorHistory[pImpl->historyIndex] = rewardSignal;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Enhanced reward prediction error signaling with adaptive learning
    float errorSignal = std::clamp(error, -1.0f, 1.0f);
    
    // Update eligibility trace for prediction error
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + std::abs(errorSignal) * 0.1f);
    
    // Dopamine responds to prediction error with asymmetric coding
    // Phasic increases for positive prediction errors (better than expected)
    // Phasic decreases for negative prediction errors (worse than expected)
    if (errorSignal > 0.0f) {
        // Positive prediction error (better than expected)
        pImpl->level = std::min(pImpl->peak, pImpl->level + errorSignal * 0.8f);
    } else {
        // Negative prediction error (worse than expected)
        pImpl->level = std::max(0.0f, pImpl->level + errorSignal * 0.6f);
    }
    
    // Add to prediction error history
    pImpl->predictionErrorHistory[pImpl->historyIndex] = errorSignal;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = peak;
}

void Dopamine::setDecayRate(float decayRate) {
    pImpl->decayRate = decayRate;
}

void Dopamine::setReleaseRate(float releaseRate) {
    pImpl->releaseRate = releaseRate;
}

void Dopamine::setStressLevel(float stressLevel) {
    pImpl->stressLevel = stressLevel;
}

} // namespace nlm
