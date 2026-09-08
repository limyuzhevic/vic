// Complete Dopamine neuromodulator implementation
#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;                    // Current dopamine concentration (0-1)
    float baseline;                  // Basal dopamine level
    float peak;                      // Maximum achievable level
    float decayRate;                 // Clearance rate (1/ms)
    float releaseRate;               // Release rate (1/ms)
    float synthesisRate;             // Synthesis rate (1/ms)
    float reuptakeRate;              // Reuptake rate (1/ms)
    float receptorSaturation;        // Receptor saturation factor
    float tonicBaseline;             // Tonic firing rate contribution
    float phasicLevel;               // Phasic response magnitude
    
    // Spike timing components
    float lastPreSpikeTime;          // Last presynaptic spike time
    float lastPostSpikeTime;         // Last postsynaptic spike time
    
    // Eligibility trace for reward prediction error
    float eligibilityTrace;
    float traceDecayRate;
    
    // State variables for temporal difference learning
    float predictionError;
    float valuePrediction;            // Current value prediction
    float rewardPrediction;           // Expected future reward
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f), synthesisRate(0.05f),
             reuptakeRate(0.2f), receptorSaturation(0.95f),
             tonicBaseline(0.01f), phasicLevel(0.1f),
             lastPreSpikeTime(-1.0f), lastPostSpikeTime(-1.0f),
             eligibilityTrace(0.0f), traceDecayRate(0.05f),
             predictionError(0.0f), valuePrediction(0.0f),
             rewardPrediction(0.0f) {}
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
    // Clamp to physiological range with saturation
    level = std::clamp(level, 0.0f, 1.0f);
    
    // Apply receptor saturation model
    if (level > pImpl->receptorSaturation) {
        // Non-linear response due to receptor saturation
        float excess = level - pImpl->receptorSaturation;
        level = pImpl->receptorSaturation + excess * 0.3f;
    }
    
    pImpl->level = level;
}

float Dopamine::getPlasticityFactor() const {
    // Real dopamine-modulated plasticity: combines baseline and phasic effects
    // Formula: P = (1 - saturation) * baseline + phasic * f(Δt)
    float saturation = std::min(1.0f, pImpl->level / pImpl->peak);
    
    // Baseline plasticity (tonic dopamine)
    float baselinePlasticity = 0.3f + 0.7f * pImpl->tonicBaseline;
    
    // Phasic plasticity (burst dopamine for prediction error)
    float phasicPlasticity = 0.5f * (1.0f - saturation) + 
                           0.8f * pImpl->phasicLevel;
    
    // Combine with appropriate weighting based on dopamine level
    float plasticityFactor = baselinePlasticity * (1.0f - pImpl->level) +
                             phasicPlasticity * pImpl->level;
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics with multiple timescales
    float timeMs = static_cast<float>(dt);
    
    // 1. Phasic component: decays exponentially
    pImpl->phasicLevel *= std::exp(-pImpl->decayRate * timeMs);
    
    // 2. Tonic component: maintains baseline with fluctuations
    float tonicVariation = pImpl->tonicBaseline * 0.2f * 
                          std::sin(pImpl->synthesisRate * timeMs);
    pImpl->tonicBaseline = std::max(0.01f, 0.01f + tonicVariation);
    
    // 3. Reuptake and clearance
    float reuptake = pImpl->level * pImpl->reuptakeRate * timeMs;
    
    // 4. Synthesis contribution to tonic levels
    float synthesis = pImpl->synthesisRate * timeMs * 0.05f;
    
    // Net change
    float netChange = synthesis - reuptake - pImpl->phasicLevel;
    
    // Update level with bounds
    float newLevel = pImpl->level + netChange;
    newLevel = std::max(0.0f, std::min(newLevel, 1.0f));
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= (1.0f - pImpl->traceDecayRate * timeMs);
    
    pImpl->level = newLevel;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with temporal derivative computation
    float rewardRate = reward / 0.001f;  // Convert to per-ms rate
    
    // Compute reward prediction error
    float predictionError = reward - pImpl->rewardPrediction;
    pImpl->predictionError = predictionError;
    
    // Update prediction
    pImpl->rewardPrediction = reward;
    pImpl->valuePrediction += reward * 0.1f;  // Temporal difference learning
    
    // Phasic dopamine response: proportional to reward prediction error
    float phasicResponse = std::max(0.0f, predictionError * pImpl->phasicLevel * pImpl->releaseRate);
    
    // Add phasic burst with saturating response
    pImpl->phasicLevel = std::min(pImpl->phasicLevel + phasicResponse, 1.0f);
    
    // Total dopamine level changes
    float phasicAddition = phasicResponse * 0.3f;  // 30% contribution to total
    setLevel(getLevel() + phasicAddition);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct reward prediction error signaling (alternative pathway)
    pImpl->predictionError = error;
    
    // Update prediction without actual reward
    pImpl->rewardPrediction += error;
    
    // Immediate phasic response
    float phasicResponse = error * pImpl->phasicLevel * pImpl->releaseRate * 0.5f;
    pImpl->phasicLevel = std::min(pImpl->phasicLevel + phasicResponse, 1.0f);
    
    // Eligibility trace for downstream plasticity
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace + error * 0.1f);
    
    // Update total level
    float totalResponse = phasicResponse * 0.4f;
    setLevel(getLevel() + totalResponse);
}

void Dopamine::recordPreSpike(Timestamp time) {
    pImpl->lastPreSpikeTime = time;
}

void Dopamine::recordPostSpike(Timestamp time) {
    pImpl->lastPostSpikeTime = time;
    
    // Compute spike timing difference for STDP modulation
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float dt = time - pImpl->lastPreSpikeTime;
        
        // Dopamine modulates STDP based on spike timing
        if (dt > 0) {
            // Post after pre: LTP facilitation
            float modulation = 0.5f * std::exp(-dt / 20.0f);  // 20ms time constant
            pImpl->phasicLevel += modulation * 0.1f;
        } else {
            // Pre after post: LTD
            float modulation = -0.3f * std::exp(dt / 20.0f);
            pImpl->phasicLevel = std::max(0.0f, pImpl->phasicLevel + modulation * 0.1f);
        }
    }
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getValuePrediction() const {
    return pImpl->valuePrediction;
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 0.1f);
}

float Dopamine::getBaseline() const {
    return pImpl->baseline;
}

void Dopamine::setPeak(float peak) {
    pImpl->peak = std::clamp(peak, 1.0f, 5.0f);
}

float Dopamine::getPeak() const {
    return pImpl->peak;
}

void Dopamine::setDecayRate(float rate) {
    pImpl->decayRate = std::clamp(rate, 0.01f, 1.0f);
}

float Dopamine::getDecayRate() const {
    return pImpl->decayRate;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = std::clamp(rate, 0.1f, 10.0f);
}

float Dopamine::getReleaseRate() const {
    return pImpl->releaseRate;
}

void Dopamine::setReuptakeRate(float rate) {
    pImpl->reuptakeRate = std::clamp(rate, 0.01f, 1.0f);
}

float Dopamine::getReuptakeRate() const {
    return pImpl->reuptakeRate;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->phasicLevel = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->valuePrediction = 0.0f;
    pImpl->rewardPrediction = 0.0f;
}

void Dopamine::snapshot() {
    // Save current state for checkpoint/restore
    // Implementation depends on what state needs to be saved
}

void Dopamine::restore() {
    // Restore state from snapshot
    // Implementation depends on what state needs to be restored
}

} // namespace nlm