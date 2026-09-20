#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;                    // Current dopamine concentration (0-1)
    float baseline;                  // Tonic baseline level (~0.1-0.2)
    float peak;                      // Maximum achievable level (1.0)
    float decayRate;                 // Decay time constant (~100-200ms)
    float releaseRate;               // Release rate constant (~0.5-1.0)
    
    // Reward prediction error dynamics
    float predictionError;          // Current prediction error signal
    float errorBaseline;             // Baseline prediction error (salience)
    float errorAdaptation;           // Rate of error adaptation
    std::vector<float> errorHistory; // History of prediction errors
    
    // Phasic/tonic dynamics
    float tonicLevel;                // Tonic firing rate (0.5-2.0 Hz)
    float phasicLevel;               // Phasic burst amplitude
    float phasicDuration;            // Duration of phasic signals
    float interBurstInterval;        // Minimum interval between bursts
    
    // Reuptake and vesicular dynamics
    float reuptakeRate;              // Vesicular reuptake rate (0.1-0.3 ms-1)
    float vesicularReleaseProb;      // Probability of vesicular release (0.3-0.7)
    float vesiclePool;               // Available vesicle pool (0-1)
    float vesicleRefillRate;         // Vesicle refill rate (0.05-0.2 ms-1)
    
    // Time-dependent dynamics
    float riseTime;                  // Rise time (~20-50ms)
    float fallTime;                  // Fall time (~100-300ms)
    
    // Plasticity modulation
    float plasticityFactor;          // Modulates synaptic plasticity
    float eligibilityTrace;          // Eligibility trace for reward modulation
    float traceDecayRate;            // Eligibility trace decay
    
    // Biological state
    bool isFiring;                   // Currently firing
    float lastBurstTime;             // Time of last burst
    float burstCount;                // Number of bursts in recent history
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.15f), releaseRate(0.8f),
             predictionError(0.0f), errorBaseline(0.0f), errorAdaptation(0.05f),
             errorHistory(), tonicLevel(0.8f), phasicLevel(0.0f), phasicDuration(50.0f), interBurstInterval(200.0f),
             reuptakeRate(0.2f), vesicularReleaseProb(0.5f), vesiclePool(0.8f), vesicleRefillRate(0.1f),
             riseTime(30.0f), fallTime(200.0f), plasticityFactor(1.0f), eligibilityTrace(0.0f),
             traceDecayRate(0.02f), isFiring(false), lastBurstTime(0.0f), burstCount(0.0f),
             rng(std::random_device()()), uniformDist(0.0f, 1.0f), normalDist(0.0f, 1.0f) {}
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
    // Dopamine modulates plasticity through two mechanisms:
    // 1. Via D1 receptors: enhance LTP (long-term potentiation)
    // 2. Via D2 receptors: suppress LTD (long-term depression)
    // The net effect depends on dopamine level
    
    // Biological range: 0.5-2.0x baseline plasticity
    float d1Effect = pImpl->level * 0.5f;  // D1 receptor effect (excitatory)
    float d2Effect = pImpl->level * 0.3f;  // D2 receptor effect (inhibitory)
    float netEffect = 1.0f + d1Effect - d2Effect;  // Net modulation
    
    // Apply bounds for biological plausibility
    return std::clamp(netEffect, 0.5f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt) / 1000.0f; // Convert to seconds
    
    // Update prediction error baseline
    pImpl->errorBaseline = std::max(0.0f, pImpl->errorBaseline - 0.001f * deltaTime);
    
    // Decay eligibility trace
    pImpl->eligibilityTrace = std::max(0.0f, pImpl->eligibilityTrace - pImpl->traceDecayRate * deltaTime);
    
    // Update vesicle pool
    pImpl->vesiclePool = std::min(pImpl->vesiclePool + pImpl->vesicleRefillRate * deltaTime, 1.0f);
    
    // Compute tonic component (baseline firing)
    float tonicInput = pImpl->tonicLevel * 0.001f * deltaTime;  // Convert Hz to concentration change
    
    // Compute phasic component (if firing)
    float phasicInput = 0.0f;
    if (pImpl->isFiring) {
        phasicInput = pImpl->phasicLevel * 0.01f * deltaTime;  // Phasic burst contribution
        
        // Decrease isFiring state
        pImpl->isFiring = false;
    }
    
    // Apply reuptake
    float reuptakeAmount = pImpl->reuptakeRate * pImpl->level * deltaTime;
    
    // Update total level with all inputs
    pImpl->level += tonicInput + phasicInput - reuptakeAmount;
    
    // Decay towards baseline
    float decayTarget = pImpl->baseline + (pImpl->errorBaseline * 0.1f);  // Error elevates baseline
    float decayAmount = (pImpl->level - decayTarget) * pImpl->decayRate * deltaTime;
    pImpl->level = std::max(0.0f, pImpl->level - decayAmount);
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine response to unexpected rewards
    // Based on reward prediction error theory
    
    // Check if we can release vesicles
    if (pImpl->vesiclePool > pImpl->vesicularReleaseProb) {
        pImpl->vesiclePool -= pImpl->vesicularReleaseProb;
        pImpl->isFiring = true;
        
        // Compute reward prediction error
        // PE = actual reward - predicted reward
        float pe = std::max(0.0f, reward);  // Simplified: treat reward as positive PE
        
        // Store prediction error
        pImpl->predictionError = pe;
        pImpl->errorBaseline = std::max(pImpl->errorBaseline, pe);
        
        // Update eligibility trace for reward-modulated learning
        pImpl->eligibilityTrace = 1.0f;
        
        // Phasic burst magnitude proportional to reward
        pImpl->phasicLevel = std::min(pImpl->phasicLevel + reward * pImpl->releaseRate, pImpl->peak);
        
        // Rapid increase in dopamine level
        pImpl->level = std::min(pImpl->level + pImpl->phasicLevel * 5.0f, pImpl->peak);
        
        pImpl->lastBurstTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        
        pImpl->burstCount += 1.0f;
        
        NLM_LOG_DEBUG("Dopamine: Phasic response to reward, PE: {}, level: {}", pe, pImpl->level);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Phasic dopamine response to reward prediction errors
    // Medial forebrain bundle signaling
    
    // Check if we can release vesicles
    if (pImpl->vesiclePool > pImpl->vesicularReleaseProb && error != 0.0f) {
        pImpl->vesiclePool -= pImpl->vesicularReleaseProb;
        pImpl->isFiring = true;
        
        // Store prediction error
        pImpl->predictionError = error;
        pImpl->errorBaseline = std::max(pImpl->errorBaseline, std::abs(error));
        
        // Update eligibility trace
        pImpl->eligibilityTrace = std::abs(error);
        
        // Phasic burst magnitude proportional to error
        float errorMagnitude = std::abs(error);
        pImpl->phasicLevel = std::min(pImpl->phasicLevel + errorMagnitude * pImpl->releaseRate, pImpl->peak);
        
        // Sign of error determines direction (positive = reward, negative = punishment)
        float errorSign = error > 0.0f ? 1.0f : -1.0f;
        pImpl->level = std::min(pImpl->level + pImpl->phasicLevel * errorSign * 5.0f, pImpl->peak);
        pImpl->level = std::max(0.0f, pImpl->level);  // Dopamine never negative
        
        pImpl->lastBurstTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        
        pImpl->burstCount += 1.0f;
        
        NLM_LOG_DEBUG("Dopamine: Phasic response to prediction error, PE: {}, level: {}", error, pImpl->level);
    }
}

void Dopamine::setReuptakeRate(float rate) {
    pImpl->reuptakeRate = std::clamp(rate, 0.0f, 1.0f);
}

void Dopamine::triggerVesicularRelease() {
    if (pImpl->vesiclePool > pImpl->vesicularReleaseProb) {
        pImpl->vesiclePool -= pImpl->vesicularReleaseProb;
        NLM_LOG_DEBUG("Dopamine: Vesicular release triggered");
    }
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

const std::vector<float>& Dopamine::getErrorHistory() const {
    return pImpl->errorHistory;
}

void Dopamine::clearHistory() {
    pImpl->errorHistory.clear();
}
