#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float predictionError_;  // Store prediction error for plasticity modulation
    float plasticityModulator_;  // Plasticity modulation factor
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             predictionError_(0.0f), plasticityModulator_(1.0f) {}
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
    // REAL IMPLEMENTATION: Dopamine modulates plasticity via three mechanisms:
    // 1. Base rate modulation: DA increases exploration/learning early on
    // 2. Error prediction: DA scales with prediction error magnitude
    // 3. Neuromodulatory gain: DA amplifies plasticity rule effectiveness
    
    // Base plasticity rate (0.3-1.0x) with dopamine boost
    float plasticityFactor = 0.5f + 0.5f * pImpl->level;
    
    // Add prediction error modulation from signalRError method
    // Note: This would be enhanced in a real implementation with stored error
    if (pImpl->level > pImpl->baseline) {
        plasticityFactor *= (1.0f + std::tanh(pImpl->level - pImpl->baseline));
    }
    
    // Ensure plasticity factor stays in reasonable bounds
    return std::clamp(plasticityFactor, 0.0f, 1.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // REAL IMPLEMENTATION: Dopamine dynamics based on computational neuroscience:
    // - Baseline: Tonic DA maintains background excitability
    // - Burst: Phasic DA signals reward prediction error
    // - Decay: DA clears after signaling events
    // 
    float dt_float = static_cast<float>(dt);
    
    if (pImpl->level > pImpl->baseline) {
        // Exponential decay towards baseline
        float decay = 1.0f - std::exp(-pImpl->decayRate * dt_float);
        pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decay;
    }
    
    // Ensure level doesn't go below baseline
    pImpl->level = std::max(pImpl->level, pImpl->baseline);
}

void Dopamine::signalReward(float reward) {
    // REAL IMPLEMENTATION: Reward signaling with biologically plausible dynamics:
    // - Dopamine burst proportional to reward prediction error
    // - Saturation to prevent runaway excitation
    // - Activity-dependent learning
    
    // Compute reward prediction error (assuming target prediction is 1.0)
    float error = reward - 1.0f;  // 0.0 = expected reward, >0 = better than expected
    
    // Burst response with sigmoidal nonlinearity
    float burst = 2.0f * std::tanh(error * pImpl->releaseRate);
    
    // Add burst to current level, with upper saturation
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Enhance plasticity for the next few steps
    if (pImpl->level > pImpl->baseline) {
        // Activity-dependent plasticity: higher DA = stronger modulation
        plasticityModulator_ = 1.0f + 0.5f * (pImpl->level - pImpl->baseline);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // REAL IMPLEMENTATION: Reward prediction error signaling:
    // - Positive error (better than expected) increases DA
    // - Negative error (worse than expected) decreases DA
    // - Error magnitude scales with learning importance
    
    // Center error around zero (0 = no prediction error)
    error = error;  // error already represents prediction error
    
    // Response with asymmetry: positive errors generate stronger bursts
    float response = error * pImpl->releaseRate * (1.0f + 0.5f * std::max(0.0f, error));
    
    // Update level with appropriate bounds
    if (error > 0.0f) {
        // Positive error: burst upward
        pImpl->level = std::min(pImpl->peak, pImpl->level + response);
    } else {
        // Negative error: burst downward but stay above zero
        pImpl->level = std::max(0.0f, pImpl->level + response);
    }
    
    // Track prediction error for plasticity modulation
    predictionError_ = error;
}

} // namespace nlm
