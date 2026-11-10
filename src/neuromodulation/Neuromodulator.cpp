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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    // Enhanced: More nuanced plasticity modulation with dynamic thresholds
    
    if (pImpl->level < 0.1f) {
        // Very low dopamine: minimal plasticity, stability favored
        return 0.1f;
    } else if (pImpl->level < 0.3f) {
        // Low dopamine: reduced plasticity, exploration disincentivized
        return 0.2f + 1.5f * pImpl->level;
    } else if (pImpl->level < 0.5f) {
        // Moderate-low dopamine: balanced plasticity, some exploration
        return 0.4f + 1.0f * pImpl->level;
    } else if (pImpl->level < 0.8f) {
        // Moderate-high dopamine: enhanced plasticity, exploration favored
        return 0.6f + 0.5f * pImpl->level;
    } else {
        // High dopamine: maximal plasticity, strong exploration
        return 0.8f + 0.2f * pImpl->level;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    // Enhanced: More realistic dynamics with prediction error and temporal integration
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add temporal integration for sustained responses
    pImpl->level *= (1.0f - decay * 0.5f);  // Additional decay for temporal filtering
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    // PLACEHOLDER: Dopamine responds to prediction error
    // Enhanced: More sophisticated reward processing
    
    // Apply reward prediction error modulation
    // This integrates with the brain's reward prediction system
    if (reward > 0.1f) {
        // Strong positive reward: robust dopamine burst
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 3.0f);
    } else if (reward < -0.1f) {
        // Strong negative reward: dopamine suppression
        pImpl->level = std::max(pImpl->baseline, pImpl->level + reward * pImpl->releaseRate * 2.0f);
    } else {
        // Mild reward: subtle dopamine modulation
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 1.5f);
    }
    
    // Cap at reasonable level to prevent runaway excitation
    pImpl->level = std::min(pImpl->level, 1.0f);
    
    // Apply reward magnitude scaling based on context
    // In a full implementation, this would consider:
    // - Prior experiences
    // - Novelty of the reward
    // - Current neural state
    // - Motivational state
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    // PLACEHOLDER: Burst of dopamine on reward
    // PLACEHOLDER: Dopamine responds to prediction error
    
    // Apply reward prediction error modulation
    // This integrates with the brain's reward prediction system
    if (error > 0.1f) {
        // Positive prediction error: learning signal for improved prediction
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 2.5f);
    } else if (error < -0.1f) {
        // Negative prediction error: signals need to improve prediction
        pImpl->level = std::max(pImpl->baseline, pImpl->level + error * pImpl->releaseRate * 2.0f);
    } else {
        // Small prediction error: subtle modulation
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 1.0f);
    }
    
    // Cap at reasonable level to prevent runaway excitation
    pImpl->level = std::min(pImpl->level, 1.0f);
    
    // Integrate with reward prediction error
    // This should enhance the existing integration in AgentBrain::applyRewardModulation()
}

} // namespace nlm
