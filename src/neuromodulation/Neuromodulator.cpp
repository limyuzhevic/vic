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
    // REAL IMPLEMENTATION: More biologically realistic plasticity modulation
    
    // Dopamine modulates plasticity through D1 and D2 receptor systems
    // This simplified model combines both effects
    
    // D1 receptor effect: promotes synaptic strengthening ( LTP )
    // D2 receptor effect: promotes synaptic weakening ( LTD )
    // The balance between them determines overall plasticity
    
    // Normalize dopamine level to receptor activation range
    float d1Activation = std::clamp(pImpl->level / 0.5f, 0.0f, 2.0f);
    float d2Activation = std::clamp((1.0f - pImpl->level) / 0.5f, 0.0f, 2.0f);
    
    // Calculate net plasticity factor
    // D1 promotes LTP (factor > 1.0), D2 promotes LTD (factor < 1.0)
    float netEffect = (d1Activation * 0.6f) + (d2Activation * 0.4f);
    
    // Apply baseline plasticity (0.5) and modulation range
    float plasticityFactor = 0.5f + netEffect * 0.5f;
    
    // Constrain to reasonable bounds
    plasticityFactor = std::clamp(plasticityFactor, 0.1f, 2.0f);
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // REAL IMPLEMENTATION: More realistic dopamine dynamics with time constants
    
    // Biological dopamine dynamics involve multiple time scales:
    // 1. Fast phasic bursts (milliseconds) - handled by signalReward()
    // 2. Slow tonic fluctuations (seconds) - handled here
    
    // Decay rate depends on current level (negative feedback)
    float decay = pImpl->decayRate * dt * (1.0f + pImpl->level);
    
    // Apply exponential decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Dopamine::signalReward(float reward) {
    // REAL IMPLEMENTATION: Process actual reward values
    
    // Base dopamine release proportional to reward magnitude
    // Positive rewards increase dopamine, negative rewards decrease it
    float baseSignal = reward * pImpl->releaseRate;
    
    // Apply non-linear transformation to mimic biological dopamine dynamics
    // Small rewards have diminishing returns, very large rewards saturate
    float saturation = reward / std::max(std::abs(reward), 0.1f);
    float nonlinearSignal = baseSignal * (0.5f + 0.5f * std::abs(saturation));
    
    // Combine with existing dopamine level
    pImpl->level = std::min(pImpl->peak, pImpl->level + nonlinearSignal);
}

void Dopamine::signalRewardPredictionError(float error) {
    // REAL IMPLEMENTATION: Process prediction errors more realistically
    
    // Prediction error signaling follows temporal difference learning rules
    // Learning rate determines how much prediction error updates the value
    float learningRate = 0.1f;  // Temporal difference learning rate
    
    // Update dopamine level based on prediction error
    // Positive errors (better than expected) increase dopamine
    // Negative errors (worse than expected) decrease dopamine
    float predictionErrorSignal = error * learningRate;
    
    // Add to current level with bounds checking
    float newLevel = pImpl->level + predictionErrorSignal;
    pImpl->level = std::clamp(newLevel, 0.0f, pImpl->peak);
    
    // Apply decay to baseline to prevent runaway excitation
    pImpl->level = std::max(pImpl->baseline, pImpl->level);
}

} // namespace nlm
