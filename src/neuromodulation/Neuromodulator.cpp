#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    // Core dopamine dynamics
    float level;
    float baseline;              // Tonic dopamine baseline
    float peak;                  // Maximum achievable level
    
    // Phasic dynamics
    float burstIntensity;        // Intensity of dopamine bursts (unexpected rewards)
    float predictionError;      // Reward prediction error signal
    float temporalDifference;    // TD error signal
    
    // Adaptive properties
    float adaptationRate;        // Rate of homeostatic adaptation
    float learningRate;          // Learning rate for prediction updates
    float memoryTrace;           // Eligibility trace for dopamine
    
    // Burst dynamics
    float refractoryPeriod;      // Refractory period after burst
    float burstCooldown;         // Cooldown between bursts
    float burstThreshold;        // Threshold to trigger burst
    
    // State variables
    float timeSinceLastBurst;    // Time since last burst event
    float expectedValue;         // Expected value prediction
    float rewardHistory[10];     // Rolling reward history
    
    // Plasticity modulation parameters
    float weightFactor;          // Weight factor for plasticity modulation
    float eligibilityExponent;   // Exponent for eligibility trace decay
    
    // Random generator for burst variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : level(0.1f), baseline(0.1f), peak(1.0f)
           , burstIntensity(0.0f), predictionError(0.0f), temporalDifference(0.0f)
           , adaptationRate(0.05f), learningRate(0.1f), memoryTrace(0.0f)
           , refractoryPeriod(100.0f), burstCooldown(0.0f), burstThreshold(0.5f)
           , timeSinceLastBurst(0.0f), expectedValue(0.0f), weightFactor(1.0f)
           , eligibilityExponent(0.9f) {
        rng.seed(std::random_device{}());
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
        std::fill(std::begin(rewardHistory), std::end(rewardHistory), 0.0f);
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
    // Biologically plausible dopamine-modulated plasticity factor
    // Incorporates both phasic and tonic components with saturation
    float phasicComponent = pImpl->burstIntensity * 0.6f;
    float tonicComponent = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
    
    // Combined plasticity factor with non-linear interaction
    float plasticityFactor = phasicComponent + tonicComponent * 0.4f;
    
    // Apply sigmoidal saturation for bounded response
    plasticityFactor = 1.0f / (1.0f + std::exp(-10.0f * (plasticityFactor - 0.5f)));
    
    return plasticityFactor;
}

void Dopamine::update(TimestepDuration dt) {
    // Update dopamine dynamics with biologically plausible model
    pImpl->timeSinceLastBurst += static_cast<float>(dt);
    
    // Homeostatic adaptation - return to baseline
    float adaptationForce = (pImpl->baseline - pImpl->level) * pImpl->adaptationRate;
    pImpl->level += adaptationForce * static_cast<float>(dt);
    
    // Decay of burst intensity and prediction error
    if (pImpl->burstIntensity > 0.0f) {
        pImpl->burstIntensity *= (1.0f - pImpl->adaptationRate * 2.0f);
    }
    if (pImpl->predictionError != 0.0f) {
        pImpl->predictionError *= 0.95f;
    }
    
    // Update memory trace with eligibility decay
    if (pImpl->memoryTrace > 0.0f) {
        pImpl->memoryTrace = std::pow(pImpl->memoryTrace, pImpl->eligibilityExponent);
    }
    
    // Cooldown recovery
    if (pImpl->burstCooldown > 0.0f) {
        pImpl->burstCooldown -= static_cast<float>(dt);
    }
}

void Dopamine::signalReward(float reward) {
    // Phasic dopamine burst signaling for reward reception
    // Implements temporal difference reward prediction error
    
    // Calculate reward prediction error (RPE)
    float rpe = reward - pImpl->expectedValue;
    pImpl->predictionError = rpe;
    pImpl->temporalDifference = rpe;
    
    // Update expected value
    pImpl->expectedValue += pImpl->learningRate * rpe;
    
    // Rolling reward history update
    for (int i = 9; i > 0; --i) {
        pImpl->rewardHistory[i] = pImpl->rewardHistory[i - 1];
    }
    pImpl->rewardHistory[0] = reward;
    
    // Burst generation based on RPE magnitude and history
    float burstMagnitude = std::abs(rpe) * pImpl->learningRate;
    
    // Add variability to burst generation (biologically realistic)
    float variability = uniformDist(pImpl->rng) * 0.2f - 0.1f;
    burstMagnitude *= (1.0f + variability);
    
    // Apply refractory and cooldown constraints
    if (pImpl->timeSinceLastBurst >= pImpl->refractoryPeriod) {
        pImpl->burstCooldown = 50.0f; // Cooldown period
    }
    
    if (pImpl->burstCooldown <= 0.0f && burstMagnitude > pImpl->burstThreshold) {
        // Generate burst
        pImpl->burstIntensity = std::min(burstMagnitude, 1.0f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->burstIntensity * 2.0f);
        
        // Reset burst-related state
        pImpl->timeSinceLastBurst = 0.0f;
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Direct reward prediction error signaling (for cases where RPE is computed externally)
    pImpl->predictionError = error;
    pImpl->temporalDifference = error;
    
    // Update expected value
    pImpl->expectedValue += pImpl->learningRate * error;
    
    // Convert error to burst intensity
    float burstMagnitude = std::abs(error) * 0.5f * pImpl->learningRate;
    
    // Add noise for biological realism
    float noise = uniformDist(pImpl->rng) * 0.1f - 0.05f;
    burstMagnitude *= (1.0f + std::abs(noise));
    
    // Update burst if conditions met
    if (burstMagnitude > pImpl->burstThreshold) {
        pImpl->burstIntensity = burstMagnitude;
        pImpl->level = std::min(pImpl->peak, pImpl->level + burstMagnitude);
    }
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

float Dopamine::getExpectedValue() const {
    return pImpl->expectedValue;
}

float Dopamine::getBurstIntensity() const {
    return pImpl->burstIntensity;
}

float Dopamine::getAdaptivePlasticity() const {
    // Calculate adaptive plasticity factor based on recent reward history
    float recentSum = 0.0f;
    for (int i = 0; i < 5; ++i) {
        recentSum += pImpl->rewardHistory[i];
    }
    float recentAverage = recentSum / 5.0f;
    
    // Adapt learning rate based on reward consistency
    float consistency = 1.0f - std::abs(recentAverage);
    return consistency * getPlasticityFactor();
}

} // namespace nlm
