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
    float adaptationTime;
    float previousReward;
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.2f), releaseRate(2.0f)
            , adaptationTime(100.0f), previousReward(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {
    // Initialize with baseline level
    pImpl->level = pImpl->baseline;
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

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity factor implementation
    // Based on biological evidence: moderate dopamine levels optimize plasticity
    // Low dopamine: reduced plasticity, high dopamine: saturation of plasticity
    
    if (pImpl->level < 0.2f) {
        // Low dopamine - reduced plasticity (stabilization)
        return 0.1f + 0.6f * (pImpl->level / 0.2f);
    } else if (pImpl->level > 0.8f) {
        // High dopamine - saturation of plasticity (reduced learning efficiency)
        return 1.0f - 0.8f * ((pImpl->level - 0.8f) / 0.2f);
    } else {
        // Optimal dopamine range - maximal plasticity
        return 0.7f + 0.6f * ((pImpl->level - 0.2f) / 0.6f);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics implementation
    // Includes: exponential decay, reward prediction error integration, adaptation
    
    float decay = 1.0f - std::exp(-pImpl->decayRate * static_cast<float>(dt));
    
    // Decay towards baseline
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * (1.0f - decay);
    
    // Adapt baseline based on previous rewards
    if (std::abs(pImpl->previousReward) > 0.001f) {
        float adaptation = 1.0f - std::exp(-1.0f / pImpl->adaptationTime);
        pImpl->baseline = pImpl->baseline * (1.0f - adaptation) + 
                         pImpl->previousReward * adaptation;
        pImpl->baseline = std::clamp(pImpl->baseline, 0.0f, 1.0f);
    }
    
    // Clamp to ensure stability
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling implementation
    // Uses reward prediction error with temporal difference learning
    
    if (std::abs(reward - pImpl->previousReward) > 0.001f) {
        // Reward prediction error
        float rpe = reward - pImpl->previousReward;
        
        // Response magnitude depends on prediction error
        float response = std::tanh(rpe * pImpl->releaseRate);
        
        // Apply response to dopamine level
        pImpl->level = std::clamp(pImpl->level + response, 0.0f, 1.0f);
    }
    
    // Store current reward for next update
    pImpl->previousReward = reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling implementation
    // Dopamine neurons encode reward prediction error as in dopamine hypothesis
    
    // Time-varying synaptic efficacy for learning
    float learningRate = 0.01f * (1.0f + 0.5f * pImpl->level);
    learningRate = std::clamp(learningRate, 0.001f, 0.1f);
    
    // Integration of prediction error
    float integratedError = error * learningRate;
    
    // Apply with temporal dynamics
    float decay = 1.0f - std::exp(-0.5f);  // Fixed timescale
    pImpl->level = pImpl->level * decay + integratedError * (1.0f - decay);
    
    // Ensure positive dopamine levels
    pImpl->level = std::max(0.0f, pImpl->level);
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->previousReward = 0.0f;
}

} // namespace nlm
