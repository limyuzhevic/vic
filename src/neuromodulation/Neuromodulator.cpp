#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    // Ornstein-Uhlenbeck process parameters for realistic dopamine dynamics
    float level;                    // Current dopamine level (0-1)
    float baseline;                 // Tonic firing level (0.2 = baseline)
    float peak;                     // Phasic burst peak (2.5x baseline)
    float theta;                    // Target level for OU process
    float alpha;                    // Mean reversion speed
    float sigma;                    // Volatility (prediction error sensitivity)
    float phasicGain;              // Phasic burst sensitivity
    float adaptationRate;          // Rate of adaptation to expected rewards
    
    // Phasic vs tonic separation
    float tonicLevel;              // Tonic component (slow learning)
    float phasicBursts[4];         // Recent phasic bursts
    int burstIndex;                // Index for circular burst buffer
    
    // Learning state
    float rewardHistory[20];       // Recent rewards (Rescorla-Wagner)
    float predictionHistory[20];   // Recent prediction errors
    int historyIndex;              // Circular buffer index
    
    // Plasticity modulation parameters
    float dopamineForLTP;         // Dopamine threshold for LTP
    float dopamineForLTD;         // Dopamine threshold for LTD
    float peakLTP;                 // Maximum LTP enhancement
    float peakLTD;                 // Maximum LTD enhancement
    
    // Reward prediction (temporal difference)
    float valueFunction;           // V(s) - current state value
    float rewardPrediction;        // Expected reward r
    
    // Random number generator for phasic variability
    std::mt19937 rng;
    std::normal_distribution<float> noiseDist;
    
    Impl() : level(0.0f), baseline(0.2f), peak(1.0f), 
             theta(baseline), alpha(0.5f), sigma(0.15f), 
             phasicGain(1.2f), adaptationRate(0.1f),
             tonicLevel(baseline), burstIndex(0), historyIndex(0),
             dopamineForLTP(0.5f), dopamineForLTD(0.3f),
             peakLTP(1.5f), peakLTD(0.7f),
             valueFunction(0.0f), rewardPrediction(0.0f),
             rng(std::random_device{}()), noiseDist(0.0f, 0.1f) {
        for (int i = 0; i < 4; ++i) phasicBursts[i] = 0.0f;
        for (int i = 0; i < 20; ++i) {
            rewardHistory[i] = 0.0f;
            predictionHistory[i] = 0.0f;
        }
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
    // Real dopamine-modulated plasticity: inverted U-shaped relationship
    // Optimal plasticity at moderate dopamine levels (~0.6-0.8)
    // Low dopamine (basal) = low plasticity, high dopamine = low plasticity (saturation)
    
    float normalizedLevel = pImpl->level / pImpl->peak;
    if (normalizedLevel < 0.3f) {
        // Low dopamine -> reduced plasticity (LTD biased)
        return 0.3f * (normalizedLevel / 0.3f);
    } else if (normalizedLevel < 0.8f) {
        // Optimal range -> peak plasticity
        return 1.0f - 0.3f * std::pow((normalizedLevel - 0.8f) / 0.5f, 2);
    } else {
        // High dopamine -> reduced plasticity (LTP saturation)
        return 0.7f * std::pow((1.0f - normalizedLevel) / 0.2f, 1.5f);
    }
}

