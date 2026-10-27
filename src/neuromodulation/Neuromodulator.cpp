#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityFactor;
    float rewardHistory;
    float predictionErrorHistory;
    
    // Dopamine dynamics
    float tonicLevel;
    float phasicResponse;
    float predictionErrorResponse;
    
    // Learning state
    float eligibilityTrace;
    float learningRate;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.05f), releaseRate(2.0f),
             plasticityFactor(1.0f), rewardHistory(0.0f), predictionErrorHistory(0.0f),
             tonicLevel(baseline), phasicResponse(0.0f), predictionErrorResponse(0.0f),
             eligibilityTrace(0.0f), learningRate(0.01f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    // Combine tonic and phasic components
    float totalLevel = pImpl->tonicLevel + pImpl->phasicResponse * 0.3f;
    return std::clamp(totalLevel, 0.0f, 1.0f);
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    pImpl->phasicResponse = level * 2.0f;  // Phasic is stronger
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity based on reward prediction error
    // Higher dopamine in appropriate contexts enhances learning
    float modulation = pImpl->plasticityFactor;
    
    // Error-based plasticity modulation
    if (pImpl->predictionErrorHistory > 0.0f) {
        modulation *= (1.0f + pImpl->predictionErrorHistory * 2.0f);
    }
    
    return std::clamp(modulation, 0.1f, 3.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics
    
    // Decay phasic responses
    pImpl->phasicResponse = std::max(0.0f, pImpl->phasicResponse - 0.1f * static_cast<float>(dt));
    pImpl->predictionErrorResponse = std::max(0.0f, pImpl->predictionErrorResponse - 0.1f * static_cast<float>(dt));
    
    // Tonic component maintains baseline
    pImpl->tonicLevel = pImpl->baseline + (pImpl->rewardHistory * 0.3f);
    
    // Update eligibility trace
    pImpl->eligibilityTrace *= 0.95f;  // Decay eligibility
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with burst response
    pImpl->rewardHistory = reward;
    
    // Phasic dopamine burst proportional to reward prediction error
    float predictionError = std::max(0.0f, reward - pImpl->predictionErrorHistory);
    pImpl->phasicResponse = std::min(pImpl->peak, pImpl->phasicResponse + predictionError * pImpl->releaseRate);
    
    // Update prediction error for next calculation
    pImpl->predictionErrorHistory = reward;
    
    // Learn action-reward association
    pImpl->eligibilityTrace = 1.0f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    pImpl->predictionErrorHistory = error;
    
    // Phasic response to prediction error
    pImpl->predictionErrorResponse = std::min(pImpl->peak, error * pImpl->releaseRate * 1.5f);
    
    // Error-driven learning
    if (std::abs(error) > 0.01f) {
        pImpl->eligibilityTrace = 1.0f;
    }
}

} // namespace nlm
