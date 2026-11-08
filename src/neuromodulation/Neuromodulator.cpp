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
    // PHASE 2: Implement real dopamine-modulated plasticity factor
    // Biological: Dopamine modulates synaptic plasticity in complex ways
    // Implementation: Inverted-U curve for optimal learning
    
    // Convert dopamine level to plasticity factor
    // Use an inverted-U function: moderate dopamine promotes learning
    // Low dopamine: reduced motivation and learning
    // High dopamine: potential interference with learning
    
    float normalizedLevel = pImpl->level;
    
    // Inverted-U function centered at 0.5 with width 0.3
    float distanceFromOptimal = std::abs(normalizedLevel - 0.5f);
    float plasticityFactor = 1.0f - 4.0f * distanceFromOptimal * distanceFromOptimal;
    
    // Ensure non-negative result
    plasticityFactor = std::max(0.0f, plasticityFactor);
    
    // Scale to reasonable range (0.1 to 1.0)
    plasticityFactor = 0.1f + plasticityFactor * 0.9f;
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // PHASE 2: Implement real dopamine dynamics
    // Biological: Dopamine decays exponentially but can be sustained by novel events
    // Implementation: exponential decay with recovery toward baseline
    
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add some spontaneous fluctuations for realistic dynamics
    // Small random variations that could represent internal state changes
    if (dt > 0.0f) {
        float spontaneousFluctuation = 0.05f * static_cast<float>(dt);
        pImpl->level += spontaneousFluctuation * 0.5f;
    }
}

void Dopamine::signalReward(float reward) {
    // PHASE 2: Implement real dopamine dynamics
    // Biological: Unexpected rewards cause phasic bursts; expected rewards cause weaker responses
    // Enhanced: Use prediction error-style modulation for learning
    
    // Compute reward prediction error (simplified)
    // In real TD learning, this would compare actual reward to predicted reward
    // For now, use the raw reward as a proxy for prediction error
    
    // Phasic burst response to unexpected rewards
    if (reward > 0.0f) {
        // Stronger response to larger unexpected rewards
        pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate * 2.0f);
        
        // Add some temporal summation for sustained response
        pImpl->peak = std::max(pImpl->peak, reward * 1.5f);
    } else {
        // Depressive response to negative outcomes or punishments
        float depressiveEffect = -reward * 0.5f; // Negative reward strength
        pImpl->level = std::max(pImpl->baseline, pImpl->level - depressiveEffect);
    }
    
    // Ensure level doesn't exceed reasonable bounds
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

void Dopamine::signalRewardPredictionError(float error) {
    // PHASE 2: Implement reward prediction error signaling
    // Biological: Dopamine encodes reward prediction errors (RPE)
    // Phasic increases for positive RPE, decreases for negative RPE
    // RPE = actual reward - predicted reward
    
    if (error > 0.0f) {
        // Positive prediction error (better than expected)
        // This is a learning signal that reinforces the current behavior
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 1.5f);
        
        // Novelty detection: unexpected positive outcomes promote exploration
        if (error > 0.5f) {
            // Significant positive RPE indicates novel/valuable outcome
            pImpl->peak = std::max(pImpl->peak, 1.0f);
        }
    } else if (error < -0.1f) {
        // Negative prediction error (worse than expected)
        // This is a learning signal that suppresses the current behavior
        float depressiveEffect = -error * 0.3f; // Convert negative error to depression strength
        pImpl->level = std::max(pImpl->baseline, pImpl->level - depressiveEffect);
        
        // Negative RPE may indicate need for exploration or strategy change
    } else {
        // Small negative errors or near-zero errors
        // Minor adjustments to maintain homeostasis
        if (std::abs(error) < 0.1f) {
            // Very small error, just maintain current level
            // Some spontaneous activity for ongoing regulation
            pImpl->level += 0.01f * static_cast<float>(error > 0.0f ? 1 : -1);
        }
    }
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

} // namespace nlm
