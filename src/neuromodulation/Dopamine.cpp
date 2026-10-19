#include "Dopamine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct Dopamine::Impl {
    // Phasic and tonic components
    float phasicComponent;      // Burst firing for unexpected rewards
    float tonicComponent;       // Baseline firing rate
    float baseline;             // Reference level
    float rewardPredictionError; // RPE signal from prediction error
    
    // Biological parameters
    float peak;                 // Maximum achievable concentration
    float minLevel;             // Minimum biological level (0.05 nM)
    float maxLevel;             // Maximum biological level (50 nM)
    float decayRate;            // Decay towards baseline (0.5 /s)
    float releaseRate;          // Release rate on reward (1.2 /s)
    float saturationThreshold;  // Saturation threshold (0.9)
    float depletionThreshold;   // Depletion threshold (0.1)
    
    // Receptor subtypes
    float d1ReceptorSensitivity; // D1 receptor sensitivity (0.7)
    float d2ReceptorSensitivity; // D2 receptor sensitivity (0.6)
    float d1ReceptorEffect;      // D1 receptor effect on plasticity
    float d2ReceptorEffect;      // D2 receptor effect on plasticity
    
    // Temporal dynamics
    float timeSinceLastBurst;    // Time since last dopamine burst
    float lastBurstTime;         // Time of last burst
    float burstDuration;         // Duration of burst (0.1s)
    float pauseDuration;         // Duration of pause after burst (0.5s)
    
    // State tracking
    float peakLevel;             // Historical peak level
    float minLevelObserved;      // Historical minimum level
    float timeAboveBaseline;     // Time above baseline threshold
    
    // Asymmetry parameters
    float rewardSensitivity;     // Sensitivity to rewards (1.5)
    float punishmentSensitivity; // Sensitivity to punishments (0.7)
    
    // History for state tracking
    std::vector<float> levelHistory;
    std::vector<float> errorHistory;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    Impl() : 
        phasicComponent(0.0f),
        tonicComponent(0.05f),  // Baseline tonic firing
        baseline(0.05f),
        rewardPredictionError(0.0f),
        peak(1.5f),
        minLevel(0.05f),
        maxLevel(50.0f),
        decayRate(0.5f),
        releaseRate(1.2f),
        saturationThreshold(0.9f),
        depletionThreshold(0.1f),
        d1ReceptorSensitivity(0.7f),
        d2ReceptorSensitivity(0.6f),
        d1ReceptorEffect(0.0f),
        d2ReceptorEffect(0.0f),
        timeSinceLastBurst(0.0f),
        lastBurstTime(0.0f),
        burstDuration(0.1f),
        pauseDuration(0.5f),
        peakLevel(0.05f),
        minLevelObserved(0.05f),
        timeAboveBaseline(0.0f),
        rewardSensitivity(1.5f),
        punishmentSensitivity(0.7f),
        uniformDist(0.0f, 1.0f),
        normalDist(0.0f, 0.1f) {
        
        levelHistory.reserve(1000);
        errorHistory.reserve(1000);
    }
};

Dopamine::Dopamine() : pImpl(std::make_unique<Impl>()) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    // Total dopamine level = phasic + tonic
    return pImpl->phasicComponent + pImpl->tonicComponent;
}

void Dopamine::setLevel(float level) {
    // Clamp to biological range
    pImpl->phasicComponent = std::clamp(level * 0.8f, pImpl->minLevel, pImpl->maxLevel * 0.8f);
    pImpl->tonicComponent = std::clamp(level * 0.2f, pImpl->minLevel, pImpl->maxLevel * 0.2f);
    
    // Update history
    pImpl->levelHistory.push_back(getLevel());
    if (pImpl->levelHistory.size() > 1000) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
    }
}

float Dopamine::getPlasticityFactor() const {
    // Combined D1 and D2 receptor effects on plasticity
    float d1Plasticity = pImpl->d1ReceptorSensitivity * pImpl->d1ReceptorEffect;
    float d2Plasticity = pImpl->d2ReceptorSensitivity * pImpl->d2ReceptorEffect;
    
    // D1 enhances LTP, D2 enhances LTD
    return 0.5f + 0.3f * d1Plasticity - 0.2f * d2Plasticity;
}

