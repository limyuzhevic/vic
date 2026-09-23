#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Volume transmission parameters
    float diffusionRate;
    float receptorDensity;
    float reuptakeRate;
    
    // Neuronal dynamics
    float timeConstant;      // Membrane time constant for dopamine neurons
    float threshold;         // Spike threshold for dopamine neurons
    float resetPotential;    // Reset potential after spike
    
    // Reward prediction error parameters
    float rpeGain;           // RPE scaling factor
    float rpeTimeConstant;   // RPE integration time constant
    float predictionError;   // Current reward prediction error
    
    // Statistics for analysis
    float totalRPE;          // Cumulative reward prediction error
    float maxLevel;          // Maximum observed dopamine level
    size_t rewardEvents;      // Number of reward events processed
    
    Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             diffusionRate(0.02f), receptorDensity(0.8f), reuptakeRate(0.01f),
             timeConstant(100.0f), threshold(-55.0f), resetPotential(-70.0f),
             rpeGain(0.1f), rpeTimeConstant(50.0f), predictionError(0.0f),
             totalRPE(0.0f), maxLevel(0.0f), rewardEvents(0) {}
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
    // Realistic dopamine-modulated plasticity factor
    // Based on empirical findings: dopamine scales plasticity magnitude
    // with nonlinear relationship (inverted-U shape at very high levels)
    
    float normalizedLevel = pImpl->level / pImpl->peak;
    
    // Inverted-U relationship: optimal plasticity at moderate dopamine levels
    if (normalizedLevel < 0.3f) {
        // Suboptimal: too little dopamine - reduced plasticity
        return 0.3f + normalizedLevel * 2.0f;
    } else if (normalizedLevel < 0.8f) {
        // Optimal: moderate dopamine - enhanced plasticity
        return 0.6f + (normalizedLevel - 0.3f) * 0.6f;
    } else {
        // Excessive: too much dopamine - reduced plasticity (neurotoxicity)
        return 1.0f - (normalizedLevel - 0.8f) * 1.25f;
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Realistic dopamine dynamics based on computational neuroscience
    float dtSeconds = static_cast<float>(dt) * 0.001f;  // Convert ms to seconds
    
    // Volume transmission: dopamine diffuses from release sites
    float diffusionEffect = pImpl->diffusionRate * (pImpl->peak - pImpl->level);
    
    // Reuptake: dopamine clearance by transporters
    float reuptakeEffect = pImpl->reuptakeRate * pImpl->level;
    
    // Natural decay towards baseline
    float naturalDecay = pImpl->decayRate * pImpl->level;
    
    // Update dopamine level with all factors
    pImpl->level = pImpl->level + (diffusionEffect - reuptakeEffect - naturalDecay) * dtSeconds;
    
    // Ensure level stays within bounds
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    
    // Update statistics
    if (pImpl->level > pImpl->maxLevel) {
        pImpl->maxLevel = pImpl->level;
    }
}

void Dopamine::signalReward(float reward) {
    // Realistic reward signaling based on experimental data
    
    // Primary burst response: immediate dopamine increase proportional to reward magnitude
    float burstMagnitude = std::min(reward * pImpl->releaseRate, 1.0f - pImpl->level);
    pImpl->level += burstMagnitude;
    
    // Phasic component: higher for unexpected large rewards
    float predictionError = reward - pImpl->predictionError;
    float phasicBoost = std::max(0.0f, predictionError * pImpl->rpeGain * 0.5f);
    pImpl->level += phasicBoost * 0.3f;  // Phasic component is smaller
    
    // Update prediction error
    pImpl->predictionError = pImpl->predictionError * 0.9f + reward * 0.1f;
    
    // Update statistics
    pImpl->rewardEvents++;
    pImpl->totalRPE += predictionError;
    
    // Clamp to reasonable bounds
    pImpl->level = std::clamp(pImpl->level, pImpl->baseline, pImpl->peak);
    
    // Record RPE for learning
    if (std::abs(predictionError) > 1e-4f) {
        pImpl->predictionError += predictionError * 0.01f;  // Slow integration
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling based on temporal difference learning
    
    // Phasic dopamine response to prediction error
    float rpeResponse = error * pImpl->rpeGain;
    
    // Update dopamine level with RPE (can be positive or negative)
    pImpl->level += rpeResponse;
    
    // Constrain to biological limits (dopamine doesn't go below baseline)
    pImpl->level = std::max(pImpl->level, pImpl->baseline);
    
    // Integrate prediction error for learning
    pImpl->predictionError += error * pImpl->rpeTimeConstant * 0.001f;
    
    // Update statistics
    pImpl->totalRPE += error;
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getTotalRPE() const {
    return pImpl->totalRPE;
}

size_t Dopamine::getRewardEvents() const {
    return pImpl->rewardEvents;
}

float Dopamine::getMaxLevel() const {
    return pImpl->maxLevel;
}

float Dopamine::getReceptorDensity() const {
    return pImpl->receptorDensity;
}

void Dopamine::configure(float decayRate, float releaseRate, float diffusionRate, float reuptakeRate) {
    pImpl->decayRate = std::clamp(decayRate, 0.001f, 1.0f);
    pImpl->releaseRate = std::clamp(releaseRate, 0.1f, 5.0f);
    pImpl->diffusionRate = std::clamp(diffusionRate, 0.001f, 0.1f);
    pImpl->reuptakeRate = std::clamp(reuptakeRate, 0.001f, 0.1f);
}

} // namespace nlm
