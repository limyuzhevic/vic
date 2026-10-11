#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <numeric>

namespace nlm {

struct Dopamine::Impl {
    // Dopamine dynamics
    float level;
    float baseline;
    float tonicLevel;
    float predictionError;
    float rewardPrediction;
    
    // Temporal dynamics
    float phasicComponent;
    float tonicComponent;
    float eligibilityTrace;
    float historyBuffer[8];  // For temporal integration
    
    // Neuronal representation
    float firingRate;
    float burstIntensity;
    float adaptationFactor;
    
    // Plasticity modulation parameters
    float rewardMagnitudeScale;
    float predictionErrorSensitivity;
    float plasticityModulationStrength;
    float neuromodulationThreshold;
    
    // Configuration parameters
    float baselineDA;
    float phasicGain;
    float burstDuration;
    float burstFrequency;
    float reuptakeRate;
    float releaseDynamics;
    float tdLearningRate;
    float tdDiscount;
    
    // Temporal tracking
    float timeSinceLastBurst;
    float timeSinceLastReward;
    SimulationStep stepsSincePredictionUpdate;
    
    Impl() : level(0.0f), baseline(0.0f), tonicLevel(0.1f), predictionError(0.0f), rewardPrediction(0.0f),
              phasicComponent(0.0f), tonicComponent(0.0f), eligibilityTrace(0.0f), 
              firingRate(0.0f), burstIntensity(0.0f), adaptationFactor(1.0f),
              rewardMagnitudeScale(1.0f), predictionErrorSensitivity(1.0f),
              plasticityModulationStrength(1.0f), neuromodulationThreshold(0.1f),
              baselineDA(0.05f), phasicGain(10.0f), burstDuration(50.0f), burstFrequency(20.0f),
              reuptakeRate(0.01f), releaseDynamics(1.0f), tdLearningRate(0.01f), tdDiscount(0.95f),
              timeSinceLastBurst(0.0f), timeSinceLastReward(0.0f), stepsSincePredictionUpdate(0) {
        // Initialize history buffer
        for (int i = 0; i < 8; ++i) {
            historyBuffer[i] = 0.0f;
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
    // Implement biologically realistic dopamine-modulated plasticity factor
    // Based on reward prediction errors and phasic/tonic decomposition
    
    float predictionErrorFactor = 1.0f + pImpl->predictionErrorSensitivity * pImpl->predictionError;
    float tonicFactor = pImpl->tonicLevel > 0.0f ? (1.0f + pImpl->plasticityModulationStrength * pImpl->tonicLevel) : 1.0f;
    
    // Nonlinear interaction between phasic and tonic components
    float phasicInteraction = std::tanh(pImpl->phasicComponent * 2.0f);
    float combinedFactor = predictionErrorFactor * tonicFactor * (1.0f + phasicInteraction * pImpl->plasticityModulationStrength);
    
    return std::max(0.0f, std::min(combinedFactor, 5.0f)); // Keep within reasonable bounds
}

void Dopamine::update(TimestepDuration dt) {
    // Update dopamine dynamics with biologically realistic model
    // Implements temporal dynamics, adaptation, and reuptake
    
    // Update time counters
    pImpl->timeSinceLastBurst += static_cast<float>(dt);
    pImpl->timeSinceLastReward += static_cast<float>(dt);
    pImpl->stepsSincePredictionUpdate++;
    
    // Calculate combined dopamine level from phasic and tonic components
    float phasicInfluence = pImpl->phasicComponent * pImpl->phasicGain * (1.0f - pImpl->reuptakeRate * static_cast<float>(dt));
    float tonicInfluence = pImpl->tonicLevel * pImpl->plasticityModulationStrength;
    
    // Smooth level update with temporal integration
    pImpl->level = pImpl->level * (1.0f - pImpl->reuptakeRate * static_cast<float>(dt)) + 
                   (phasicInfluence + tonicInfluence) * 0.1f * static_cast<float>(dt);
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Tonic component adaptation
    pImpl->tonicLevel = pImpl->tonicLevel * (1.0f - pImpl->reuptakeRate) * static_cast<float>(dt);
    
    // Phasic component decay
    pImpl->phasicComponent = pImpl->phasicComponent * (1.0f - pImpl->reuptakeRate) * static_cast<float>(dt);
    
    // Eligibility trace decay
    pImpl->eligibilityTrace *= (1.0f - pImpl->reuptakeRate * 0.5f) * static_cast<float>(dt);
    
    // Adaptation factor recovery
    pImpl->adaptationFactor = std::max(1.0f - 0.01f * static_cast<float>(dt), pImpl->adaptationFactor);
    
    // Update history buffer for temporal integration
    std::rotate(std::begin(pImpl->historyBuffer), std::begin(pImpl->historyBuffer) + 1, std::end(pImpl->historyBuffer));
    pImpl->historyBuffer[7] = pImpl->level;
    
    // Firing rate dynamics
    pImpl->firingRate = std::max(0.0f, pImpl->firingRate - pImpl->adaptationFactor * 0.001f * static_cast<float>(dt));
    
    // Apply baseline regulation
    pImpl->baseline = pImpl->baselineDA * pImpl->adaptationFactor;
}

void Dopamine::signalReward(float reward) {
    // Implement biologically realistic reward signaling with phasic bursts
    // Models dopamine neuron burst firing in response to unexpected rewards
    
    // Calculate scaled reward magnitude
    float scaledReward = reward * pImpl->rewardMagnitudeScale;
    
    // Update reward prediction
    pImpl->rewardPrediction = pImpl->rewardPrediction * pImpl->tdDiscount + scaledReward;
    pImpl->stepsSincePredictionUpdate = 0;
    
    // Phasic burst generation based on reward magnitude and surprise
    float burstAmplitude = std::min(1.0f, std::max(0.0f, scaledReward * pImpl->phasicGain));
    
    // Burst dynamics with frequency and duration control
    float burstFactor = burstAmplitude * (1.0f - std::exp(-pImpl->burstFrequency * static_cast<float>(dt)));
    
    // Update phasic component
    pImpl->phasicComponent = burstFactor * pImpl->releaseDynamics * (1.0f - pImpl->reuptakeRate * static_cast<float>(dt));
    
    // Firing rate modulation
    float burstIncrease = burstFactor * 2.0f * pImpl->adaptationFactor;
    pImpl->firingRate += burstIncrease;
    
    // Time since last reward for adaptation
    float rewardInterval = pImpl->timeSinceLastReward > 0.0f ? static_cast<float>(dt) / pImpl->timeSinceLastReward : 0.0f;
    if (rewardInterval > 0.0f) {
        pImpl->adaptationFactor = std::min(2.0f, pImpl->adaptationFactor + 0.1f * rewardInterval);
    }
    pImpl->timeSinceLastReward = 0.0f;
    
    // Update eligibility trace
    pImpl->eligibilityTrace = 1.0f; // Reset for reward event
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling with temporal difference learning
    // Models dopamine responses to unexpected rewards and prediction errors
    
    // Store prediction error
    pImpl->predictionError = error * pImpl->predictionErrorSensitivity;
    
    // Scale prediction error with reward magnitude and tonic level
    float scaledError = error * pImpl->rewardMagnitudeScale * (1.0f + pImpl->tonicLevel);
    
    // Temporal difference learning update
    if (pImpl->stepsSincePredictionUpdate > 0) {
        float tdUpdate = pImpl->tdLearningRate * (pImpl->rewardPrediction - pImpl->rewardPrediction * pImpl->tdDiscount);
        pImpl->phasicComponent += tdUpdate * static_cast<float>(pImpl->stepsSincePredictionUpdate);
    }
    
    // Update phasic component with prediction error
    pImpl->phasicComponent += scaledError * pImpl->phasicGain * (1.0f - pImpl->reuptakeRate * static_cast<float>(dt));
    
    // Firing rate modulation based on prediction error
    float errorResponse = std::max(0.0f, scaledError * pImpl->plasticityModulationStrength);
    pImpl->firingRate += errorResponse;
    
    // Update eligibility trace for learning
    pImpl->eligibilityTrace = std::max(pImpl->eligibilityTrace, std::abs(error));
    
    // Tonic level adaptation based on prediction error variance
    float errorVariance = pImpl->predictionError * pImpl->predictionError;
    pImpl->tonicLevel += errorVariance * 0.001f * static_cast<float>(dt);
    pImpl->tonicLevel = std::clamp(pImpl->tonicLevel, 0.0f, 1.0f);
    
    // Reset prediction error after processing
    if (std::abs(error) > 1e-6f) {
        pImpl->predictionError = 0.0f;
    }
}

} // namespace nlm
