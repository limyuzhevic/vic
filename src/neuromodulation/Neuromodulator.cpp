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

Dopamine::Dopamine() : pImpl(new Impl) {
    // Initialize dopamine neuron state
    // Baseline dopamine is not zero - it provides tonic activity
    pImpl->baseline = 0.05f;  // Physiological baseline level
    pImpl->level = pImpl->baseline;
}

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
    // Biologically plausible dopamine-modulated plasticity factor
    // Based on empirical data: dopamine modulates plasticity in a non-linear way
    // Low dopamine reduces plasticity (rigidity), moderate dopamine optimizes
    // learning rate, high dopamine can impair learning (distraction)
    
    if (pImpl->level <= 0.0f) {
        return 0.1f;  // Low baseline plasticity when no dopamine
    } else if (pImpl->level < 0.5f) {
        // Increasing returns: learning accelerates with moderate dopamine
        return 0.1f + 0.8f * pImpl->level;
    } else {
        // Diminishing returns: excessive dopamine impairs learning
        return 0.9f - 0.8f * (pImpl->level - 0.5f);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Biologically plausible dopamine dynamics
    // Dopamine has multiple components: tonic and phasic, with different dynamics
    
    // Tonic component: decays towards baseline, influenced by novelty
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add small noise for realistic fluctuations
    pImpl->level += (pImpl->level * 0.01f) * static_cast<float>(dt);
}

void Dopamine::signalReward(float reward) {
    // Biologically realistic reward signaling
    // Dopamine neurons show phasic bursts on unexpected rewards
    // The burst magnitude depends on reward prediction error magnitude
    
    // Baseline dopamine increase (tonic component)
    pImpl->baseline = std::min(0.2f, pImpl->baseline + reward * 0.05f);
    
    // Phasic burst: proportional to reward prediction error (when positive)
    if (reward > 0.0f) {
        float burstMagnitude = std::min(0.8f, reward * 0.04f);
        
        // Dopamine neurons adapt to bursts: less responsive to repeated rewards
        float adaptationFactor = std::max(0.1f, 1.0f - pImpl->level * 0.1f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude * adaptationFactor);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling based on temporal difference learning
    // This is what dopamine neurons actually encode (Schultz et al., 1997)
    
    // Phasic prediction error response
    // Positive error (better than expected): reward prediction signal
    // Negative error (worse than expected): punishment prediction signal
    if (error > 0.0f) {
        // Phasic burst for positive prediction error
        float burst = std::min(0.8f, error * 0.4f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    } else if (error < 0.0f) {
        // Phasic pause for negative prediction error
        // Short-term suppression of baseline
        pImpl->baseline = std::max(0.0f, pImpl->baseline + error * 0.05f);
        // Brief dip in tonic activity
        pImpl->level = std::max(0.0f, pImpl->level + error * 0.1f);
    }
}

} // namespace nlm
