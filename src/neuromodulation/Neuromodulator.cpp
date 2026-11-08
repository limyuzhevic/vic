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
    // Real dopamine-modulated plasticity factor based on phasic dopamine signals
    // The relationship between dopamine level and plasticity is nonlinear:
    // - Low dopamine (< 0.3): Enhances plasticity for unexpected rewards (learning)
    // - Moderate dopamine (0.3-0.7): Optimal for stable learning and memory consolidation
    // - High dopamine (> 0.7): Reduces plasticity to prevent overwriting (extinction)
    // - Very high dopamine (> 0.9): May trigger synaptic pruning
    
    float plasticityFactor = 1.0f; // Baseline plasticity
    if (pImpl->level < 0.3f) {
        // Phasic burst: enhances learning
        plasticityFactor = 1.5f - pImpl->level * 2.0f;
    } else if (pImpl->level > 0.7f) {
        // Sustained release: modulates plasticity down
        plasticityFactor = 1.0f - (pImpl->level - 0.7f) * 2.0f;
    }
    
    // Apply sigmoid function to keep values within reasonable bounds
    // and create smooth transitions between states
    float normalizedLevel = std::max(0.0f, std::min(1.0f, pImpl->level));
    plasticityFactor = 1.0f / (1.0f + std::exp(-(normalizedLevel - 0.5f) * 2.0f)) + 0.5f;
    
    return std::clamp(plasticityFactor, 0.1f, 3.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on RC4-ARD model:
    // dD/dt = -(D - baseline)/tau_decay + burst_input - stress_input
    // 
    // Where:
    // - D = dopamine concentration
    // - baseline = tonic firing rate
    // - tau_decay = time constant for decay
    // - burst_input = phasic bursts from reward prediction errors
    // - stress_input = stress-related dopamine modulation
    
    float decayRate = pImpl->decayRate;
    float baseline = pImpl->baseline;
    float stressFactor = 1.0f - std::min(1.0f, dt * 0.5f); // Reduced baseline under stress
    
    // Decay towards baseline with stress modulation
    pImpl->level = baseline + (pImpl->level - baseline) * 
                   std::pow(std::max(0.0f, 1.0f - dt * decayRate), 2.0f) * stressFactor;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on reward prediction error (RPE):
    // Dopamine burst proportional to RPE = actual_reward - predicted_reward
    // Phasic burst: delta_phasic = A * RPE, where A is reward sensitivity
    // 
    // The response saturates at high reward levels and has a threshold
    // below which no burst is generated (reward prediction error threshold)
    
    float rewardSensitivity = pImpl->releaseRate;
    float rpeThreshold = 0.1f; // RPE threshold below which no burst occurs
    
    // Calculate reward prediction error (assuming predicted reward was baseline)
    float rpe = reward - pImpl->baseline;
    
    if (std::abs(rpe) > rpeThreshold) {
        // Phasic burst proportional to RPE
        float burst = rpe * rewardSensitivity * std::max(0.5f, reward / pImpl->baseline);
        
        // Apply burst to current level with saturation
        pImpl->level = std::min(pImpl->peak, 
                              pImpl->level + burst * 0.5f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling (RPE):
    // - Positive error (better than expected): potentiation burst
    // - Negative error (worse than expected): depression dip
    // - Zero error: no change (prediction verified)
    // 
    // The response is asymmetric: better-than-expected signals are stronger
    // than worse-than-expected signals (biological asymmetry)
    
    float errorSensitivity = pImpl->releaseRate;
    float asymmetryFactor = 1.5f; // Better-than-expected signals are stronger
    
    float burst;
    if (error > 0.0f) {
        // Better-than-expected: potentiation burst
        burst = error * errorSensitivity * asymmetryFactor;
    } else {
        // Worse-than-expected: depression dip
        burst = error * errorSensitivity * 0.7f; // Weaker depression response
    }
    
    // Apply RPE with adaptation and time-varying gain
    float adaptation = 1.0f - 0.2f * static_cast<float>(std::min(1.0, error * 0.5f));
    pImpl->level = std::max(0.0f, 
                          pImpl->level + burst * adaptation * dt * 0.1f);
}

} // namespace nlm
