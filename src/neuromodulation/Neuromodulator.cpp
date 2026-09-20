#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionErrorHistory[10]; // For prediction error integration
    size_t historyIndex;
    float eligibilityTrace; // For reward prediction error credit assignment
    
    Impl() : level(0.0f), baseline(0.05f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             historyIndex(0), eligibilityTrace(0.0f) {
        for (size_t i = 0; i < 10; ++i) {
            predictionErrorHistory[i] = 0.0f;
        }
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
    // Realistic dopamine-modulated plasticity: 
    // - Optimal learning at moderate levels (around 0.5)
    // - High levels induce LTP, low levels induce LTD
    // - Implements dopamine prediction error in reinforcement learning
    float error = std::abs(pImpl->level - 0.5f);
    float baseFactor = 0.5f;
    return baseFactor + (1.0f - error * 2.0f);  // Bell curve around 0.5
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics:
    // - Natural phasic activity in response to reward prediction errors
    // - Tonic baseline for steady state
    // - Decay and integration of prediction errors
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Integrate prediction error history
    if (pImpl->historyIndex > 0) {
        float error = pImpl->predictionErrorHistory[pImpl->historyIndex - 1];
        pImpl->level += error * pImpl->releaseRate * 0.1f;
        pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    }
    
    // Update eligibility trace (for credit assignment)
    if (pImpl->eligibilityTrace > 0.001f) {
        pImpl->eligibilityTrace *= 0.9f; // Decay eligibility
    }
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with temporal dynamics
    // Phasic dopamine burst proportional to reward prediction error
    float burst = std::min(reward * pImpl->releaseRate, pImpl->peak - pImpl->level);
    pImpl->level += burst;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (key mechanism in RL)
    // Negative error (worse than expected) -> dip in dopamine
    // Positive error (better than expected) -> burst in dopamine
    
    // Store in history for integration
    pImpl->predictionErrorHistory[pImpl->historyIndex] = error;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 10;
    
    // Immediate response
    pImpl->level += error * pImpl->releaseRate;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Set eligibility trace for later weight updates
    if (std::abs(error) > 0.01f) {
        pImpl->eligibilityTrace = std::min(1.0f, std::abs(error) * 0.5f);
    }
}

} // namespace nlm
