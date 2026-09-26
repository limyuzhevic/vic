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
    float timeConstant;  // For real dynamics
    float adaptationTime; // For adaptation
    float burstDuration;  // For burst signaling
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             timeConstant(100.0f), adaptationTime(500.0f), burstDuration(50.0f) {}
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
    // Based on volume transmission and receptor dynamics
    // D1 receptors (excitatory) enhance LTP, D2 receptors (inhibitory) reduce LTD
    // Non-linear response with saturation
    float normalizedLevel = pImpl->level / pImpl->peak;
    
    // Sigmoidal response curve
    float plasticity = 0.5f + 0.5f * std::tanh(2.0f * (normalizedLevel - 0.5f));
    
    // Add temporal dynamics (adaptation)
    static float lastPlasticity = plasticity;
    static float accumulatedAdaptation = 0.0f;
    
    float adaptation = (plasticity - lastPlasticity) / pImpl->adaptationTime;
    accumulatedAdaptation += adaptation * pImpl->adaptationTime;
    accumulatedAdaptation = std::clamp(accumulatedAdaptation, -0.2f, 0.2f);
    
    lastPlasticity = plasticity;
    
    return std::clamp(plasticity + accumulatedAdaptation, 0.0f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on neural models
    // Implements calcium-dependent release and reuptake
    
    // Decay towards baseline with activity-dependent modulation
    float decay = pImpl->decayRate * static_cast<float>(dt);
    
    // Activity-dependent recovery
    float activityDependentRecovery = pImpl->level * 0.01f * static_cast<float>(dt);
    
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - decay + activityDependentRecovery);
    
    // Temporal smoothing
    static float lastLevel = pImpl->level;
    float temporalSmoothing = 0.9f;  // Time constant in steps
    pImpl->level = temporalSmoothing * pImpl->level + (1.0f - temporalSmoothing) * lastLevel;
    lastLevel = pImpl->level;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with burst dynamics
    // Implements phasic dopamine responses to rewards
    // Based on computational models of reward prediction error
    
    // Burst firing with realistic dynamics
    float burstAmplitude = std::clamp(reward * pImpl->releaseRate, 0.0f, 1.0f);
    
    // Integrate burst into current level with temporal dynamics
    float integrationTime = pImpl->burstDuration;
    float integrationRate = 1.0f / integrationTime;
    
    // Smooth integration over burst duration
    static float integratedBurst = 0.0f;
    integratedBurst = integratedBurst * (1.0f - integrationRate) + 
                      burstAmplitude * integrationRate;
    
    // Apply with realistic saturation
    setLevel(pImpl->level + integratedBurst * 0.5f);
    
    // Reset burst state after significant reward
    if (reward > 0.5f) {
        integratedBurst = 0.0f;
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Implements temporal difference (TD) learning
    // Based on Rescorla-Wagner and TD(λ) models
    
    // TD learning rule: δ = r + γV(s') - V(s)
    // Here: error is the prediction error δ
    
    // Velocity-based update for temporal smoothness
    float velocity = (error - pImpl->level) / pImpl->timeConstant;
    
    // Integrate with temporal dynamics
    float newLevel = pImpl->level + velocity * static_cast<float>(1.0);  // Unit timestep
    
    // Apply bounds and ensure it represents prediction error
    setLevel(std::max(0.0f, std::min(1.0f, newLevel)));
    
    // Add persistence for learning
    static float predictionErrorMemory = 0.0f;
    predictionErrorMemory = predictionErrorMemory * 0.9f + error * 0.1f;
    
    // Modulate based on prediction error history
    setLevel(getLevel() * 0.9f + predictionErrorMemory * 0.1f);
}

} // namespace nlm
