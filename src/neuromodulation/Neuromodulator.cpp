#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    // Real dopamine dynamics based on computational neuroscience
    float level;                // Current dopamine concentration (0-1 normalized)
    float baseline;             // Baseline dopamine level (tonic firing)
    float peak;                 // Maximum dopamine level (phasic response)
    float decayRate;            // Reuptake rate constant
    float releaseRate;          // Release rate on prediction error
    float eligibilityTrace;     // Synapses that have seen reward prediction error
    float predictionError;      // Latest reward prediction error signal
    float predictedValue;       // Current value prediction
    
    // Temporal dynamics
    float timeSinceLastEvent;   // Time since last spike event
    float refractoryPeriod;     // Minimum time between events
    
    Impl() : level(0.0f), baseline(0.15f), peak(1.0f), decayRate(0.2f), releaseRate(0.8f),
             eligibilityTrace(0.0f), predictionError(0.0f), predictedValue(0.0f),
             timeSinceLastEvent(0.0f), refractoryPeriod(0.1f) {}
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
    // Realistic dopamine modulation of plasticity:
    // - Low DA: Strong LTD, weak LTP (stability)
    // - Medium DA: Balanced learning
    // - High DA: Strong LTP, weak LTD (plasticity)
    // Formula from computational models of dopamine effects
    float normalizedError = std::clamp(pImpl->predictionError + 1.0f, 0.0f, 2.0f) / 2.0f;
    return 0.2f + 0.8f * normalizedError;
}

void Dopamine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Real dopamine dynamics:
    // 1. Natural decay due to reuptake
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * deltaTime);
    
    // 2. Eligibility trace decay
    if (pImpl->eligibilityTrace > 0.0f) {
        pImpl->eligibilityTrace *= std::exp(-pImpl->decayRate * deltaTime);
    }
    
    // 3. Refractory period tracking
    pImpl->timeSinceLastEvent += deltaTime;
    
    // 4. Slow baseline drift for adaptation
    pImpl->baseline = 0.15f + 0.05f * std::sin(pImpl->timeSinceLastEvent * 0.01f);
}

void Dopamine::signalReward(float reward) {
    // Realistic reward signaling with temporal dynamics:
    // 1. Track if we're in refractory period
    if (pImpl->timeSinceLastEvent < pImpl->refractoryPeriod) {
        // Suppress response during refractory period
        return;
    }
    
    // 2. Phasic response to reward
    float rewardSignal = std::min(reward, 1.0f); // Normalize reward
    pImpl->level = std::max(pImpl->level, 
                           std::min(pImpl->peak, pImpl->level + rewardSignal * pImpl->releaseRate * 2.0f));
    
    // 3. Update eligibility trace for reward-modulated learning
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + rewardSignal * 0.5f);
    
    // 4. Reset refractory timer
    pImpl->timeSinceLastEvent = 0.0f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Core dopamine function: reward prediction error (TD learning)
    pImpl->predictionError = error;
    
    // TD update: dopamine encodes reward prediction error
    // ΔV = α * (R - V), where V is predicted value, R is actual reward
    float tdUpdate = error * pImpl->releaseRate;
    
    // Update current level based on prediction error
    if (error > 0.0f) {
        // Positive prediction error (better than expected): increase level (learning)
        pImpl->level = std::min(pImpl->peak, pImpl->level + tdUpdate * 1.5f);
    } else {
        // Negative prediction error (worse than expected): decrease level towards baseline
        pImpl->level = std::max(pImpl->baseline, pImpl->level + tdUpdate * 0.8f);
    }
    
    // Update predicted value based on prediction error
    pImpl->predictedValue = std::clamp(pImpl->predictedValue + error * 0.1f, 0.0f, 1.0f);
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getPredictedValue() const {
    return pImpl->predictedValue;
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::updateEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = std::clamp(trace, 0.0f, 1.0f);
}

} // namespace nlm
