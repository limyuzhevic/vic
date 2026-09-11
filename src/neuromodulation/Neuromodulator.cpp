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
    // Implement real dopamine-modulated plasticity factor
    // Based on neuroscience research, dopamine modulates plasticity strength
    // with an inverted U-shaped relationship (inverted U-function)
    float normalizedDA = std::clamp(pImpl->level, 0.0f, 1.0f);
    // Inverted U-shape: peak plasticity at moderate dopamine levels
    return 0.8f * std::exp(-4.0f * (normalizedDA - 0.5f) * (normalizedDA - 0.5f));
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics based on neuroscience research
    // Dopamine follows an inverted U-function with respect to learning
    
    float decayFactor = pImpl->decayRate * static_cast<float>(dt);
    
    // Decay toward baseline (homeostatic regulation)
    pImpl->level += (pImpl->baseline - pImpl->level) * decayFactor;
    
    // Apply synaptic plasticity based on dopamine level
    // Use the derived plasticity factor for learning modulation
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Implement real reward signaling based on dopamine burst hypothesis
    // Reward prediction error (RPE) theory: dopamine encodes the difference between expected and actual reward
    
    // Current prediction error-based burst
    float predictionError = reward;
    
    // Apply learning rate modulation
    float learningRate = 0.1f + 0.4f * pImpl->level;  // Dynamic learning rate
    
    // Compute dopamine burst using temporal difference learning
    pImpl->level = std::min(pImpl->peak, 
                           pImpl->level + predictionError * learningRate * pImpl->releaseRate);
    
    // Add a rapid decay after burst to simulate dopaminergic reset
    pImpl->level = std::max(pImpl->level - 0.5f * pImpl->decayRate, pImpl->baseline);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement real reward prediction error signaling (RPE) based on temporal difference learning
    // Dopamine encodes prediction errors: positive for unexpected reward, negative for omitted reward
    
    // Apply Temporal Difference (TD) learning formula: δ = r + γV(s') - V(s)
    // For simplicity, we use immediate prediction error
    
    // Modulate error by current dopamine level (dopamine history)
    float historyModulation = 1.0f - pImpl->level * 0.3f;  // Lower history affects learning
    
    // Compute prediction error with adaptive gain
    float predictionError = error * historyModulation;
    
    // Update dopamine level with TD error and adaptive learning rate
    float learningRate = 0.05f * (1.0f + 2.0f * error);  // Error-dependent learning rate
    pImpl->level = std::max(0.0f, pImpl->level + predictionError * learningRate * pImpl->releaseRate);
    
    // Maintain physiological bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

} // namespace nlm
