#include "Neuromodulator.hpp"
#include <algorithm>
#include "../core/Random/Random.hpp"

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
    // Dopamine has complex effects on plasticity:
    // - D1 receptor activation promotes LTP (learning)
    // - D2 receptor activation promotes LTD (forgetting)
    // - Balance between D1 and D2 determines learning rate
    // - Dopamine surge signals prediction error for reinforcement learning
    
    float level = pImpl->level;
    
    // Base plasticity factor increases with dopamine
    float plasticity = 0.5f + 0.5f * level;
    
    // Prediction error modulation (reward prediction error)
    if (level > 0.7f) {
        // Strong dopamine burst signals positive prediction error
        // Enhances learning of predicted actions
        plasticity = std::min(1.0f, plasticity * 1.5f);
    } else if (level < 0.3f) {
        // Low dopamine signals negative prediction error or lack of reward
        // Reduces learning rate, promotes forgetting
        plasticity = std::max(0.1f, plasticity * 0.6f);
    }
    
    return plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    // Implement real dopamine dynamics
    // Dopamine dynamics involve:
    // - Tonic release (baseline firing)
    // - Phasic release (reward/prediction error signals)
    // - Reuptake and enzymatic degradation
    // - Receptor activation and signaling cascades
    
    float dtFloat = static_cast<float>(dt);
    
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Add small spontaneous fluctuations (tonal variability)
    pImpl->level += (uniformReal(0.0f, 1.0f) - 0.5f) * 0.01f * dtFloat;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Implement real reward signaling
    // Dopamine encodes reward prediction errors:
    // - Higher than expected reward = positive prediction error
    // - Lower than expected reward = negative prediction error
    // - Reward magnitude affects dopamine burst amplitude
    
    // Compute reward prediction error
    // For now, assume expected reward is baseline
    float predictionError = reward - pImpl->baseline;
    
    // Signal reward with burst response
    // Stronger rewards produce larger bursts
    float burst = std::min(1.0f, predictionError * pImpl->releaseRate);
    
    // Apply dopamine burst with saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Track peak reward for context
    pImpl->peak = std::max(pImpl->peak, reward);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Implement reward prediction error signaling
    // Core of reinforcement learning: compute prediction error
    // which drives learning and updates value predictions
    
    // Dopamine neurons encode reward prediction error:
    // - Positive error (unexpected reward) -> increase firing
    // - Negative error (omitted reward) -> decrease firing
    // - Larger errors produce larger responses
    
    // Apply prediction error to dopamine level
    // Error can be positive (better than expected) or negative (worse than expected)
    float delta = error * pImpl->releaseRate;
    
    // For positive errors (better than expected), increase dopamine
    if (delta > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + delta);
    } else {
        // For negative errors, decrease dopamine (but don't go below baseline)
        pImpl->level = std::max(pImpl->baseline, pImpl->level + delta);
    }
}

} // namespace nlm
