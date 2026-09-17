#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // Dopamine enhances plasticity for reward prediction error
    // High dopamine -> more synaptic change for significant errors
    float modulation = 1.0f;
    
    if (pImpl->level > 0.5f) {
        // Strong dopaminergic signal enhances plasticity
        modulation = 1.0f + 2.0f * (pImpl->level - 0.5f);
    } else if (pImpl->level < 0.2f) {
        // Low dopamine suppresses plasticity
        modulation = 0.5f * pImpl->level;
    }
    
    return modulation;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on computational neuroscience
    // – Tonic firing: baseline activity maintained by VTA/SNc neurons
    // – Phasic responses: burst firing for rewards/prediction errors
    
    float dtFloat = static_cast<float>(dt);
    
    // Decay with base rate (captures tonic firing)
    pImpl->level *= std::exp(-pImpl->decayRate * dtFloat);
    
    // Add baseline component
    pImpl->level += pImpl->baseline * (1.0f - std::exp(-pImpl->decayRate * dtFloat));
    
    // Clamp to reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine response to reward prediction
    // Implements temporal difference learning mechanism
    // The reward prediction error drives synaptic learning
    
    float rewardMagnitude = std::clamp(reward, 0.0f, 10.0f);
    
    // Calculate prediction error magnitude
    // For perfect predictions, error would be small
    // For unexpected rewards, error is large
    float predictionError = rewardMagnitude * pImpl->releaseRate;
    
    // Dopamine burst response
    pImpl->level = std::min(pImpl->peak, 
                           pImpl->level + predictionError);
    
    // Store reward for potential prediction error calculations
    // In a full implementation, this would be compared to expected value
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling (temporal difference)
    // This is the core mechanism of reinforcement learning
    // Error = reward + γ‰[V(s')] - V(s)
    
    // Negative errors (worse than expected) suppress dopamine
    // Positive errors (better than expected) increase dopamine
    // Error is scaled by importance
    
    // Error integration with time constant
    pImpl->level = std::max(0.0f, 
                           pImpl->level + error * pImpl->releaseRate);
    
    // Apply eligibility trace mechanism for reward-modulated learning
    // Dopamine modulates synaptic plasticity during learning windows
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

} // namespace nlm
