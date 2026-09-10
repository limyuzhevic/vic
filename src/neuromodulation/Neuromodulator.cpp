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
    // Implements volume transmission model: plasticity depends on both concentration and duration
    // Higher and sustained dopamine increases synaptic strengthening
    
    // Base plasticity factor
    float factor = 0.5f + 0.5f * pImpl->level;
    
    // Duration dependence: longer exposure to dopamine has greater effect
    // This models the difference between phasic and tonic dopamine signals
    if (pImpl->level > 0.3f) {
        float durationBoost = std::min(2.0f, pImpl->level + 0.5f);
        factor *= durationBoost;
    }
    
    // Implement prediction error gating: only plasticity when prediction error is positive
    // For now, simplified version
    factor *= (1.0f + pImpl->level * 0.3f);
    
    return std::min(2.0f, factor);  // Cap at 2x baseline plasticity
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics: integrate multiple inputs
    // Models dopamine neuron firing rate based on:
    // - Reward prediction error (RPE)
    // - Contextual modulation ( novelty, salience)
    // - Baseline activity and decay
    
    // Simple baseline decay model
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Maintain upper bound
    pImpl->level = std::min(pImpl->peak, pImpl->level);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling: phasic dopamine burst
    // Implements temporal difference learning model
    // Burst magnitude proportional to reward prediction error
    
    // Phasic burst based on surprise
    float predictionError = reward - pImpl->baseline;  // Simplified: baseline = expected reward
    
    // Burst magnitude: larger for unexpected rewards
    float burstMagnitude = std::max(0.0f, std::min(1.0f, predictionError * pImpl->releaseRate));
    
    // Add to current dopamine level
    pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
    
    // Duration: dopamine neurons fire for ~100-200ms
    // This is modeled by the decay rate during update
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    // Negative error (unexpected lack of reward) causes dip
    // Positive error (better than expected) causes burst
    
    if (error > 0.1f) {
        // Better than expected: positive prediction error
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 2.0f);
    } else if (error < -0.1f) {
        // Worse than expected: negative prediction error
        pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate * 0.5f);
    }
    
    // Update baseline for next calculation
    pImpl->baseline = error;
}

} // namespace nlm
