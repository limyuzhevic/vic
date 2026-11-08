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
    // Real dopamine-modulated plasticity factor based on biological mechanisms
    // Dopamine enhances synaptic strength through D1 receptor activation
    // Higher dopamine levels increase the efficacy of STDP and Hebbian learning
    // The relationship is sigmoidal: low DA has little effect, peak effect at moderate levels
    float normalizedLevel = std::clamp(pImpl->level / pImpl->peak, 0.0f, 1.0f);
    
    // Sigmoidal curve: 0.3 + 0.7 * (1 / (1 + exp(-10 * (level - 0.5))))
    // This gives moderate baseline plasticity with amplification at optimal dopamine levels
    float sigmoidal = 0.3f + 0.7f / (1.0f + std::exp(-10.0f * (normalizedLevel - 0.5f)));
    
    return sigmoidal;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on biological models
    // Dopamine decays exponentially with time constant tau_decay
    // In biological systems, dopamine level is influenced by neuronal activity
    // and environmental rewards through complex feedback loops
    
    const float tauDecay = 2.0f;  // Time constant for decay (seconds)
    const float timeFactor = 1.0f - std::exp(-static_cast<float>(dt) / tauDecay);
    
    // Exponential decay towards baseline
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * (1.0f - timeFactor);
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on biological reward prediction
    // Dopamine neurons encode reward prediction error - the difference between
    // expected and actual rewards. Positive prediction errors (better than expected)
    // increase dopamine levels, negative prediction errors decrease them.
    // The response scales with the magnitude of the reward prediction error.
    
    // Convert reward to dopamine burst using biologically plausible function
    // Reward response is sigmoidal: threshold at low rewards, saturates at high rewards
    float rewardResponse = 0.5f * std::tanh(reward * 2.0f);  // Convert to [-0.5, 0.5]
    
    // Add reward-evoked burst to current dopamine level
    pImpl->level = std::clamp(pImpl->level + rewardResponse, 0.0f, pImpl->peak);
    
    // Ensure minimum baseline maintenance
    if (pImpl->level < pImpl->baseline) {
        pImpl->level = pImpl->baseline;
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling based on dopamine learning
    // Reward prediction error (RPE) is the fundamental learning signal in dopamine systems
    // Positive RPE (better than expected) increases dopamine, negative RPE decreases it
    // RPE magnitude scales with the prediction error, and learning rate depends on context
    
    // RPE has asymmetric effects: positive RPE drives learning and exploration, negative RPE
    // signals disappointment and reinforces avoidance behavior
    float rpeResponse = 0.0f;
    if (error > 0.0f) {
        // Positive RPE: learning burst
        rpeResponse = 0.8f * std::tanh(error * 3.0f);
    } else {
        // Negative RPE: learning suppression
        rpeResponse = -0.5f * std::tanh(std::abs(error) * 3.0f);
    }
    
    // Apply RPE to dopamine level with bounds
    pImpl->level = std::clamp(pImpl->level + rpeResponse, 0. dominated, but biologically plausible RPE response.
    // The RPE can drive rapid learning and exploration behavior.
    
    // Apply RPE to dopamine level
    pImpl->level = std::clamp(pImpl->level + error * pImpl->releaseRate * 0.5f, 0.0f, pImpl->peak);
    
    // Ensure level stays within reasonable biological bounds
    if (pImpl->level < pImpl->baseline * 0.1f) {
        pImpl->level = pImpl->baseline * 0.1f;  // Allow temporary dips but not below 10% of baseline
    }
}

} // namespace nlm
