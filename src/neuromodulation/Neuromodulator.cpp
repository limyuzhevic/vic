#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Reward prediction error components
    float predictionError;
    float predictionErrorBaseline;
    
    // Temporal dynamics
    float lastBurstTime;
    float burstDuration;
    float burstAmplitude;
    
    // Learning state
    float valuePrediction;
    float rewardPrediction;
    
    // Exploration-exploitation trade-off
    float explorationRate;
    float exploitationRate;
    
    Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             predictionError(0.0f), predictionErrorBaseline(0.0f),
             lastBurstTime(0.0f), burstDuration(0.5f), burstAmplitude(2.0f),
             valuePrediction(0.0f), rewardPrediction(0.0f),
             explorationRate(0.2f), exploitationRate(0.8f) {}
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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // Enhanced model with temporal dynamics and prediction error
    
    // Base plasticity from dopamine level
    float basePlasticity = 0.5f + 0.5f * pImpl->level;
    
    // Modulate by prediction error
    float predictionModulation = 1.0f + 0.3f * pImpl->predictionError;
    
    // Apply exploration-exploitation trade-off
    float explorationMod = pImpl->explorationRate + (1.0f - pImpl->explorationRate) * pImpl->level;
    
    // Calculate final plasticity factor
    float plasticityFactor = basePlasticity * predictionModulation * explorationMod;
    
    // Clip to reasonable bounds
    return std::clamp(plasticityFactor, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // More biologically realistic model with temporal components
    
    float timeStep = static_cast<float>(dt);
    
    // Decay towards baseline
    float decayAmount = pImpl->decayRate * timeStep;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Apply prediction error modulation
    float predictionModulation = std::clamp(pImpl->predictionError + pImpl->predictionErrorBaseline, 0.0f, 1.0f);
    pImpl->level += (predictionModulation - pImpl->baseline) * 0.2f * timeStep;
    
    // Handle burst responses (temporal dynamics)
    float currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() * 0.001f;
    
    if (currentTime - pImpl->lastBurstTime < pImpl->burstDuration) {
        // During burst period
        float burstProgress = 1.0f - (currentTime - pImpl->lastBurstTime) / pImpl->burstDuration;
        float burstLevel = pImpl->peak * burstAmplitude * burstProgress;
        pImpl->level = std::max(pImpl->level, burstLevel);
    }
    
    // Clip to valid range
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // More sophisticated reward processing with temporal dynamics
    
    // Immediate response to reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    
    // Update value prediction based on reward
    pImpl->valuePrediction += (reward - pImpl->valuePrediction) * 0.3f;
    
    // Trigger burst response
    pImpl->lastBurstTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() * 0.001f;
    
    // Log reward signaling
    NLM_LOG_DEBUG(std::string("Dopamine reward signal: ") + "reward=" + std::to_string(reward) +
                 " level=" + std::to_string(pImpl->level));
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // Implements temporal difference learning for reward prediction error
    
    // Update prediction error
    pImpl->predictionError = error;
    
    // Update reward prediction
    pImpl->rewardPrediction += error * 0.1f;
    
    // Apply prediction error to dopamine level
    float predictionErrorModulation = std::clamp(error, -1.0f, 1.0f) * 0.5f;
    
    if (error > 0.0f) {
        // Positive prediction error (better than expected) - increase dopamine
        pImpl->level = std::min(pImpl->peak, pImpl->level + predictionErrorModulation * pImpl->releaseRate);
    } else if (error < 0.0f) {
        // Negative prediction error (worse than expected) - decrease dopamine
        pImpl->level = std::max(pImpl->baseline, pImpl->level + predictionErrorModulation * pImpl->releaseRate);
    }
    
    // Adjust exploration-exploitation balance based on prediction error
    if (error > 0.1f) {
        pImpl->explorationRate = std::min(pImpl->explorationRate + 0.05f, 0.5f);
        pImpl->exploitationRate = std::max(pImpl->exploitationRate - 0.05f, 0.5f);
    } else if (error < -0.1f) {
        pImpl->exploitationRate = std::min(pImpl->exploitationRate + 0.05f, 0.5f);
        pImpl->explorationRate = std::max(pImpl->explorationRate - 0.05f, 0.5f);
    }
    
    // Log prediction error signaling
    NLM_LOG_DEBUG(std::string("Dopamine prediction error: ") + "error=" + std::to_string(error) +
                 " prediction_error=" + std::to_string(pImpl->predictionError) +
                 " level=" + std::to_string(pImpl->level));
}

} // namespace nlm
