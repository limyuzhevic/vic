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
    // Real dopamine-modulated plasticity factor
    // Based on experimental findings: moderate dopamine levels optimize plasticity,
    // both too low and too high levels impair learning
    // Plasticity follows an inverted-U relationship with dopamine
    
    float normalizedLevel = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Inverted-U function: optimal plasticity around 0.5-0.7 dopamine
    float optimalLevel = 0.6f;
    float width = 0.4f;  // Width of the optimal range
    
    float diff = std::abs(normalizedLevel - optimalLevel);
    float plasticity = 1.0f - diff / width;
    
    // Ensure minimum plasticity to maintain some learning capability
    return std::max(0.1f, plasticity);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with multiple time scales
    // Based on prefrontal cortex dopamine dynamics
    
    static_cast<float>(dt); // Suppress unused parameter warning for now
    
    // Short-term dynamics: phasic dopamine release
    // Fast decay towards baseline
    if (pImpl->level > pImpl->baseline) {
        float decay = pImpl->decayRate * (1.0f + pImpl->level * 2.0f);  // Faster decay at higher levels
        pImpl->level = std::max(pImpl->baseline, pImpl->level - decay * 0.01f);
    }
    
    // Long-term dynamics: background dopamine tone
    // Slow integration with behavioral context
    // This is a simplified version - real dopamine has complex multimodal integration
    
    // Maintain reasonable bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on dopamine neuron responses
    // Dopaminergic neurons show phasic bursts proportional to reward prediction
    
    // Predict reward response from reward magnitude and expectation
    float expectedValue = pImpl->baseline + pImpl->peak * 0.3f;  // Moderate expectation
    float rewardError = reward - expectedValue;
    
    // Dopamine burst proportional to reward prediction error (RL theory)
    float burstMagnitude = std::max(0.0f, rewardError * pImpl->releaseRate * 2.0f);
    
    // Apply burst with ceiling at peak level
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
    
    // Ensure minimum level to maintain signal
    if (pImpl->level < 0.1f) {
        pImpl->level = 0.1f;
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling
    // Dopamine neurons encode temporal difference prediction errors
    
    // Dopamine responds to both positive and negative prediction errors
    // But with asymmetry: positive errors (better than expected) produce larger responses
    float errorResponse = error * pImpl->releaseRate;
    
    // Positive errors (surprise of better reward) produce stronger bursts
    if (error > 0.0f) {
        errorResponse *= 1.5f;  // Amplify positive prediction errors
        errorResponse = std::min(errorResponse, 0.5f);  // Cap magnitude
    }
    
    // Negative errors (worse than expected) produce smaller responses
    // and can lead to dopamine suppression
    if (error < 0.0f) {
        errorResponse *= 0.7f;  // Dampen negative prediction errors
        errorResponse = std::max(errorResponse, -0.3f);  // Prevent excessive negative
    }
    
    // Apply prediction error signal
    pImpl->level += errorResponse;
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
}

} // namespace nlm
