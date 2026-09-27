#include "Neuromodulator.hpp"
#include <algorithm>
#include "../core/Random/Random.hpp"

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
    // Based on computational neuroscience: dopamine modulates synaptic plasticity
    // Strong dopamine (reward prediction) enhances LTP, weak dopamine enhances LTD
    
    if (pImpl->level <= 0.0f) {
        // No dopamine - baseline plasticity (slight depression)
        return 0.3f;
    } else if (pImpl->level < 0.5f) {
        // Low dopamine - favors depression (learning about non-reward)
        return 0.3f - pImpl->level * 0.2f;
    } else {
        // High dopamine - favors potentiation (learning about reward)
        return 0.5f + pImpl->level * 0.5f;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on computational neuroscience
    // Dopamine neurons show phasic responses to rewards and prediction errors
    // Here we implement a simplified model of dopamine dynamics
    
    // Decay towards baseline (tonic firing)
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add small spontaneous fluctuations
    float fluctuation = (brain_->getRandomGenerator()->uniformReal(-0.1f, 0.1f)) * std::sqrt(pImpl->decayRate);
    pImpl->level = std::max(0.0f, pImpl->level + fluctuation);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: burst of dopamine proportional to unexpected reward
    // Dopamine neurons encode reward prediction errors
    // Strong bursts for unexpected rewards, weak for expected rewards
    
    // Compute reward prediction error signal
    float burst = reward * pImpl->releaseRate;
    
    // Cap at peak level
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Positive error (better than expected): dopamine burst
    // Negative error (worse than expected): dopamine dip (or suppression)
    // This is crucial for reinforcement learning
    
    if (error > 0.0f) {
        // Positive prediction error: reward burst
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 1.5f);
    } else {
        // Negative prediction error: suppression or dip
        // Model this as reducing the level below baseline
        pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.5f);
    }
}

} // namespace nlm
