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
    
    // Reward prediction error history
    float predictionError;
    float rewardHistory[10]; // Last 10 reward values
    int historyIndex;
    
    // Dopamine dynamics
    float burstStrength;
    float tonicLevel;
    float adaptationIndex;
    
    Impl() : level(0.0f), baseline(0.1f), peak(2.0f), decayRate(0.1f), releaseRate(1.0f),
             predictionError(0.0f), burstStrength(0.0f), tonicLevel(0.0f), adaptationIndex(0.0f),
             historyIndex(0) {
        for (int i = 0; i < 10; ++i) rewardHistory[i] = 0.0f;
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
    // Dopamine modulates plasticity based on reward prediction error
    // Phasic dopamine (prediction error) strongly affects plasticity
    // Tonic dopamine provides baseline plasticity modulation
    
    float predictionErrorEffect = pImpl->predictionError * 1.5f; // RPE drives plasticity
    float tonicEffect = pImpl->tonicLevel * 0.5f; // Tonic level supports baseline plasticity
    float burstEffect = pImpl->burstStrength * 0.8f; // Phasic bursts enhance plasticity
    float adaptationEffect = pImpl->adaptationIndex * 0.2f; // Adaptation prevents over-learning
    
    return std::max(0.1f, std::min(3.0f, 
        0.3f + predictionErrorEffect + tonicEffect + burstEffect - adaptationEffect));
}

void Dopamine::update(TimestepDuration dt) {
    // Natural decay towards baseline with adaptation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Phasic components decay
    pImpl->predictionError = std::max(0.0f, pImpl->predictionError - pImpl->decayRate * static_cast<float>(dt) * 0.5f);
    pImpl->burstStrength = std::max(0.0f, pImpl->burstStrength - pImpl->decayRate * static_cast<float>(dt) * 0.7f);
    
    // Adaptation index slowly decays
    pImpl->adaptationIndex = std::max(0.0f, pImpl->adaptationIndex - pImpl->decayRate * static_cast<float>(dt) * 0.1f);
    
    // Slight tonic level maintenance
    pImpl->tonicLevel = 0.1f + std::sin(static_cast<float>(dt) * 0.01f) * 0.05f; // Small oscillation
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine burst on unexpected reward
    pImpl->predictionError = std::max(0.0f, reward - pImpl->predictionError); // Simplified RPE
    pImpl->burstStrength = std::min(pImpl->peak, pImpl->burstStrength + reward * pImpl->releaseRate * 1.5f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
    
    // Store in history
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
    
    NLM_LOG_INFO("Dopamine: Phasic burst on reward " + std::to_string(reward) + " (level: " + std::to_string(pImpl->level) + ")");
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phasic dopamine response to prediction error (positive = better than expected, negative = worse)
    pImpl->predictionError = error;
    pImpl->burstStrength = std::min(pImpl->peak, pImpl->burstStrength + std::abs(error) * pImpl->releaseRate * 2.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 1.5f);
    
    // Update adaptation based on prediction error
    if (std::abs(error) > 0.5f) {
        pImpl->adaptationIndex = std::min(pImpl->peak, pImpl->adaptationIndex + 0.1f);
    }
    
    NLM_LOG_INFO("Dopamine: Prediction error " + std::to_string(error) + " (level: " + std::to_string(pImpl->level) + ")");
}

} // namespace nlm