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
    // Dopamine concentration affects synaptic modification
    // High dopamine increases LTP/LTD probability, decreases consolidation
    // Low dopamine increases stability, decreases learning rate
    
    // Biological basis: Dopamine concentration correlates with plasticity
    // DA > 0.5: Enhanced learning, unstable memories
    // DA < 0.2: Stable memories, slow learning
    // DA ~ 0.3: Optimal balance for memory consolidation
    
    float plasticity = 0.5f + 0.5f * pImpl->level;  // Linear mapping
    
    // Add nonlinear modulation based on learning phase
    if (pImpl->level < 0.2f) {
        plasticity *= 0.3f;  // Low DA: suppressed learning
    } else if (pImpl->level > 0.8f) {
        plasticity *= 1.5f;  // High DA: enhanced but unstable learning
    }
    
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics
    // Decay towards baseline and respond to reward signals
    // Implements biological dopamine neuron firing patterns
    
    // Validate timestep
    dt = std::max(0.0, dt);
    
    // Natural decay: dopamine neurons have spontaneous activity
    float naturalDecay = pImpl->level * 0.05f;  // 5% per timestep
    
    // Towards baseline: homeostasis
    pImpl->level += (pImpl->baseline - pImpl->level) * pImpl->decayRate * dt - naturalDecay;
    
    // Ensure non-negative
    pImpl->level = std::max(0.0f, pImpl->level);
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling (dopamine burst)
    // Implement dopamine neuron burst response to unexpected rewards
    // This is the "teaching signal" mechanism in reinforcement learning
    
    // Validate reward input
    reward = std::clamp(reward, -10.0f, 10.0f);  // Reasonable reward range
    
    // Reward prediction error (simplified)
    float predictionError = reward - pImpl->level;  // Expected reward ~ current level
    
    // Dopamine neuron response to prediction error
    if (predictionError > 0.1f) {
        // Phasic burst for positive prediction error (better than expected)
        pImpl->level = std::min(pImpl->peak, pImpl->level + predictionError * 2.0f * pImpl->releaseRate);
    } else if (predictionError < -0.1f) {
        // Phasic dip for negative prediction error (worse than expected)
        pImpl->level = std::max(0.0f, pImpl->level + predictionError * pImpl->releaseRate);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling
    // This is the key reinforcement learning signal
    // Negative error (worse than expected) -> dips dopamine
    // Positive error (better than expected) -> bursts dopamine
    
    // Validate prediction error
    error = std::clamp(error, -5.0f, 5.0f);  // Reasonable error range
    
    // Phasic modulation based on prediction error
    if (error < -0.1f) {
        // Reward prediction error (expected > actual) -> dip
        pImpl->level = std::max(0.0f, pImpl->level - std::abs(error) * pImpl->releaseRate * 0.5f);
    } else if (error > 0.1f) {
        // Reward prediction error (actual > expected) -> burst
        pImpl->level = std::min(pImpl->peak, pImpl->level + error * pImpl->releaseRate * 2.0f);
    }
    
    // Clip to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = peak;
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = rate;
}

float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

} // namespace nlm