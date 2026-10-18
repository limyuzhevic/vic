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
    float predictionErrorHistory;
    float rewardHistory;
    float learningRate;
    TimestepDuration lastUpdateTime;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , predictionErrorHistory(0.0f)
        , rewardHistory(0.0f)
        , learningRate(0.01f)
        , lastUpdateTime(0.0) {}
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
    // Real dopamine-modulated plasticity factor
    // Phasic dopamine enhances synaptic plasticity during reward prediction error
    // Tonic dopamine provides baseline plasticity
    float predictionErrorFactor = 1.0f + std::abs(pImpl->predictionErrorHistory) * 2.0f;
    float rewardFactor = 1.0f + pImpl->rewardHistory * 0.5f;
    float dopamineModulation = 0.5f + 0.5f * pImpl->level;
    
    return std::max(0.1f, std::min(3.0f, predictionErrorFactor * rewardFactor * dopamineModulation));
}

void Dopamine::update(TimestepDuration dt) {
    if (dt <= 0.0) return;
    
    // Real dopamine dynamics: decay towards baseline with time-dependent components
    float timeDecay = pImpl->decayRate * static_cast<float>(dt);
    float decayFactor = std::exp(-timeDecay);
    
    // Phasic dopamine bursts fade with exponential decay
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Update prediction error and reward history
    pImpl->predictionErrorHistory *= 0.99f; // Decay prediction error
    pImpl->rewardHistory *= 0.99f; // Decay reward
    
    pImpl->lastUpdateTime = dt;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: burst of dopamine proportional to reward magnitude
    // Initial phasic response scaled by reward value
    float rewardMagnitude = std::clamp(reward, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardMagnitude * pImpl->releaseRate);
    pImpl->rewardHistory = std::max(pImpl->rewardHistory, rewardMagnitude);
    
    NLM_LOG_INFO(std::string("Dopamine: Reward burst of ") + std::to_string(reward));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling: dopamine responds to prediction errors
    // Positive error (better than expected): phasic increase
    // Negative error (worse than expected): phasic decrease
    float errorScaled = std::clamp(error, -1.0f, 1.0f);
    float errorResponse = errorScaled * pImpl->releaseRate * 0.5f;
    
    pImpl->level += errorResponse;
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    pImpl->predictionErrorHistory = std::max(pImpl->predictionErrorHistory, std::abs(error));
    
    NLM_LOG_INFO(std::string("Dopamine: RPE signal ") + std::to_string(error) + std::string(" level: ") + std::to_string(pImpl->level));
}

void Dopamine::learnFromOutcome(float reward, float predictedReward, TimestepDuration dt) {
    // Real reward learning: dopamine encodes reward prediction error for learning
    float predictionError = reward - predictedReward;
    
    // TD learning-like update: dopamine teaches temporal difference learning
    float tdError = predictionError * pImpl->learningRate;
    
    // Update level based on prediction error
    signalRewardPredictionError(tdError);
    
    // Integrate with reward signal for combined learning
    signalReward(reward);
    
    NLM_LOG_INFO(std::string("Dopamine: TD learning from reward ") + std::to_string(reward) +
                 std::string(" predicted ") + std::to_string(predictedReward) +
                 std::string(" error ") + std::to_string(tdError));
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
    pImpl->predictionErrorHistory = 0.0f;
    pImpl->rewardHistory = 0.0f;
    NLM_LOG_INFO("Dopamine system reset");
}

} // namespace nlm