void Dopamine::update(TimestepDuration dt) {
    // Update temporal dynamics
    pImpl->timeSinceLastBurst += static_cast<float>(dt);
    
    // Decay phasic component towards baseline
    if (pImpl->phasicComponent > pImpl->baseline) {
        pImpl->phasicComponent = std::max(
            pImpl->baseline,
            pImpl->phasicComponent - pImpl->decayRate * static_cast<float>(dt)
        );
    }
    
    // Tonic component with small fluctuations
    pImpl->tonicComponent = pImpl->baseline + 
        pImpl->normalDist(pImpl->rng) * 0.01f;
    
    // Ensure tonic component is within bounds
    pImpl->tonicComponent = std::clamp(pImpl->tonicComponent, pImpl->minLevel, pImpl->maxLevel * 0.1f);
    
    // Update state tracking
    float currentLevel = getLevel();
    pImpl->peakLevel = std::max(pImpl->peakLevel, currentLevel);
    pImpl->minLevelObserved = std::min(pImpl->minLevelObserved, currentLevel);
    
    if (currentLevel > pImpl->baseline * 1.1f) {
        pImpl->timeAboveBaseline += static_cast<float>(dt);
    }
}

void Dopamine::signalReward(float reward) {
    // Enhance phasic component based on reward magnitude
    float rewardMagnitude = std::clamp(reward, -1.0f, 1.0f);
    
    // Apply asymmetric sensitivity (more sensitive to rewards than punishments)
    float sensitivity = rewardMagnitude > 0.0f ? 
        pImpl->rewardSensitivity : pImpl->punishmentSensitivity;
    
    float burstStrength = std::min(
        pImpl->peak - getLevel(),
        sensitivity * rewardMagnitude * pImpl->releaseRate * 0.01f
    );
    
    if (burstStrength > 0.0f) {
        pImpl->phasicComponent += burstStrength;
        pImpl->lastBurstTime = pImpl->timeSinceLastBurst;
        
        // Apply saturation if near peak
        if (getLevel() > pImpl->peak * pImpl->saturationThreshold) {
            applySaturation();
        }
        
        NLM_LOG_INFO("Dopamine burst: reward=" + std::to_string(reward) + 
                    ", level=" + std::to_string(getLevel()));
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // RPE signaling with biological constraints
    pImpl->rewardPredictionError = std::clamp(error, -1.0f, 1.0f);
    
    // Phasic dopamine response to RPE
    float rpeResponse = pImpl->rewardPredictionError * pImpl->releaseRate * 0.8f;
    
    if (pImpl->rewardPredictionError > 0.0f) {
        // Positive RPE (better than expected) -> increase dopamine
        pImpl->phasicComponent += rpeResponse;
        
        // Burst-pause pattern for unexpected rewards
        if (pImpl->timeSinceLastBurst > pImpl->pauseDuration) {
            NLM_LOG_INFO("Dopamine burst-pause: positive RPE=" + std::to_string(error));
        }
    } else {
        // Negative RPE (worse than expected) -> decrease dopamine
        pImpl->phasicComponent = std::max(
            pImpl->baseline,
            pImpl->phasicComponent + rpeResponse
        );
        
        // Suppress firing for punishments
        if (std::abs(error) > 0.3f) {
            NLM_LOG_INFO("Dopamine suppression: negative RPE=" + std::to_string(error));
        }
    }
}

void Dopamine::signalPhasicBurst(float magnitude) {
    // Direct phasic burst signaling
    float burst = std::clamp(magnitude, 0.0f, pImpl->peak - getLevel());
    pImpl->phasicComponent += burst;
    pImpl->lastBurstTime = pImpl->timeSinceLastBurst;
}

void Dopamine::signalTonicBaseline(float baseline) {
    // Set tonic baseline level
    pImpl->baseline = std::clamp(baseline, pImpl->minLevel, pImpl->maxLevel * 0.1f);
}

float Dopamine::getPhasicComponent() const {
    return pImpl->phasicComponent;
}

float Dopamine::getTonicComponent() const {
    return pImpl->tonicComponent;
}

float Dopamine::getD1ReceptorEffect() const {
    return pImpl->d1ReceptorEffect;
}

float Dopamine::getD2ReceptorEffect() const {
    return pImpl->d2ReceptorEffect;
}

void Dopamine::setD1ReceptorSensitivity(float sensitivity) {
    pImpl->d1ReceptorSensitivity = std::clamp(sensitivity, 0.0f, 1.0f);
}

void Dopamine::setD2ReceptorSensitivity(float sensitivity) {
    pImpl->d2ReceptorSensitivity = std::clamp(sensitivity, 0.0f, 1.0f);
}

void Dopamine::applySaturation() {
    // Dopamine saturation effect
    pImpl->phasicComponent = pImpl->peak * 0.9f;
    NLM_LOG_INFO("Dopamine saturation reached");
}

void Dopamine::applyDepletion(float depletionRate, TimestepDuration dt) {
    // Dopamine depletion effect
    float depletion = depletionRate * static_cast<float>(dt) * 0.1f;
    pImpl->phasicComponent = std::max(pImpl->baseline, pImpl->phasicComponent - depletion);
    pImpl->tonicComponent = std::max(pImpl->baseline, pImpl->tonicComponent - depletion * 0.5f);
}

bool Dopamine::isSaturated() const {
    return getLevel() > pImpl->peak * pImpl->saturationThreshold;
}

float Dopamine::getDepletionState() const {
    return (pImpl->baseline - getLevel()) / (pImpl->baseline - pImpl->minLevel);
}

float Dopamine::getLearningRateModulation(float predictionError) const {
    // Asymmetric learning rate modulation (more sensitive to rewards than punishments)
    float rewardError = std::max(0.0f, predictionError);
    float punishmentError = std::max(0.0f, -predictionError);
    
    float rewardLearningRate = pImpl->rewardSensitivity * rewardError * 0.1f;
    float punishmentLearningRate = pImpl->punishmentSensitivity * punishmentError * 0.05f;
    
    return rewardLearningRate - punishmentLearningRate;
}

NeuromodulatorInteraction Dopamine::getInteractionWith(const Neuromodulator& other) const {
    // Dopamine typically synergizes with norepinephrine and antagonizes with serotonin
    const auto* otherDA = dynamic_cast<const Dopamine*>(&other);
    if (otherDA) {
        return NeuromodulatorInteraction::Additive;
    }
    
    const auto* ach = dynamic_cast<const Acetylcholine*>(&other);
    if (ach) {
        return NeuromodulatorInteraction::Synergistic;
    }
    
    const auto* ne = dynamic_cast<const Norepinephrine*>(&other);
    if (ne) {
        return NeuromodulatorInteraction::Synergistic;
    }
    
    const auto* 5ht = dynamic_cast<const Serotonin*>(&other);
    if (5ht) {
        return NeuromodulatorInteraction::Antagonistic;
    }
    
    return NeuromodulatorInteraction::Neutral;
}

float Dopamine::getSynergyMultiplier(const Neuromodulator& other) const {
    // Calculate synergy effect with other neuromodulators
    if (getInteractionWith(other) == NeuromodulatorInteraction::Synergistic) {
        return 1.5f; // 50% increase in effect
    }
    return 1.0f;
}

float Dopamine::getAntagonismMultiplier(const Neuromodulator& other) const {
    // Calculate antagonism effect with other neuromodulators
    if (getInteractionWith(other) == NeuromodulatorInteraction::Antagonistic) {
        return 0.5f; // 50% reduction in effect
    }
    return 1.0f;
}

float Dopamine::getHistoryAverage(unsigned int window) const {
    if (pImpl->levelHistory.empty()) return getLevel();
    
    unsigned int startIdx = std::max(0, static_cast<int>(pImpl->levelHistory.size()) - static_cast<int>(window));
    float sum = 0.0f;
    unsigned int count = 0;
    
    for (unsigned int i = startIdx; i < pImpl->levelHistory.size(); ++i) {
        sum += pImpl->levelHistory[i];
        count++;
    }
    
    return count > 0 ? sum / count : getLevel();
}

float Dopamine::getPeakLevel() const {
    return pImpl->peakLevel;
}

float Dopamine::getTimeAboveThreshold(float threshold) const {
    return pImpl->timeAboveBaseline; // Simplified
}

bool Dopamine::isWithinBiologicalRange(float level) const {
    return level >= pImpl->minLevel && level <= pImpl->maxLevel;
}

float Dopamine::getMinLevel() const {
    return pImpl->minLevel;
}

float Dopamine::getMaxLevel() const {
    return pImpl->maxLevel;
}

} // namespace nlm