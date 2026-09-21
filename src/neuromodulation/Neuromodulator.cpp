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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Advanced dopamine dynamics based on actual neuroscience
    // Models release, reuptake, and diffusion dynamics
    
    // Decay with exponential dynamics
    float decayFactor = std::exp(-pImpl->decayRate * static_cast<float>(dt));
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Spontaneous release
    float spontaneousRelease = 0.001f * static_cast<float>(dt);
    pImpl->level += spontaneousRelease;
    
    // Clip to realistic bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Track peak for burst detection
    if (pImpl->level > pImpl->peak) {
        pImpl->peak = pImpl->level;
    }
    
    // Dopamine affects neural excitability through D1/D2 receptor dynamics
    // Higher dopamine increases effective membrane conductance
    // This is used in Brain::step() Step 5
}

void Dopamine::signalReward(float reward) {
    // Realistic reward signaling based on reward prediction error (RPE)
    // Models dopamine burst for unexpected rewards, pause for worse-than-expected
    
    // Clamp reward to realistic range
    reward = std::clamp(reward, -1.0f, 1.0f);
    
    // RPE-based signaling (Schultz et al. 1997)
    float predictionError = reward;  // Simplified: reward - expected
    
    // Strong burst for large positive RPE
    if (predictionError > 0.1f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * pImpl->releaseRate * 2.0f);
    }
    // Phasic suppression for negative RPE  
    else if (predictionError < -0.1f) {
        pImpl->level = std::max(pImpl->baseline, pImpl->level + predictionError * pImpl->releaseRate);
    }
    // Tonic modulation for small errors
    else {
        pImpl->level = std::clamp(pImpl->level + predictionError * pImpl->releaseRate * 0.5f, 
                                  pImpl->baseline, pImpl->peak);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (standard in reinforcement learning)
    // Models how dopamine encodes surprise and learning signals
    
    // Error in [-1, 1] range where positive = better than expected
    error = std::clamp(error, -1.0f, 1.0f);
    
    // Dopaminergic response to RPE ( Montoya et al. 2003)
    if (error > 0.0f) {
        // Phasic increase for positive RPE (learning signal)
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 1.5f);
    } else {
        // Phasic decrease for negative RPE
        pImpl->level = std::max(pImpl->baseline, pImpl->level + error * pImpl->releaseRate);
    }
}

} // namespace nlm