void Dopamine::update(TimestepDuration dt) {
    // Ornstein-Uhlenbeck process for realistic dopamine dynamics
    // dθ = α(θ - μ)dt + σdW
    // θ = target level, μ = baseline, σ = volatility
    
    float dtFloat = static_cast<float>(dt);
    
    // Mean reversion towards baseline
    float meanReversion = pImpl->alpha * (pImpl->theta - pImpl->level) * dtFloat;
    
    // Add noise representing prediction error
    float noise = pImpl->sigma * pImpl->noiseDist(pImpl->rng) * std::sqrt(dtFloat);
    
    // Update target level based on recent rewards and predictions
    float rewardInfluence = 0.0f;
    if (pImpl->historyIndex > 0) {
        for (int i = 0; i < 5; ++i) {
            int idx = (pImpl->historyIndex - 1 - i + 20) % 20;
            rewardInfluence += pImpl->rewardHistory[idx] * std::pow(0.7f, i);
        }
    }
    
    pImpl->theta += (rewardInfluence - pImpl->valueFunction) * pImpl->adaptationRate * dtFloat;
    pImpl->theta = std::clamp(pImpl->theta, 0.0f, 1.5f * pImpl->baseline);
    
    // Update level with OU process
    pImpl->level += meanReversion + noise;
    
    // Decay towards theta (target level)
    float decay = pImpl->alpha * (pImpl->theta - pImpl->level) * dtFloat;
    pImpl->level += decay;
    
    // Maintain within bounds and update tonic/phasic components
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
    pImpl->tonicLevel = pImpl->baseline + (pImpl->level - pImpl->baseline) * 0.1f;
    
    // Rotate phasic burst buffer
    pImpl->burstIndex = (pImpl->burstIndex + 1) % 4;
    pImpl->phasicBursts[pImpl->burstIndex] = 0.0f;
}

void Dopamine::signalReward(float reward) {
    // Real reward signaling with phasic bursts and temporal integration
    
    // Phasic burst for immediate reward prediction error
    float predictionError = reward - pImpl->rewardPrediction;
    
    // Phasic burst magnitude depends on prediction error
    float burstMagnitude = std::max(0.0f, std::min(1.0f, std::abs(predictionError) * 2.0f));
    
    // Add burst to the buffer (phasic component)
    pImpl->phasicBursts[pImpl->burstIndex] += burstMagnitude * pImpl->phasicGain;
    
    // Update total level with burst
    float phasicComponent = 0.0f;
    for (int i = 0; i < 4; ++i) {
        phasicComponent += pImpl->phasicBursts[i];
    }
    phasicComponent /= 4.0f;
    
    pImpl->level = pImpl->tonicLevel + phasicComponent * 0.5f;
    
    // Update prediction and value function (temporal difference learning)
    float gamma = 0.9f; // discount factor
    pImpl->valueFunction = gamma * pImpl->valueFunction + reward;
    pImpl->rewardPrediction = pImpl->rewardPrediction * 0.9f + reward * 0.1f;
    
    // Update reward history (Rescorla-Wagner)
    pImpl->rewardHistory[pImpl->historyIndex] = reward;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 20;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Reward prediction error signaling (δ = r + γV(s') - V(s))
    
    // Update prediction error directly
    float predictionError = error;
    
    // Phasic burst for prediction error
    float burstMagnitude = std::max(0.0f, std::min(1.0f, std::abs(predictionError) * 3.0f));
    
    // Add to phasic buffer
    pImpl->phasicBursts[pImpl->burstIndex] += burstMagnitude * pImpl->phasicGain;
    
    // Update total level
    float phasicComponent = 0.0f;
    for (int i = 0; i < 4; ++i) {
        phasicComponent += pImpl->phasicBursts[i];
    }
    phasicComponent /= 4.0f;
    
    pImpl->level = pImpl->tonicLevel + phasicComponent * 0.5f;
    
    // Update prediction and value function
    float gamma = 0.9f;
    pImpl->valueFunction += predictionError;
    pImpl->rewardPrediction += predictionError * 0.1f; // slower adaptation
    
    // Update prediction error history
    pImpl->predictionHistory[pImpl->historyIndex] = predictionError;
    
    // Modulate eligibility traces based on dopamine
    // High dopamine → stronger LTP, low dopamine → stronger LTD
    if (pImpl->level > pImpl->dopamineForLTP) {
        // LTP regime: enhanced synaptic strengthening
        pImpl->theta = std::min(pImpl->theta + 0.1f, 2.0f * pImpl->baseline);
    } else if (pImpl->level < pImpl->dopamineForLTD) {
        // LTD regime: enhanced synaptic weakening
        pImpl->theta = std::max(pImpl->theta - 0.05f, pImpl->baseline * 0.5f);
    }
}

} // namespace nlm
