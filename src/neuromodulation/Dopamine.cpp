#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
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
    float reuptakeRate;
    float synthesisRate;
    float predictionErrorGain;
    float rewardTrace;
    float predictionErrorTrace;
    SimulationStep lastReleased;
    float rewardHistory[10];  // Sliding window for reward history
    float predictionErrorHistory[10];  // Sliding window for prediction error
    int historyIndex;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.2f),
             reuptakeRate(0.3f), synthesisRate(0.02f), predictionErrorGain(1.5f),
             rewardTrace(0.0f), predictionErrorTrace(0.0f), lastReleased(0),
             historyIndex(0) {
        for (int i = 0; i < 10; ++i) {
            rewardHistory[i] = 0.0f;
            predictionErrorHistory[i] = 0.0f;
        }
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {
    NLM_LOG_INFO("Dopamine neuromodulator enhanced initialized");
}

Dopamine::~Dopamine() {
    NLM_LOG_INFO("Dopamine neuromodulator enhanced destroyed");
}

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
    // Enhanced dopamine: complex dynamics based on reward history and prediction error
    float basePlasticity = 0.5f;
    
    // Compute moving averages of reward and prediction error
    float rewardAvg = 0.0f;
    float errorAvg = 0.0f;
    for (int i = 0; i < 10; ++i) {
        rewardAvg += pImpl->rewardHistory[i];
        errorAvg += pImpl->predictionErrorHistory[i];
    }
    rewardAvg /= 10.0f;
    errorAvg /= 10.0f;
    
    // Dopamine enhances plasticity when rewards are positive and prediction error is high
    float rewardFactor = 1.0f + 0.5f * rewardAvg;
    float errorFactor = 1.0f + 1.0f * std::abs(errorAvg);
    float levelFactor = 1.0f + 1.5f * pImpl->level;
    
    return basePlasticity * rewardFactor * errorFactor * levelFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Dynamic dopamine dynamics with traces
    float timeFactor = static_cast<float>(dt) / 100.0f;
    
    // Natural clearance and reuptake
    pImpl->level -= pImpl->decayRate * timeFactor;
    pImpl->level -= pImpl->reuptakeRate * timeFactor * pImpl->level;
    
    // Update traces
    pImpl->rewardTrace = std::max(0.0f, pImpl->rewardTrace - 0.05f * timeFactor);
    pImpl->predictionErrorTrace = std::max(0.0f, pImpl->predictionErrorTrace - 0.05f * timeFactor);
    
    // Continuous synthesis
    pImpl->level += pImpl->synthesisRate * timeFactor;
    
    // Ensure level stays in valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Store reward in history
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
    
    // Boost level based on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    
    // Stronger reward enhances prediction error learning
    pImpl->predictionErrorTrace = std::min(pImpl->predictionErrorTrace + reward * 0.5f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Store prediction error in history
    pImpl->predictionErrorHistory[pImpl->historyIndex] = error;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
    
    // Respond to prediction error (both positive and negative)
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->predictionErrorGain);
    
    // Update prediction error trace
    pImpl->predictionErrorTrace = std::min(pImpl->predictionErrorTrace + std::abs(error) * 0.3f, 1.0f);
}

float Dopamine::getRewardTrace() const {
    return pImpl->rewardTrace;
}

float Dopamine::getPredictionErrorTrace() const {
    return pImpl->predictionErrorTrace;
}

void Dopamine::enhanceLearning(float learningSignal) {
    // Enhanced dopamine specifically for reinforcement learning
    pImpl->level = std::min(pImpl->peak, pImpl->level + learningSignal * 1.0f);
    
    // Strong learning signals enhance future learning (dopamine burst)
    if (learningSignal > 0.5f) {
        setLevel(getLevel() + 0.3f);
    }
    
    // Update traces
    pImpl->rewardTrace = std::min(pImpl->rewardTrace + learningSignal * 0.4f, 1.0f);
    pImpl->predictionErrorTrace = std::min(pImpl->predictionErrorTrace + learningSignal * 0.2f, 1.0f);
}

} // namespace nlm