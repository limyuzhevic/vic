#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float expectedValue;     // Expected reward for prediction error calculation
    float rewardPredictionError;  // Current prediction error signal
    float attentionSignal;   // Modulatory signal from acetylcholine
    
    Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             expectedValue(0.0f), rewardPredictionError(0.0f), attentionSignal(0.0f) {}
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
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on reinforcement learning principles
    // Implements temporal difference learning and reward prediction error
    // Models dopamine neuron responses to unexpected rewards
    
    const float dt_float = static_cast<float>(dt);
    
    // Primary decay mechanism: baseline dopamine level
    // DA neurons exhibit tonic firing modulated by arousal states
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-pImpl->decayRate * dt_float);
    
    // Burst modulation for reward prediction errors
    if (pImpl->rewardPredictionError > 0.0f) {
        // Phasic burst for positive prediction error (unexpected reward)
        const float burstMagnitude = pImpl->rewardPredictionError * 10.0f;
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
        
        // Rapid decay after burst
        pImpl->rewardPredictionError = 0.0f;
    } else if (pImpl->rewardPredictionError < 0.0f) {
        // Dip for negative prediction error (omitted reward)
        const float dipMagnitude = std::abs(pImpl->rewardPredictionError) * 5.0f;
        pImpl->level = std::max(0.0f, pImpl->level - dipMagnitude);
        
        // Recovery after dip
        pImpl->rewardPredictionError = 0.0f;
    }
    
    // Modulation by attention and salience signals
    // Acetylcholine modulates dopamine release
    pImpl->level *= (1.0f + pImpl->attentionSignal * 0.3f);
    
    // Ensure level stays within biological bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on reward prediction error theory
    // Implements temporal difference learning: δ = r + γV(s') - V(s)
    // Where δ is the prediction error signal that modulates plasticity
    
    // Calculate reward prediction error using temporal difference formula
    // Simplified: current reward minus expected value
    const float expectedValue = pImpl->expectedValue;
    float predictionError = reward - expectedValue;
    
    // Apply reward prediction error signaling
    if (predictionError > 0.0f) {
        // Positive prediction error (better than expected)
        pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * pImpl->releaseRate * 2.0f);
        
        // Update expected value for next time step
        pImpl->expectedValue = reward * 0.9f + pImpl->expectedValue * 0.1f;
        
        // Store prediction error for neural plasticity
        pImpl->rewardPredictionError = predictionError;
    } else if (predictionError < 0.0f) {
        // Negative prediction error (worse than expected)
        pImpl->level = std::max(0.0f, pImpl->level + predictionError * pImpl->releaseRate);
        
        // Update expected value
        pImpl->expectedValue = reward * 0.9f + pImpl->expectedValue * 0.1f;
        
        // Store prediction error for learning (negative for prediction error learning)
        pImpl->rewardPredictionError = predictionError;
    }
    
    // Clamp to biological range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
