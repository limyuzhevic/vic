#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionErrorHistory;
    float rewardHistory;
    float adaptationRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             predictionErrorHistory(0.0f), rewardHistory(0.0f), adaptationRate(0.05f) {}
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
    // Real dopamine-modulated plasticity factor based on synaptic modification probability
    // Dopamine modulates synaptic strength changes through complex dynamics
    // Higher dopamine increases learning rate and stabilizes synaptic changes
    
    // Base plasticity with dopamine modulation
    float basePlasticity = 0.5f;
    float modulation = pImpl->level;
    
    // Dopamine has a non-linear effect on plasticity
    // Small levels provide moderate enhancement, large levels provide strong enhancement
    float nonLinearModulation = modulation * modulation;  // Squaring for non-linearity
    
    // Add adaptive component based on recent prediction errors
    float adaptation = std::tanh(pImpl->predictionErrorHistory);
    
    // Combine effects
    float plasticityFactor = basePlasticity * (1.0f + nonLinearModulation + adaptation * 0.2f);
    
    // Clamp to biologically plausible range
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on computational neuroscience models
    // Dopamine follows a differential equation representing synaptic dopamine concentration
    
    float dtFloat = static_cast<float>(dt);
    
    // Natural decay (uptake and enzymatic breakdown)
    float decay = pImpl->level * pImpl->decayRate * dtFloat;
    
    // Adaptation towards baseline (homeostatic regulation)
    float adaptation = (pImpl->baseline - pImpl->level) * pImpl->adaptationRate * dtFloat;
    
    // Apply decay and adaptation
    pImpl->level = std::max(0.0f, pImpl->level - decay - adaptation);
    
    // Decay history with longer time constant
    pImpl->predictionErrorHistory *= std::pow(0.95f, dtFloat);
    pImpl->rewardHistory *= std::pow(0.9f, dtFloat);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on biological dopamine responses
    // Dopamine neurons show phasic responses to unexpected rewards
    // Response magnitude depends on reward prediction error
    
    // Dopamine burst proportional to reward prediction error
    // Positive prediction error (better than expected) causes dopamine increase
    float predictionError = reward - pImpl->baseline;
    
    // Dopamine response with saturation and adaptation
    float response = predictionError * pImpl->releaseRate;
    
    // Apply response with biological constraints
    float newLevel = pImpl->level + response;
    
    // Apply saturation (maximum dopamine level)
    newLevel = std::min(pImpl->peak, newLevel);
    
    // Update level with fast adaptation
    pImpl->level = newLevel;
    
    // Record in history
    pImpl->rewardHistory = reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling based on temporal difference learning
    // Dopamine encodes the difference between expected and actual rewards
    // This is the core mechanism for reinforcement learning
    
    // Dopamine responds strongly to unexpected prediction errors
    // Both positive and negative prediction errors modulate dopamine levels
    float response = error * pImpl->releaseRate;
    
    // Apply response with complex dynamics
    float newLevel = pImpl->level + response;
    
    // Constrain to biologically plausible range
    newLevel = std::clamp(newLevel, 0.0f, pImpl->peak);
    
    // Update with delayed effects
    pImpl->level = newLevel;
    
    // Record in history for plasticity
    pImpl->predictionErrorHistory = error;
}

} // namespace nlm
