#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // For reward prediction error tracking
    float expectedReward;
    float recentPredictionError;
    
    // Eligibility trace for reward-modulated plasticity
    float eligibilityTrace;
    float eligibilityDecay;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             expectedReward(0.5f), recentPredictionError(0.0f),
             eligibilityTrace(0.0f), eligibilityDecay(0.95f) {}
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
    // Higher dopamine increases synaptic plasticity with optimal range
    float normalizedLevel = pImpl->level / pImpl->peak;
    
    // Gaussian function: plasticity peaks at moderate dopamine levels
    // Too low or too high dopamine reduces plasticity
    float optimalLevel = 0.6f;
    float sensitivity = 2.0f;
    float plasticityFactor = std::exp(-std::pow((normalizedLevel - optimalLevel) / sensitivity, 2.0f));
    
    // Add contribution from prediction error
    float errorContribution = 1.0f + std::abs(pImpl->recentPredictionError) * 0.5f;
    
    return plasticityFactor * errorContribution;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with temporal filtering and decay
    // Implements biologically realistic dopamine release and clearance
    
    float timeFactor = static_cast<float>(dt);
    
    // Multiple timescales of dopamine dynamics
    // Fast decay for immediate response, slower components for sustained signaling
    
    // Fast component: rapid decay toward baseline (synaptic clearance)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * timeFactor);
    
    // Slow component: gradual return to baseline (tonic dopamine)
    if (pImpl->level > pImpl->baseline * 1.5f) {
        float slowDecay = pImpl->decayRate * 0.5f * timeFactor;
        pImpl->level -= slowDecay;
    }
    
    // Update baseline to track recent activity levels
    pImpl->baseline = pImpl->level * 0.1f + pImpl->baseline * 0.9f;
    
    // Decay eligibility trace for reward-modulated learning
    pImpl->eligibilityTrace *= pImpl->eligibilityDecay;
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with burst dynamics and eligibility traces
    // Implements reward prediction error with temporal difference learning
    
    // Compute reward prediction error based on expected vs actual reward
    float predictionError = reward - pImpl->expectedReward;
    
    // Apply reward prediction error with reward prediction error signaling
    signalRewardPredictionError(predictionError);
    
    // Add burst of dopamine proportional to actual reward magnitude
    float burstMagnitude = std::min(reward, 1.0f) * pImpl->releaseRate * 2.0f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
    
    // Update expected reward for future predictions
    pImpl->expectedReward = reward * 0.7f + pImpl->expectedReward * 0.3f;
    
    // Store recent reward for eligibility trace computation
    pImpl->peak = std::max(pImpl->peak, reward);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling with eligibility trace integration
    // Implements temporal difference learning for reward-modulated plasticity
    
    // Store prediction error for eligibility trace computation
    pImpl->recentPredictionError = error;
    
    // Compute dopamine response based on prediction error
    float dopamineResponse = error * pImpl->releaseRate;
    
    // Integrate with existing level (negative errors reduce, positive increase)
    float newLevel = pImpl->level + dopamineResponse;
    
    // Clamp to biologically plausible range
    newLevel = std::clamp(newLevel, 0.0f, pImpl->peak);
    
    // Update level
    setLevel(newLevel);
    
    // Update eligibility trace for reward-modulated plasticity
    // Positive prediction errors strengthen synapses, negative weaken them
    pImpl->eligibilityTrace += error * pImpl->releaseRate * 0.5f;
    
    // Clamp eligibility trace to reasonable range
    pImpl->eligibilityTrace = std::clamp(pImpl->eligibilityTrace, -1.0f, 1.0f);
}

} // namespace nlm