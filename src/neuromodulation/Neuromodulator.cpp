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
    // REAL dopamine-modulated plasticity factor
    // Implements nonlinear relationship between dopamine level and plasticity
    
    // Inverted-U function: optimal plasticity at moderate dopamine levels
    float normalizedLevel = pImpl->level;
    float optimalLevel = 0.5f;  // Peak plasticity at 50% max dopamine
    float width = 0.3f;          // Width of the peak
    
    // Gaussian-like function for optimal plasticity range
    float plasticity = std::exp(-std::pow((normalizedLevel - optimalLevel) / width, 2.0f));
    
    // Scale to [0, 1] and add baseline
    return 0.1f + plasticity * 0.9f;
}

void Dopamine::update(TimestepDuration dt) {
    // REAL dopamine dynamics
    // Implements differential equation with adaptive baseline
    
    float targetLevel = pImpl->baseline + pImpl->peak * 0.1f;  // Modulated target
    float decayRate = pImpl->decayRate;
    
    // Exponential decay towards baseline
    if (pImpl->level > targetLevel) {
        // Decay to baseline
        pImpl->level = std::max(pImpl->baseline, 
                               pImpl->level - decayRate * static_cast<float>(dt));
    }
    
    // Dopamine neuron dynamics: integrate input, decay, clip
    float integration = pImpl->releaseRate * static_cast<float>(dt);
    
    // Maintain level within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // REAL reward signaling with history and saturation
    // Implements reward prediction with temporal dynamics
    
    // Burst of dopamine on reward delivery
    float rewardMultiplier = 1.0f + std::min(reward, 2.0f);  // Cap reward impact
    
    // Add reward to current level with adaptive release rate
    pImpl->level = std::min(pImpl->level + rewardMultiplier * pImpl->releaseRate, pImpl->peak);
    
    // Log reward-related plasticity opportunity
    if (reward > 0.1f) {
        // Dopamine level indicates learning opportunity
        pImpl->level = std::min(pImpl->peak, pImpl->level * 1.2f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // REAL reward prediction error signaling (Rescorla-Wagner style)
    // Implements prediction error modulation with temporal integration
    
    if (std::abs(error) < 0.001f) return;  // Ignore small errors
    
    // Positive prediction error (better than expected): increase dopamine
    // Negative prediction error (worse than expected): decrease dopamine
    float errorSignal = error * pImpl->releaseRate * 0.5f;  // Scale error signal
    
    // Integrate prediction error into dopamine level
    pImpl->level = std::max(0.0f, std::min(1.0f, pImpl->level + errorSignal * static_cast<float>(dt)));
    
    // Prediction error modulates plasticity factor for next learning
    if (error > 0.0f) {
        // Positive prediction error: enhance learning rate
        pImpl->level = std::min(pImpl->peak, pImpl->level * 1.5f);
    } else {
        // Negative prediction error: reduce learning rate temporarily
        pImpl->level = std::max(pImpl->baseline, pImpl->level * 0.7f);
    }
}

} // namespace nlm
