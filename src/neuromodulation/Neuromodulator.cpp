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
    // Real dopamine-modulated plasticity factor implementation
    // Based on computational neuroscience models:
    // - Low dopamine: Reduces learning ( stabilizes behavior )
    // - Medium dopamine: Optimal learning rate  
    // - High dopamine: Increases learning ( exploratory behavior )
    // 
    // Sigmoidal response curve typical of dopamine systems
    float activation = pImpl->level * 4.0f - 2.0f;  // Scale to [-2, 2]
    float plasticity = 1.0f / (1.0f + std::exp(-activation)) * 2.0f - 0.5f;
    
    // Dopamine modulates both LTP and LTD rates multiplicatively
    return 0.5f + plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics implementation
    // Based on differential equation: dDA/dt = -k * (DA - DA_baseline) + input
    // 
    // Two-component model:
    // 1. Tonic dopamine (baseline level)
    // 2. Phasic bursts on unexpected rewards/prediction errors
    
    float dtFloat = static_cast<float>(dt);
    
    // Apply decay towards baseline (tonic component)
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add small noise for biological realism
    // pImpl->level += (rng() - 0.5f) * 0.01f * dtFloat;  // Would need random access
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling implementation
    // Based on temporal difference learning model (Schultz et al., 1997)
    // 
    // Phasic dopamine burst proportional to reward prediction error:
    // ΔDA = α * (R - V), where R = received reward, V = predicted value
    
    // Reward prediction error calculation
    float rewardPredictionError = reward - pImpl->baseline;  // Simplified
    
    // Phasic response with bounded amplitude
    float phasicIncrease = rewardPredictionError * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + phasicIncrease);
    
    // Cap at biologically plausible maximum
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling implementation
    // Based on predictive coding theory: dopamine encodes the difference
    // between expected and actual outcomes
    
    // Positive error (better than expected): LTP-like potentiation
    // Negative error (worse than expected): LTD-like depression
    
    // Apply prediction error with appropriate sign
    pImpl->level = std::max(pImpl->baseline, pImpl->level + error * pImpl->releaseRate);
    
    // Add small stabilization term to prevent runaway
    pImpl->level = pImpl->level * 0.99f + pImpl->baseline * 0.01f;
}

} // namespace nlm
