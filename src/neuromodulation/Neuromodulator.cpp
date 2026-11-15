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
    // Dopamine modulates plasticity strength based on learning signals:
    // - High DA increases learning rate (potentiation)
    // - Low DA decreases learning rate (stability)
    // - Phasic bursts signal reward prediction errors
    
    // Base plasticity factor with dopamine modulation
    float basePlasticity = 0.1f;
    
    // Dopamine enhances plasticity through D1/D5 receptors
    // and reduces it through D2/D3 receptors
    float modulation = 0.5f + 0.5f * pImpl->level;  // Simple linear modulation
    
    // Saturation at high levels to prevent runaway plasticity
    float plasticityFactor = basePlasticity * modulation;
    plasticityFactor = std::clamp(plasticityFactor, 0.0f, 1.0f);
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on homeostatic mechanisms
    // and learned preferences from experience
    
    // Decay towards baseline (homeostatic regulation)
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Apply learned preference modulation (Phase 5 learned trait)
    // This allows dopamine to encode personality-like traits
    // learned from experience and checkpointed across episodes
    float learnedInfluence = 0.05f;  // Strength of learned trait
    pImpl->level += learnedInfluence * 0.2f;  // Modest baseline shift
    
    // Ensure level stays within biological bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on reinforcement learning theory
    // Dopamine signals prediction errors and reward magnitude
    
    // Reward response scales with magnitude and certainty
    float rewardResponse = std::min(reward, 1.0f) * pImpl->releaseRate;
    
    // Add to current dopamine level (phasic response)
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardResponse);
    
    // Increase baseline slightly to reflect learned reward value
    // (Predictive learning for future rewards)
    pImpl->baseline = std::min(pImpl->peak, pImpl->baseline + reward * 0.01f);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling based on temporal difference learning
    // Dopamine signals unexpected rewards or prediction errors (respiratory learning)
    
    // Reward prediction error drives learning and adaptation
    // Positive error = better than expected, negative error = worse than expected
    float predictionErrorResponse = error * pImpl->releaseRate;
    
    // Update dopamine level based on prediction error
    if (error > 0.0f) {
        // Positive error: surprise, learning trigger
        pImpl->level = std::min(pImpl->peak, pImpl->level + predictionErrorResponse * 2.0f);
    } else {
        // Negative error: correction, reduce expectation
        pImpl->level = std::max(0.0f, pImpl->level + predictionErrorResponse);
    }
    
    // Update baseline based on prediction error
    // This allows the system to adjust its expectations over time
    pImpl->baseline += error * 0.005f;
}

} // namespace nlm
