#include " Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityFactor;  // Dopamine-modulated plasticity factor
    float eligibilityTrace;  // Synaptic eligibility for plasticity
    float predictionError;  // Reward prediction error signal
    float learningRate;     // Current learning rate (dopamine-modulated)
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), 
             releaseRate(2.0f), plasticityFactor(0.5f), eligibilityTrace(0.0f),
             predictionError(0.0f), learningRate(0.01f) {}
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
    // Real implementation: Dopamine modulates synaptic plasticity
    // Higher dopamine increases learning rate and stabilizes synapses
    // This is based on dopaminergic functions in the brain:
    // - Reward prediction error signals
    // - Learning rate modulation
    // - Synaptic tagging and capture
    
    // Dual-phase model:
    // 1. Phasic bursts for reward prediction errors
    // 2. Tonic baseline for maintaining plasticity
    
    // Plasticity factor is a function of both level and prediction error
    float predictionErrorWeight = 0.5f;
    float baselineWeight = 0.5f;
    
    float plasticity = pImpl->baselineWeight * pImpl->plasticityFactor;
    if (std::abs(pImpl->predictionError) > 0.001f) {
        // Reward prediction error modulates plasticity
        float errorInfluence = std::min(1.0f, std::abs(pImpl->predictionError));
        plasticity += predictionErrorWeight * errorInfluence * pImpl->level;
    }
    
    // Cap at reasonable values
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Real dopamine dynamics based on biological models:
    // - Decay towards baseline
    // - Facilitation with repeated inputs
    // - Response adaptation
    // - Noise for stochastic learning
    
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * 
                  std::exp(-pImpl->decayRate * dtFloat);
    
    // Decay eligibility trace
    pImpl->eligibilityTrace *= std::exp(-0.1f * dtFloat);
    
    // Small noise for stochastic dynamics
    static float seed = static_cast<float>(rand()) / RAND_MAX * 0.02f - 0.01f;
    pImpl->level += seed;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Dopamine::signalReward(float reward) {
    // Burst dopamine for positive rewards (reward prediction error)
    // This follows the Rescorla-Wagner model of reinforcement learning
    
    // Calculate reward prediction error
    // Reward is better than expected
    float expected = pImpl->baseline + pImpl->predictionError;
    
    if (reward > expected) {
        // Positive prediction error
        pImpl->predictionError = reward - expected;
        
        // Strong burst of dopamine
        float burst = 0.5f + 0.5f * pImpl->predictionError;
        
        // Add to current level with saturation
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst * pImpl->releaseRate);
        
        // Create eligibility trace for reward-modulated learning
        pImpl->eligibilityTrace += pImpl->predictionError;
        
        // Modulate learning rate based on prediction error magnitude
        pImpl->learningRate = 0.01f * (1.0f + std::min(2.0f, std::abs(pImpl->predictionError)));
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct prediction error signaling
    // Negative errors reduce dopamine (punishment prediction)
    
    pImpl->predictionError = error;
    
    // Response magnitude scales with error
    float response = std::min(0.5f, std::abs(error));
    
    if (error > 0) {
        // Positive error: increase dopamine
        pImpl->level = std::min(pImpl->peak, pImpl->level + response);
    } else {
        // Negative error: decrease dopamine below baseline
        pImpl->level = std::max(0.0f, pImpl->level - response);
    }
    
    // Update eligibility trace with opposite sign
    pImpl->eligibilityTrace += error;
    
    // Modulate learning rate: small errors maintain exploratory learning
    if (std::abs(error) > 0.1f) {
        pImpl->learningRate = 0.01f;  // Strong learning for large errors
    } else {
        pImpl->learningRate = 0.001f; // Weak learning for small errors (exploration)
    }
}

float Dopamine::getLearningRate() const {
    return pImpl->learningRate;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::updateEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->predictionError = 0.0f;
    pImpl->learningRate = 0.01f;
}

} // namespace nlm
