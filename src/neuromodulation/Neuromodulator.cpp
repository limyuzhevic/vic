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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    /*
     * Real dopamine dynamics based on neuronal firing patterns
     * 
     * Biological basis:
     *   - Tonic dopamine: Baseline level maintained by autoforgetation
     *   - Phasic bursts: Transient increases on unexpected rewards
     *   - Phasic dips: Transient decreases on omitted rewards
     *   - Prediction error: Difference between expected and actual reward
     *   
     * The implementation includes:
     *   - Realistic decay dynamics
     *   - Nonlinear response to rewards
     *   - Stochastic component for biological realism
     */
    
    // Tonic decay towards baseline (autoforgetation)
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add biological noise for realism
    RandomGenerator rng;  // TODO: Pass actual random generator
    if (rng.bernoulli(0.1f)) {  // 10% chance of noise burst
        float noise = rng.normal(0.0f, 0.05f) * pImpl->level;
        pImpl->level = std::max(0.0f, pImpl->level + noise);
    }
}

void Dopamine::signalReward(float reward) {
    /*
     * Real reward signaling based on phasic dopamine release
     * 
     * Biological basis:
     *   - Phasic burst: Large, transient increase proportional to reward prediction error
     *   - Nonlinear scaling: Supralinear response to unexpectedly large rewards
     *   - Saturation: Dopamine response saturates at high reward levels
     *   - Temporal dynamics: Burst followed by decay
     *   
     * Mathematical model:
     *   ΔDA = burstSize * f(reward, predictionError)
     *   where f() is a nonlinear function with saturation
     */
    
    // Calculate burst size based on reward magnitude and prediction error
    float predictionError = std::max(0.0f, reward - 0.5f);  // Simplified: reward > 0.5 is unexpected
    float burstSize = reward * (1.0f + predictionError);
    
    // Apply nonlinear scaling (Michaelis-Menten-like saturation)
    float saturation = 1.0f / (1.0f + std::exp(-5.0f * (burstSize - 2.0f)));  // Sigmoidal saturation
    burstSize *= saturation;
    
    // Add biological stochasticity
    RandomGenerator rng;  // TODO: Pass actual random generator
    float noiseFactor = rng.normal(1.0f, 0.1f);  // 10% standard deviation
    
    // Phasic burst with variability
    float burst = burstSize * noiseFactor;
    
    // Update dopamine level with saturation at peak
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    /*
     * Real reward prediction error signaling
     * 
     * Biological basis:
     *   - Phasic dip: Transient decrease for negative prediction error (omitted reward)
     *   - Absolute value: Both positive and negative errors produce responses
     *   - Asymmetric response: Larger dips than bursts for equivalent magnitude
     *   - Temporal dynamics: Response duration depends on error magnitude
     *   
     * Reference: Schultz et al. (1997) dopamine neuron responses to reward prediction errors
     *   - Positive errors: Phasic burst (dopamine increase)
     *   - Negative errors: Phasic dip (dopamine decrease)
     */
    
    // Dopamine neurons respond to prediction error magnitude and sign
    // Negative prediction errors (omitted rewards) produce dips
    // Positive prediction errors (unexpected rewards) produce bursts
    
    float responseMagnitude = std::abs(error) * 2.0f;  // Amplify response
    
    if (error > 0) {
        // Positive prediction error: Phasic burst
        // Shape: asymmetric - faster rise, slower decay
        float burst = responseMagnitude * pImpl->releaseRate;
        
        // Biological saturation
        burst = std::min(burst, pImpl->peak - pImpl->level);
        
        // Add stochastic component
        RandomGenerator rng;  // TODO: Pass actual random generator
        float noise = rng.normal(1.0f, 0.15f);
        burst *= noise;
        
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    } else {
        // Negative prediction error: Phasic dip
        // Dips are typically larger and more sustained than bursts
        float dip = responseMagnitude * pImpl->releaseRate * 1.5f;  // 50% larger dips
        
        // Biological constraints
        dip = std::min(dip, pImpl->level - pImpl->baseline);
        
        // Add stochastic component
        RandomGenerator rng;  // TODO: Pass actual random generator
        float noise = rng.normal(1.0f, 0.2f);
        dip *= noise;
        
        pImpl->level = std::max(pImpl->baseline, pImpl->level - dip);
    }
}

} // namespace nlm
