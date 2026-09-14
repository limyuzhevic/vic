#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float adaptationRate;
    float predictionError;
    float expectedReward;
    float actualReward;
    mutable std::mt19937 rng;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), 
             releaseRate(1.5f), adaptationRate(0.1f), predictionError(0.0f),
             expectedReward(0.0f), actualReward(0.0f) {
        rng.seed(std::random_device()());
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
    // Real dopamine-modulated plasticity: higher DA increases plasticity up to optimal level
    // Use inverted U-shape response: optimal at moderate levels
    float optimalLevel = 0.6f;
    float plasticity = 0.0f;
    
    if (pImpl->level <= optimalLevel) {
        plasticity = pImpl->level / optimalLevel; // Linear increase up to optimal
    } else {
        // Decrease past optimal level (too much dopamine impairs learning)
        plasticity = 1.0f - ((pImpl->level - optimalLevel) / (1.0f - optimalLevel)) * 0.5f;
    }
    
    return std::clamp(plasticity, 0.0f, 1.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with adaptive baseline
    if (dt <= 0.0) return;
    
    // Decay towards baseline with time constant
    float decayFactor = std::exp(-pImpl->decayRate * dt);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Small spontaneous fluctuations
    std::uniform_real_distribution<float> dist(-0.01f, 0.01f);
    pImpl->level += dist(pImpl->rng);
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with burst and adaptation
    pImpl->actualReward = reward;
    pImpl->expectedReward = pImpl->actualReward; // In simple model, expected equals actual
    
    // Calculate reward prediction error
    pImpl->predictionError = reward - pImpl->expectedReward;
    
    // Burst response proportional to reward magnitude
    float burstIntensity = std::min(std::abs(reward) * 0.8f, 0.8f);
    
    if (reward > 0.0f) {
        // Phasic increase for positive reward
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstIntensity * pImpl->releaseRate);
    } else if (reward < 0.0f) {
        // Phasic decrease for negative reward
        pImpl->level = std::max(pImpl->baseline, pImpl->level - std::abs(reward) * pImpl->releaseRate * 0.5f);
    }
    
    // Adaptive response modulation
    pImpl->baseline = pImpl->baseline + pImpl->adaptationRate * (reward - pImpl->baseline);
    pImpl->baseline = std::clamp(pImpl->baseline, 0.0f, 0.5f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling for learning
    pImpl->predictionError = error;
    
    // Error-based dopamine response: prediction errors update dopamine level
    if (error > 0.0f) {
        // Positive prediction error: unexpected reward
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 1.2f);
    } else if (error < 0.0f) {
        // Negative prediction error: worse than expected
        pImpl->level = std::max(pImpl->baseline, pImpl->level + error * pImpl->releaseRate * 0.8f);
    }
    
    // Track for plasticity calculations
    pImpl->expectedReward += error;
    pImpl->actualReward += error;
}

} // namespace nlm
