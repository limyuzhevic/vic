#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float predictionError;
    float rewardHistory;
    float expectedReward;
    float predictionErrorBaseline;
    float traceDecay;
    float burstThreshold;
    float decayRate;
    float releaseRate;
    float peakLevel;
    float eligibility;
    Timestamp lastBurstTime;
    
    Impl() : level(0.0f), baseline(0.005f), predictionError(0.0f), 
             rewardHistory(0.0f), expectedReward(0.0f), predictionErrorBaseline(0.0f),
             traceDecay(0.95f), burstThreshold(0.5f), decayRate(0.1f), 
             releaseRate(1.0f), peakLevel(1.0f), eligibility(0.0f),
             lastBurstTime(-10.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    // Initialize prediction error baseline
    pImpl->predictionErrorBaseline = 0.0f;
}

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
    // Positive prediction error increases plasticity, negative decreases it
    float modFactor = 0.5f + 0.5f * pImpl->predictionError;
    return std::clamp(modFactor, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Update dopamine level with decay and eligibility trace
    TimestepDuration currentTime = static_cast<Timestamp>(pImpl->currentStep) * dt;
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay eligibility trace
    pImpl->eligibility *= pImpl->traceDecay;
    
    // Decay prediction error baseline
    pImpl->predictionErrorBaseline *= pImpl->traceDecay;
    pImpl->predictionErrorBaseline = std::max(0.0f, pImpl->predictionErrorBaseline);
}

void Dopamine::signalReward(float reward) {
    // Calculate reward prediction error
    float error = reward - pImpl->expectedReward;
    
    // Update expected reward (exponential moving average)
    pImpl->expectedReward = 0.9f * pImpl->expectedReward + 0.1f * reward;
    
    // Update prediction error baseline
    pImpl->predictionErrorBaseline = 0.9f * pImpl->predictionErrorBaseline + 0.1f * std::abs(error);
    
    // Compute prediction error (normalized by baseline)
    float normalizedError = error / (pImpl->predictionErrorBaseline + 0.01f);
    
    // Update prediction error
    pImpl->predictionError = normalizedError;
    
    // Update eligibility trace for reward-modulated plasticity
    pImpl->eligibility += reward * 0.1f;
    
    // Burst response to unexpected reward
    Timestamp currentTime = static_cast<Timestamp>(pImpl->currentStep) * dt;
    if (std::abs(error) > pImpl->burstThreshold && 
        (currentTime - pImpl->lastBurstTime) > 1.0f) {
        
        // Strong burst of dopamine for unexpected reward
        float burst = std::min(pImpl->peakLevel, 0.5f + std::abs(error) * 2.0f);
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + burst * pImpl->releaseRate);
        pImpl->lastBurstTime = currentTime;
        
        NLM_LOG_DEBUG(std::string("Dopamine burst: ") + std::to_string(burst) + 
                     std::string(" (reward prediction error: ") + std::to_string(error) + std::string(")"));
    } else {
        // Gradual dopamine update for expected reward
        float gradual = std::min(0.2f, std::abs(error) * 0.5f);
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + gradual * pImpl->releaseRate);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct signal of reward prediction error (for when reward is known but unexpected)
    pImpl->predictionError = error / (pImpl->predictionErrorBaseline + 0.01f);
    
    // Update eligibility trace based on prediction error
    pImpl->eligibility += error * 0.1f;
    
    // Modulate dopamine level based on prediction error
    float errorMagnitude = std::abs(error);
    
    if (error > 0.0f) {
        // Positive error (better than expected): increase dopamine
        float increase = std::min(0.5f, errorMagnitude * 0.3f);
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + increase * pImpl->releaseRate);
    } else {
        // Negative error (worse than expected): decrease dopamine
        float decrease = std::min(0.3f, errorMagnitude * 0.2f);
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decrease * pImpl->releaseRate);
    }
    
    // Record reward history for learning
    pImpl->rewardHistory = error;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getPredictionErrorBaseline() const {
    return pImpl->predictionErrorBaseline;
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibility;
}

void Dopamine::updatePrediction(float predictedReward) {
    // Update predicted reward expectation
    pImpl->expectedReward = predictedReward;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->predictionError = 0.0f;
    pImpl->expectedReward = 0.0f;
    pImpl->predictionErrorBaseline = 0.0f;
    pImpl->eligibility = 0.0f;
    pImpl->rewardHistory = 0.0f;
}

} // namespace nlm