#include "Neuromodulator.hpp"
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
    float predictionErrorHistory[10];  // Rolling window of prediction errors
    size_t historyIndex;
    float predictionErrorAverage;
    float temporalDifferenceError;  // TD error from temporal difference learning
    float eligibilityTrace;          // For credit assignment
    
    Impl() : level(0.0f), baseline(0.5f), peak(2.0f), decayRate(0.1f), releaseRate(1.0f),
             historyIndex(0), predictionErrorAverage(0.0f), temporalDifferenceError(0.0f), eligibilityTrace(0.0f) {
        for (int i = 0; i < 10; ++i) predictionErrorHistory[i] = 0.0f;
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Implement real dopamine-modulated plasticity factor
    // Phasic dopamine modulates learning rate, baseline dopamine sets valence
    float phasicComponent = pImpl->temporalDifferenceError;
    float tonicComponent = 1.0f + pImpl->level * 0.5f;  // Tonic increases with level
    
    // Combine with nonlinear interaction
    float plasticity = tonicComponent * (1.0f + std::abs(phasicComponent) * 0.5f);
    
    // Sign of TD error determines direction of plasticity
    if (phasicComponent < 0.0f) {
        plasticity *= 0.5f;  // Negative prediction error reduces plasticity
    }
    
    return std::max(0.1f, std::min(2.0f, plasticity));  // Bound to reasonable range
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics with temporal difference learning
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // Update prediction error average
    float totalError = 0.0f;
    for (int i = 0; i < 10; ++i) {
        totalError += pImpl->predictionErrorHistory[i];
    }
    pImpl->predictionErrorAverage = totalError / 10.0f;
    
    // Temporal difference learning integration
    if (pImpl->brain && pImpl->brain->getPredictionErrorSignal()) {
        // Get current prediction error from prediction error signal
        float currentPredictionError = pImpl->brain->getPredictionErrorSignal()->getError();
        
        // Compute temporal difference error
        pImpl->temporalDifferenceError = currentPredictionError - pImpl->predictionErrorAverage;
        
        // Apply eligibility trace for credit assignment
        pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - decayAmount * 0.5f);
    } else {
        pImpl->temporalDifferenceError = 0.0f;
    }
}

void Dopamine::signalReward(float reward) {
    // Implement real reward signaling with phasic bursts
    // Compute prediction error relative to average
    float predictionError = reward - pImpl->predictionErrorAverage;
    
    // Phasic burst proportional to prediction error
    float burstSize = std::min(std::abs(predictionError) * 2.0f, pImpl->peak);
    
    // Direction depends on prediction error sign
    float burstDirection = std::copysign(1.0f, predictionError);
    
    // Update level with phasic response
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstSize * burstDirection * pImpl->releaseRate);
    
    // Update eligibility trace for reward
    pImpl->eligibilityTrace += 1.0f;  // Mark synapses as eligible for reinforcement
    
    NLM_LOG_INFO(std::string("Dopamine: Reward signaling, prediction error=") + 
                std::to_string(predictionError) + ", burst size=" + std::to_string(burstSize));
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling with temporal difference learning
    // Update prediction error history
    pImpl->predictionErrorHistory[pImpl->historyIndex] = error;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
    
    // Compute temporal difference error for credit assignment
    float previousError = pImpl->predictionErrorHistory[(pImpl->historyIndex + 9) % 10];
    pImpl->temporalDifferenceError = error - previousError;
    
    // Apply neuromodulatory effect with eligibility trace
    float neuromodulatoryEffect = pImpl->temporalDifferenceError * pImpl->eligibilityTrace * pImpl->releaseRate;
    
    // Update dopamine level with reward prediction error
    pImpl->level = std::max(0.0f, pImpl->level + neuromodulatoryEffect);
    
    // Reduce eligibility trace after use
    pImpl->eligibilityTrace *= 0.5f;
    
    NLM_LOG_INFO(std::string("Dopamine: RPE signaling, TD error=") + std::to_string(pImpl->temporalDifferenceError) +
                ", effect=" + std::to_string(neuromodulatoryEffect));
}

} // namespace nlm
