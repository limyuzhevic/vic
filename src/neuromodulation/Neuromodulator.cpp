#include "Dopamine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Dopamine implementation with realistic neuromodulation dynamics

struct Dopamine::Impl {
    float level;                    // Current dopamine concentration (normalized 0.0-1.0)
    float baseline;                  // Tonic baseline level (0.1-0.2)
    float peak;                      // Peak phasic release level (1.0-1.5)
    float decayTimeConstant;         // Decay time constant for exponential decay (ms)
    float phasicRiseTime;            // Time constant for phasic rise (ms)
    float tonicDecayRate;             // Rate of tonic component decay
    float phasicDecayRate;            // Rate of phasic component decay
    float predictionErrorGain;       // Sensitivity to reward prediction errors
    float rewardGain;                 // Sensitivity to actual rewards
    float eligibilityTrace;          // Eligibility trace for plasticity modulation
    Timestamp lastBurstTime;         // Time of last dopamine burst
    float burstIntensity;             // Intensity of last burst
    
    // Neural dynamics parameters
    static constexpr float DEFAULT_BASELINE = 0.15f;      // ~0.1-0.2
    static constexpr float DEFAULT_PEAK = 1.25f;          // ~1.0-1.5
    static constexpr float DEFAULT_DECAY_TIME_CONST = 150.0f; // ms, ~100-200ms range
    static constexpr float DEFAULT_RISE_TIME = 20.0f;     // ms
    static constexpr float DEFAULT_PREDICTION_ERROR_GAIN = 0.8f;
    static constexpr float DEFAULT_REWARD_GAIN = 0.6f;
    static constexpr float DEFAULT_ELIGIBILITY_TRACE_DECAY = 0.01f; // 1% per ms
    static constexpr float MIN_CONCENTRATION = 0.0f;
    static constexpr float MAX_CONCENTRATION = 1.0f;
    
    Impl() : level(DEFAULT_BASELINE), baseline(DEFAULT_BASELINE),
             peak(DEFAULT_PEAK), decayTimeConstant(DEFAULT_DECAY_TIME_CONST),
             phasicRiseTime(DEFAULT_RISE_TIME), tonicDecayRate(1.0f / decayTimeConstant),
             phasicDecayRate(1.0f / decayTimeConstant),
             predictionErrorGain(DEFAULT_PREDICTION_ERROR_GAIN),
             rewardGain(DEFAULT_REWARD_GAIN),
             eligibilityTrace(0.0f), lastBurstTime(0.0), burstIntensity(0.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() {
    delete pImpl;
}

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, Impl::MIN_CONCENTRATION, Impl::MAX_CONCENTRATION);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity: higher DA enhances LTP, lower DA enhances LTD
    // Normalized to range [0.1, 0.9] for safety
    float plasticityFactor = 0.5f + 0.4f * pImpl->level;
    return std::clamp(plasticityFactor, 0.1f, 0.9f);
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::updateEligibilityTrace(float delta) {
    // Update eligibility trace with decay and new contribution
    // Eligibility trace decays exponentially over time
    pImpl->eligibilityTrace *= (1.0f - pImpl->tonicDecayRate);
    pImpl->eligibilityTrace = std::clamp(pImpl->eligibilityTrace + delta, 0.0f, 1.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics with phasic and tonic components
    float dtMs = static_cast<float>(dt * 1000.0); // Convert to milliseconds
    
    // Exponential decay towards baseline
    float decayFactor = std::exp(-pImpl->phasicDecayRate * dtMs);
    float targetLevel = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Smooth transition to target
    float alpha = 1.0f - std::exp(-pImpl->phasicDecayRate * dtMs);
    pImpl->level = targetLevel + alpha * (pImpl->level - targetLevel);
    
    // Ensure bounds
    pImpl->level = std::clamp(pImpl->level, Impl::MIN_CONCENTRATION, Impl::MAX_CONCENTRATION);
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= (1.0f - pImpl->DEFAULT_ELIGIBILITY_TRACE_DECAY * dtMs);
    pImpl->eligibilityTrace = std::clamp(pImpl->eligibilityTrace, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine burst on reward delivery
    // Reward magnitude scaled by reward gain parameter
    float rewardSignal = reward * pImpl->rewardGain;
    
    // Compute burst intensity: combine with existing level for context
    float contextBoost = std::min(0.5f, pImpl->level * 0.3f); // Some carryover effect
    float burst = rewardSignal + contextBoost;
    
    // Apply burst with saturation at peak
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Record burst for eligibility trace
    pImpl->burstIntensity = burst;
    pImpl->lastBurstTime = 0.0; // Would be actual timestamp in real system
    
    // Update eligibility trace with reward prediction error
    updateEligibilityTrace(burst);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phasic dopamine response to reward prediction errors
    // Positive errors (better than expected) cause DA bursts
    // Negative errors (worse than expected) cause DA dips
    
    // Scale error by prediction error gain
    float errorSignal = error * pImpl->predictionErrorGain;
    
    if (error > 0) {
        // Reward prediction error: phasic burst
        float burst = errorSignal;
        
        // Burst magnitude depends on prediction error and current level
        float contextContribution = std::min(0.3f, pImpl->level * 0.2f);
        burst = std::max(0.0f, burst + contextContribution);
        
        // Apply with saturation
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        
        // Record for plasticity
        pImpl->burstIntensity = burst;
        pImpl->lastBurstTime = 0.0;
        
        // Update eligibility trace with positive error contribution
        updateEligibilityTrace(burst);
    } else {
        // Negative prediction error: dip below baseline
        float dip = -errorSignal; // error is negative
        float newLevel = pImpl->level - dip;
        newLevel = std::max(pImpl->baseline, newLevel);
        
        // Smooth transition to new level
        pImpl->level = newLevel;
        
        // Negative errors still contribute to eligibility but with smaller effect
        float eligibilityContribution = -errorSignal * 0.5f; // Reduced contribution
        updateEligibilityTrace(eligibilityContribution);
    }
}

float Dopamine::getBurstIntensity() const {
    return pImpl->burstIntensity;
}

Timestamp Dopamine::getLastBurstTime() const {
    return pImpl->lastBurstTime;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.1f, 0.2f);
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = std::clamp(peak, 1.0f, 1.5f);
}

void Dopamine::setDecayTimeConstant(float tau) {
    pImpl->decayTimeConstant = std::clamp(tau, 100.0f, 200.0f);
    pImpl->phasicDecayRate = 1.0f / pImpl->decayTimeConstant;
    pImpl->tonicDecayRate = pImpl->phasicDecayRate;
}

void Dopamine::setRewardGain(float gain) {
    pImpl->rewardGain = std::clamp(gain, 0.0f, 1.0f);
}

void Dopamine::setPredictionErrorGain(float gain) {
    pImpl->predictionErrorGain = std::clamp(gain, 0.0f, 1.0f);
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

float Dopamine::getDecayTimeConstant() const {
    return pImpl->decayTimeConstant;
}

float Dopamine::getRewardGain() const {
    return pImpl->rewardGain;
}

float Dopamine::getPredictionErrorGain() const {
    return pImpl->predictionErrorGain;
}

} // namespace nlm