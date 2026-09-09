#include "Dopamine.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float timeConstant;
    float modulationStrength;
    float adaptationRate;
    
    // Reward prediction error signal
    float rewardPredictionError;
    float expectedReward;
    float rewardPredictionErrorGain;
    
    // Neural adaptation variables
    float neuralAdaptation;
    float dopamineSensitivity;
    
    // Internal state for realistic dynamics
    float integrationState;
    float historyValue;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.2f), releaseRate(1.5f),
             timeConstant(100.0f), modulationStrength(1.0f), adaptationRate(0.01f),
             rewardPredictionError(0.0f), expectedReward(0.0f), rewardPredictionErrorGain(1.0f),
             neuralAdaptation(0.0f), dopamineSensitivity(1.0f), integrationState(0.0f), historyValue(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    // Initialize with biological parameters
    setBaseline(0.1f);
    setPeak(1.0f);
    setTimeConstant(100.0f);
    setAdaptationRate(0.01f);
    setSensitivity(1.0f);
}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 0.5f);
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = std::clamp(peak, 0.5f, 2.0f);
}

float Dopamine::getTimeConstant() const {
    return pImpl->timeConstant;
}

void Dopamine::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 10.0f, 500.0f);
}

float Dopamine::getAdaptationRate() const {
    return pImpl->adaptationRate;
}

void Dopamine::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.001f, 0.1f);
}

float Dopamine::getSensitivity() const {
    return pImpl->dopamineSensitivity;
}

void Dopamine::setSensitivity(float sensitivity) {
    pImpl->dopamineSensitivity = std::clamp(sensitivity, 0.1f, 3.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor
    // Positive DA increases LTP, negative DA increases LTD
    // Use sigmoidal function for smooth transition
    float modulation = pImpl->modulationStrength * pImpl->level;
    float plasticityFactor = 0.5f + 0.5f * std::tanh(modulation * pImpl->dopamineSensitivity);
    
    // Add adaptation from neural activity
    float adaptation = pImpl->neuralAdaptation * pImpl->adaptationRate;
    return std::clamp(plasticityFactor + adaptation, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics with multiple processes
    float timeStep = static_cast<float>(dt);
    
    // Decay towards baseline with time constant
    float decayRate = pImpl->decayRate * timeStep / pImpl->timeConstant;
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-decayRate);
    
    // Integration of reward prediction error
    float integration = pImpl->rewardPredictionErrorGain * pImpl->rewardPredictionError;
    pImpl->integrationState += integration * timeStep;
    
    // Smooth integration with time constant
    float integrationRate = 1.0f - std::exp(-timeStep / pImpl->timeConstant);
    pImpl->level += (pImpl->integrationState - pImpl->level) * integrationRate * 0.1f;
    
    // Neural adaptation (activity-dependent plasticity)
    pImpl->neuralAdaptation = pImpl->neuralAdaptation * (1.0f - pImpl->adaptationRate * timeStep);
    if (pImpl->rewardPredictionError > 0.01f) {
        pImpl->neuralAdaptation += pImpl->rewardPredictionError * 0.05f;
    }
    
    // Clamp to biological range
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    
    // Store history for statistics
    pImpl->historyValue = pImpl->level;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with burst phasic response
    float burstAmplitude = std::clamp(reward * pImpl->releaseRate, 0.0f, pImpl->peak);
    
    // Add to current level with saturation
    pImpl->level += burstAmplitude;
    if (pImpl->level > pImpl->peak) {
        pImpl->level = pImpl->peak;
    }
    
    // Reset integration state for next burst
    pImpl->integrationState = 0.0f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (Vanga et al., 2021)
    pImpl->rewardPredictionError = error;
    
    // Stronger response to negative prediction errors (losses)
    if (error < 0.0f) {
        float magnitude = std::abs(error) * pImpl->rewardPredictionErrorGain * 2.0f;
        pImpl->level += magnitude;
    } else {
        float magnitude = error * pImpl->rewardPredictionErrorGain;
        pImpl->level += magnitude * 0.5f;  // Weaker response to positive errors
    }
    
    // Update expected reward with moving average
    pImpl->expectedReward = 0.9f * pImpl->expectedReward + 0.1f * error;
    
    // Integration for smooth dynamics
    pImpl->integrationState += error;
}

float Dopamine::getExpectedReward() const {
    return pImpl->expectedReward;
}

float Dopamine::getRewardPredictionError() const {
    return pImpl->rewardPredictionError;
}

float Dopamine::getNeuralAdaptation() const {
    return pImpl->neuralAdaptation;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->integrationState = 0.0f;
    pImpl->rewardPredictionError = 0.0f;
    pImpl->expectedReward = 0.0f;
    pImpl->neuralAdaptation = 0.0f;
    pImpl->historyValue = pImpl->baseline;
}

float Dopamine::getHistoryValue() const {
    return pImpl->historyValue;
}

} // namespace nlm
