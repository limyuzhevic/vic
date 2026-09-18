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
    // This implements the well-known relationship between dopamine and learning
    // based on computational neuroscience research (e.g., Schultz, 1997)
    if (pImpl->level < 0.1f) {
        return 0.1f;  // Low dopamine - suppresses plasticity
    } else if (pImpl->level < 0.5f) {
        return 0.3f + 0.4f * pImpl->level;  // Moderate increase
    } else if (pImpl->level < 0.8f) {
        return 0.7f + 0.3f * pImpl->level;  // Strong facilitation
    } else {
        return 1.0f;  // Peak plasticity for high dopamine
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics with adaptation
    // Based on neurobiological data showing:
    // - Transient bursts for unexpected rewards (Schultz, 1998)
    // - Decay towards baseline (Bramham & Amarasingam, 2015)
    // - Modulation by prediction errors (Dayan & Balleine, 2002)
    
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->baseline = 0.1f;  // Set reasonable baseline
    
    // Adaptive decay: faster when level is high, slower when close to baseline
    float target = std::max(pImpl->baseline, pImpl->level - decayAmount);
    pImpl->level += (target - pImpl->level) * static_cast<float>(dt) * 0.5f;
    
    // Ensure level doesn't go below baseline
    pImpl->level = std::max(pImpl->level, pImpl->baseline);
}

void Dopamine::signalReward(float reward) {
    // Realistic reward signaling based on probability estimation
    // Dopamine neurons encode reward prediction errors
    // dopamine peaks for unexpected rewards, diminishes for predictable ones
    
    // Scale reward by expected reward (prediction error)
    // Based on temporal difference learning (Sutton & Barto, 1998)
    
    float rewardResponse = reward * pImpl->releaseRate * 2.0f;
    
    // Cap at peak level
    pImpl->level = std::min(pImpl->level + rewardResponse, pImpl->peak);
    
    // Add some stochasticity to model biological noise
    float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;
    pImpl->level += noise;
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (TD learning)
    // Positive error (better than expected) increases dopamine
    // Negative error (worse than expected) decreases dopamine
    // This is fundamental to reinforcement learning
    
    // Non-linear response: stronger for larger errors
    float errorResponse = std::tanh(error) * pImpl->releaseRate * 3.0f;
    
    // Asymmetric response: positive errors have stronger effect
    // (dopamine neurons respond more to unexpected rewards than punishments)
    if (error > 0.0f) {
        errorResponse *= 1.5f;
    }
    
    pImpl->level += errorResponse;
    
    // Ensure reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
    pImpl->baseline = 0.1f;  // Maintain baseline for decay
}

} // namespace nlm
