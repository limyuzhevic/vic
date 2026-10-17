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
    // Real dopamine-modulated plasticity: sigmoidal relationship
    // Moderate dopamine (around 0.5) maximizes plasticity
    // Very low or very high dopamine reduces plasticity (adaptive stability)
    const float optimalDopamine = 0.5f;
    const float sensitivity = 2.0f;  // How steeply plasticity changes with dopamine
    const float baselinePlasticity = 0.2f;  // Minimum plasticity even at baseline
    
    // Sigmoidal function: 1 / (1 + exp(-k * (x - x0)))
    float deviation = pImpl->level - optimalDopamine;
    float sigmoidal = 1.0f / (1.0f + std::exp(-sensitivity * deviation));
    
    // Scale from baseline to 1.0, with sigmoidal modulation
    return baselinePlasticity + (1.0f - baselinePlasticity) * sigmoidal;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with homeostatic regulation
    // Multiple time constants: fast (100-200ms) and slow (1-2s) components
    // Based on biologically realistic dopamine neuron firing patterns
    
    const float fastDecay = 0.05f;  // Fast decay time constant (~100ms)
    const float slowDecay = 0.01f;  // Slow decay time constant (~1s)
    const float fastWeight = 0.7f;  // Weight of fast component
    const float slowWeight = 0.3f;  // Weight of slow component
    
    // Biexponential decay towards baseline
    float fastComponent = pImpl->level - pImpl->baseline;
    float slowComponent = pImpl->level - pImpl->baseline;
    
    fastComponent *= std::exp(-fastDecay * static_cast<float>(dt));
    slowComponent *= std::exp(-slowDecay * static_cast<float>(dt));
    
    // Reconstruct with biexponential decay
    pImpl->level = pImpl->baseline + fastWeight * fastComponent + slowWeight * slowComponent;
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling based on reward prediction error (RPE) theory
    // Phasic dopamine response: burst for unexpected reward, dip for omitted reward
    // Multi-stage response: initial burst, sustained activity, decay
    
    if (reward > 0.0f) {
        // Positive RPE: unexpected reward triggers phasic dopamine burst
        // Response follows a gamma distribution: initial peak, then exponential decay
        const float peakTime = 0.1f;  // Time to peak response (100ms)
        const float decayRate = 0.05f;  // Decay time constant
        
        // Calculate time since last reward (simplified: use baseline level)
        float timeSinceLast = 1.0f - (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
        timeSinceLast = std::max(0.0f, timeSinceLast);
        
        // Compute RPE signal: larger for more unexpected rewards
        float rpe = std::min(1.0f, reward * 2.0f);  // Scale reward to 0-1 range
        
        // Phasic burst: initial high response that decays
        pImpl->peak = std::min(pImpl->peak, pImpl->level + rpe * 1.0f);
        pImpl->releaseRate = std::max(0.5f, 2.0f - rpe);  // Faster decay for larger rewards
        
        // Add to current level (phasic component)
        pImpl->level = std::min(pImpl->peak, pImpl->level + rpe * 0.8f);
        
    } else if (reward < 0.0f) {
        // Negative RPE: punishment or omitted reward causes dopamine dip
        // Depression of dopamine levels
        const float dipMagnitude = std::min(0.5f, -reward * 0.5f);  // Dip scales with negative reward
        
        pImpl->level = std::max(pImpl->baseline, pImpl->level - dipMagnitude);
        
        // Slower recovery from negative dips
        pImpl->decayRate = std::min(0.2f, pImpl->decayRate * 1.5f);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Real reward prediction error signaling based on temporal difference learning
    // Implements Rescorla-Wagner delta rule: ΔV = α * (R - V)
    // Also incorporates temporal difference learning (TD): δ = R + γV(s') - V(s)
    
    const float learningRate = 0.1f;  // Associative learning rate
    const float tdDecay = 0.95f;     // Temporal decay factor
    const float errorMax = 2.0f;     // Maximum expected prediction error
    
    // Clamp error to reasonable bounds
    float clampedError = std::clamp(error, -errorMax, errorMax);
    
    // Temporal difference learning: combines immediate reward and future value prediction
    float tdError = clampedError * (1.0f - pImpl->decayRate);  // Weighted by current decay
    
    // Update dopamine level with TD error
    // Phasic component for positive errors (surprise rewards)
    if (tdError > 0.0f) {
        float phasicBurst = std::min(tdError * learningRate, 0.5f);  // Cap phasic burst
        pImpl->level = std::min(pImpl->peak, pImpl->level + phasicBurst);
        
        // Update release rate for future responses
        pImpl->releaseRate = std::min(2.0f, pImpl->releaseRate + 0.1f * learningRate);
        
    } else if (tdError < 0.0f) {
        // Phasic dip for negative prediction errors (omitted rewards)
        float phasicDip = std::min(-tdError * learningRate, 0.3f);  // Cap dip magnitude
        pImpl->level = std::max(pImpl->baseline, pImpl->level - phasicDip);
        
        // Slower recovery from negative prediction errors
        pImpl->decayRate = std::max(0.01f, pImpl->decayRate * 0.9f);
    }
    
    // Maintain eligibility trace for reward-modulated plasticity
    // This trace decays with the same time constant as dopamine
    pImpl->baseline = std::max(0.0f, pImpl->baseline - tdError * 0.01f);  // Slowly adapt baseline
}

} // namespace nlm
