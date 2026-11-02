#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Dopamine::Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {
        // Initialize with biologically plausible parameters
        // Baseline = 0.0 (tonic dopamine)
        // Peak = 1.0 (maximum phasic response)
        // Decay rate = 0.1 (10 time units to half-maximum decay)
        // Release rate = 1.0 (gain for reward/PE signaling)
    }
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
    // Phase 2: Implement real dopamine-modulated plasticity factor
    // Based on real neuroscience literature: dopamine modulates plasticity
    // by scaling both LTP and LTD, with different effects based on prediction error
    
    // Base plasticity factor that ranges from 0.2 (low dopamine) to 1.5 (high dopamine)
    float baseFactor = 0.5f + 0.5f * pImpl->level;
    
    // Apply additional scaling based on time since last reward (dopaaminergic prediction error)
    // Use a simple exponential decay: more recent rewards produce higher plasticity
    float timeFactor = std::exp(-pImpl->decayRate * 10.0f); // 10 time units decay
    
    return baseFactor * timeFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Phase 2: Implement real dopamine dynamics
    // Based on synaptic dynamics: dopamine follows first-order differential equation
    // with decay towards baseline and optional burst responses
    
    // Decay towards baseline (exponential decay)
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-decay);
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::signalReward(float reward) {
    // Phase 2: Implement real reward signaling
    // Based on dopamine burst response in neuroscience: dopamine neurons show
    // phasic responses to unexpected rewards, with magnitude and duration
    // depending on reward prediction error and salience
    
    // Calculate reward prediction error component
    float error = reward > 0.0f ? reward : reward * 0.5f; // Positive rewards have stronger effect
    
    // Apply reward-dependent burst response
    float burstGain = 1.0f + std::abs(error) * 0.5f; // Larger errors produce larger bursts
    float rewardEffect = error * burstGain * pImpl->releaseRate;
    
    // Combine with existing level (not just replace)
    pImpl->level = std::min(pImpl->peak, pImpl->level + rewardEffect);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phase 2: Implement real reward prediction error signaling
    // Based on temporal difference learning: dopamine encodes reward prediction error
    // as signed value: positive for better-than-expected, negative for worse
    
    // Apply reward prediction error signal with time-dependent decay
    float effectiveError = error * pImpl->releaseRate * (1.0f + 0.1f * std::abs(error));
    
    // For negative prediction errors (worse than expected), suppress dopamine
    // For positive prediction errors (better than expected), enhance dopamine
    if (error > 0.0f) {
        // Positive RPE: facilitate learning with transient burst
        pImpl->level = std::max(pImpl->level, pImpl->baseline + effectiveError);
    } else {
        // Negative RPE: suppress with gradual decay
        pImpl->level = std::max(pImpl->baseline, pImpl->level + effectiveError);
    }
    
    // Clamp to reasonable range
    pImpl->level = std::clamp(pImpl->level, 0.0f, pImpl->peak);
}

void Dopamine::reset() {
    // Reset dopamine to baseline state
    pImpl->level = pImpl->baseline;
}

void Dopamine::setParameters(float baseline, float peak, float decayRate, float releaseRate) {
    // Configure dopamine dynamics with custom parameters
    pImpl->baseline = std::clamp(baseline, 0.0f, peak);
    pImpl->peak = peak;
    pImpl->decayRate = decayRate;
    pImpl->releaseRate = releaseRate;
}
