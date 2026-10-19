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
    // Based on biological research: Dopamine has an inverted U-shaped effect on plasticity
    // Moderate dopamine levels optimize plasticity, both too high and too low reduce it
    float level = pImpl->level;
    float plasticity = 1.0f;
    
    if (level < 0.3f) {
        // Low dopamine: Reduced plasticity (inability to learn)
        plasticity = 0.3f + level * 0.7f;
    } else if (level < 0.7f) {
        // Moderate dopamine: Optimal plasticity (learning in progress)
        plasticity = 0.9f - 0.5f * std::abs(level - 0.5f);
    } else {
        // High dopamine: Reduced plasticity (overgeneralization)
        plasticity = 0.6f * (1.0f - level);
    }
    
    // Ensure plasticity is in valid range
    plasticity = std::clamp(plasticity, 0.1f, 1.5f);
    return plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics based on biological models
    // Uses temporal difference learning and reward prediction error
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline (homeostatic regulation)
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Apply prediction error signal (temporal difference learning)
    // This simulates how dopamine encodes reward prediction errors
    float predictionError = pImpl->level - pImpl->baseline;
    pImpl->level += predictionError * 0.1f * dtFloat;
    
    // Clip to reasonable range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Store peak for reference
    pImpl->peak = std::max(pImpl->peak, pImpl->level);
}

void Dopamine::signalReward(float reward) {
    // Implement real reward signaling based on biological research
    // Dopamine neurons show phasic responses to unexpected rewards
    // Response magnitude scales with reward prediction error
    
    // Calculate reward prediction error (RPE)
    float rpe = reward - pImpl->baseline;
    
    // Normalize reward to 0-1 range based on typical reward magnitudes
    float normalizedRpe = rpe * 0.1f;  // Scale factor
    
    // Apply RPE to dopamine level with biological constraints
    float burstMagnitude = std::clamp(normalizedRpe, -0.5f, 1.0f);
    pImpl->level = std::clamp(pImpl->level + burstMagnitude, 0.0f, 1.0f);
    
    // Update expected reward (learning the value)
    pImpl->baseline = 0.95f * pImpl->baseline + 0.05f * reward;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling
    // This is key for temporal difference learning and reinforcement learning
    // Phasic dopamine responses encode prediction errors (positive for unexpected rewards)
    
    // Apply prediction error to dopamine level
    // Positive error (better than expected) -> increase dopamine
    // Negative error (worse than expected) -> decrease dopamine
    float rpeResponse = std::clamp(error * 0.2f, -0.8f, 0.8f);
    pImpl->level = std::clamp(pImpl->level + rpeResponse, 0.0f, 1.0f);
    
    // Update baseline based on prediction error
    // This simulates learning the expected value
    pImpl->baseline = 0.9f * pImpl->baseline + 0.1f * (pImpl->level + error);
}

} // namespace nlm
