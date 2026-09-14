#include "Dopamine.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Dopamine::Impl {
    // Real dopamine dynamics based on computational neuroscience
    float level;                // Current dopamine concentration (0-1 normalized)
    float baseline;             // Baseline dopamine level (tonic firing)
    float peak;                 // Maximum dopamine level (phasic response)
    float decayRate;            // Reuptake rate constant
    float releaseRate;          // Release rate on prediction error
    float eligibilityTrace;     // Synapses that have seen reward prediction error
    float predictionError;      // Latest reward prediction error signal
    float predictedValue;       // Current value prediction
    
    // Temporal dynamics
    float timeSinceLastEvent;   // Time since last spike event
    float refractoryPeriod;     // Minimum time between events
    float lastSpikeTime;        // Time of last spike
    
    // Neuromodulatory state
    float explorationBonus;    // Curiosity-driven exploration bonus
    float rewardHistory;        // Recent reward history
    
    // Random number generator for biological noise
    mutable std::mt19937 rng;
    mutable std::uniform_real_distribution<float> uniformDist;
    
    Impl() : level(0.0f), baseline(0.15f), peak(1.0f), decayRate(0.2f), releaseRate(0.8f),
             eligibilityTrace(0.0f), predictionError(0.0f), predictedValue(0.0f),
             timeSinceLastEvent(0.0f), refractoryPeriod(0.1f), lastSpikeTime(0.0f),
             explorationBonus(0.0f), rewardHistory(0.0f),
             rng(std::random_device()()), uniformDist(0.0f, 1.0f) {}
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
    // Realistic dopamine modulation of plasticity:
    // - Low DA: Strong LTD, weak LTP (stability)
    // - Medium DA: Balanced learning
    // - High DA: Strong LTP, weak LTD (plasticity)
    // Formula from computational models of dopamine effects
    float normalizedError = std::clamp(pImpl->predictionError + 1.0f, 0.0f, 2.0f) / 2.0f;
    
    // Add exploration bonus modulation
    float explorationMod = 1.0f + pImpl->explorationBonus * 0.5f;
    
    // Biological factor: DA modulates synaptic strength based on prediction error
    float plasticity = 0.3f + 0.7f * normalizedError * explorationMod;
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Dopamine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Real dopamine dynamics:
    // 1. Natural decay due to reuptake (exponential decay)
    float decayAmount = pImpl->decayRate * deltaTime;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
    
    // 2. Eligibility trace decay (longer than dopamine level)
    if (pImpl->eligibilityTrace > 0.0f) {
        pImpl->eligibilityTrace *= std::exp(-pImpl->decayRate * deltaTime * 0.5f);
    }
    
    // 3. Refractory period tracking
    pImpl->timeSinceLastEvent += deltaTime;
    
    // 4. Exploration bonus decay
    if (pImpl->explorationBonus > 0.0f) {
        pImpl->explorationBonus *= std::exp(-pImpl->decayRate * deltaTime * 2.0f);
    }
    
    // 5. Reward history decay
    pImpl->rewardHistory *= std::exp(-pImpl->decayRate * deltaTime);
}

void Dopamine::signalReward(float reward) {
    // Realistic reward signaling with temporal dynamics:
    // 1. Track if we're in refractory period
    if (pImpl->timeSinceLastEvent < pImpl->refractoryPeriod) {
        // Suppress response during refractory period
        return;
    }
    
    // 2. Biological noise in reward processing (10-20% variance)
    float noise = pImpl->uniformDist(pImpl->rng) * 0.2f - 0.1f;
    float effectiveReward = std::max(0.0f, reward + noise);
    
    // 3. Phasic response to reward with saturation
    float rewardSignal = std::min(effectiveReward, 1.0f);
    float newLevel = std::max(pImpl->level, 
                           std::min(pImpl->peak, pImpl->level + rewardSignal * pImpl->releaseRate * 2.0f));
    pImpl->level = newLevel;
    
    // 4. Update eligibility trace for reward-modulated learning
    // Eligibility trace: synapses that were active when reward prediction error occurred
    pImpl->eligibilityTrace = std::min(1.0f, pImpl->eligibilityTrace + rewardSignal * 0.5f);
    
    // 5. Update reward history (exponential moving average)
    pImpl->rewardHistory = 0.9f * pImpl->rewardHistory + 0.1f * reward;
    
    // 6. Reset refractory timer
    pImpl->timeSinceLastEvent = 0.0f;
}

void Dopamine::signalRewardPredictionError(float error) {
    // Core dopamine function: reward prediction error (TD learning)
    pImpl->predictionError = error;
    
    // TD update: dopamine encodes reward prediction error
    // ΔV = α * (R - V), where V is predicted value, R is actual reward
    float tdUpdate = error * pImpl->releaseRate;
    
    // Update current level based on prediction error with biological saturation
    if (error > 0.0f) {
        // Positive prediction error (better than expected): increase level (learning)
        // Incorporate exploration bonus
        float learningMod = 1.0f + pImpl->explorationBonus;
        pImpl->level = std::min(pImpl->peak, pImpl->level + tdUpdate * 1.5f * learningMod);
    } else {
        // Negative prediction error (worse than expected): decrease level towards baseline
        // But not below baseline to maintain tonic firing
        pImpl->level = std::max(pImpl->baseline, pImpl->level + tdUpdate * 0.8f);
    }
    
    // Update predicted value based on prediction error
    // V_{t+1} = V_t + α * δ
    pImpl->predictedValue = std::clamp(pImpl->predictedValue + error * 0.1f, 0.0f, 1.0f);
}

float Dopamine::getPredictionError() const {
    return pImpl->predictionError;
}

float Dopamine::getPredictedValue() const {
    return pImpl->predictedValue;
}

float Dopamine::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Dopamine::updateEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = std::clamp(trace, 0.0f, 1.0f);
}

void Dopamine::addExplorationBonus(float bonus) {
    // Add curiosity-driven exploration bonus to dopamine level
    pImpl->explorationBonus = std::min(1.0f, pImpl->explorationBonus + bonus * 0.5f);
}

float Dopamine::getExplorationBonus() const {
    return pImpl->explorationBonus;
}

float Dopamine::getRewardHistory() const {
    return pImpl->rewardHistory;
}

} // namespace nlm