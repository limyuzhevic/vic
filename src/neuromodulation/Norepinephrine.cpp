#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float tonicLevel;
    float phasicLevel;
    float arousalLevel;
    float vigilanceLevel;
    float stressLevel;
    
    // Adaptive dynamics
    float sensitivity;
    float threshold;
    float adaptationRate;
    float recoveryRate;
    
    // Time tracking
    float timeSinceLastStimulus;
    float timeSinceLastRecovery;
    float lastStressTime;
    
    // Temporal components
    float arousalHistory;
    float vigilanceHistory;
    float stressHistory;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : level(0.0f), baseline(0.1f), tonicLevel(0.1f), phasicLevel(0.0f),
             arousalLevel(0.1f), vigilanceLevel(0.1f), stressLevel(0.0f),
             sensitivity(1.0f), threshold(0.5f), adaptationRate(0.1f),
             recoveryRate(0.05f), timeSinceLastStimulus(0.0f),
             timeSinceLastRecovery(0.0f), lastStressTime(0.0f),
             arousalHistory(0.0f), vigilanceHistory(0.0f), stressHistory(0.0f)
    {
        std::random_device rd;
        rng.seed(rd());
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE modulates plasticity based on arousal and vigilance
    // Inverted U-curve relationship with arousal
    float optimalArousal = 0.5f;
    float arousalDeviation = std::abs(pImpl->arousalLevel - optimalArousal);
    
    // Plasticity peaks at moderate arousal
    float arousalPlasticity = 1.0f - arousalDeviation * arousalDeviation * 2.0f;
    
    // Vigilance enhances plasticity
    float vigilancePlasticity = pImpl->vigilanceLevel;
    
    // Stress reduces plasticity (chronic stress impairs learning)
    float stressPlasticity = 1.0f - pImpl->stressLevel * 0.5f;
    
    // Combined plasticity factor
    float plasticity = arousalPlasticity * vigilancePlasticity * stressPlasticity;
    
    return std::clamp(plasticity, 0.1f, 2.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    float timeStep = static_cast<float>(dt);
    
    // Update time tracking
    pImpl->timeSinceLastStimulus += timeStep;
    pImpl->timeSinceLastRecovery += timeStep;
    
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline,
                           pImpl->level - pImpl->recoveryRate * timeStep);
    
    // Update phasic component
    pImpl->phasicLevel *= (1.0f - pImpl->adaptationRate * timeStep);
    
    // Update temporal histories
    pImpl->arousalHistory *= 0.95f;
    pImpl->vigilanceHistory *= 0.95f;
    pImpl->stressHistory *= 0.95f;
    
    // Apply stress recovery over time
    if (pImpl->stressLevel > 0.0f) {
        pImpl->stressLevel -= pImpl->recoveryRate * timeStep * 2.0f;
        pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel);
    }
}

void Norepinephrine::signalStress(float stressLevel) {
    // Signal stress response
    pImpl->stressLevel = std::min(1.0f, stressLevel);
    pImpl->lastStressTime += static_cast<float>(1.0 / 60.0); // Assume 60Hz
    
    // Stress triggers phasic response
    pImpl->phasicLevel = std::min(pImpl->phasicLevel + stressLevel * 0.5f, 1.0f);
    
    // Overall NE level increases with stress
    pImpl->level = std::min(1.0f, pImpl->level + stressLevel * 0.3f);
    
    // Update stress history
    pImpl->stressHistory += stressLevel;
    
    NLM_LOG_DEBUG("NE stress signal: stress=%.3f, phasic=%.3f", stressLevel, pImpl->phasicLevel);
}

void Norepinephrine::signalNovelty(float noveltyLevel) {
    // Signal novelty detection
    // Novelty increases arousal and vigilance
    
    // Arousal response to novelty
    pImpl->arousalLevel = std::min(1.0f, noveltyLevel * 1.5f);
    pImpl->arousalHistory += pImpl->arousalLevel;
    
    // Vigilance increase for novel stimuli
    if (noveltyLevel > pImpl->threshold) {
        // High novelty triggers heightened vigilance
        pImpl->vigilanceLevel = std::min(1.0f, noveltyLevel * 0.8f);
        pImpl->vigilanceHistory += pImpl->vigilanceLevel;
        
        // Phasic NE response
        pImpl->phasicLevel = std::min(pImpl->phasicLevel + noveltyLevel * 0.7f, 1.0f);
    }
    
    // Overall NE level includes novelty component
    pImpl->level = std::min(1.0f, pImpl->level + noveltyLevel * 0.2f);
    
    // Update time since stimulus
    pImpl->timeSinceLastStimulus = 0.0f;
    
    NLM_LOG_DEBUG("NE novelty signal: novelty=%.3f, arousal=%.3f, vigilance=%.3f",
                  noveltyLevel, pImpl->arousalLevel, pImpl->vigilanceLevel);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

void Norepinephrine::setArousalBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
}

void Norepinephrine::setVigilanceThreshold(float threshold) {
    pImpl->threshold = std::clamp(threshold, 0.0f, 1.0f);
}

void Norepinephrine::setStressSensitivity(float sensitivity) {
    pImpl->sensitivity = std::clamp(sensitivity, 0.1f, 2.0f);
}

void Norepinephrine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->phasicLevel = 0.0f;
    pImpl->arousalLevel = 0.1f;
    pImpl->vigilanceLevel = 0.1f;
    pImpl->stressLevel = 0.0f;
    
    // Reset temporal components
    pImpl->arousalHistory = 0.0f;
    pImpl->vigilanceHistory = 0.0f;
    pImpl->stressHistory = 0.0f;
    
    NLM_LOG_INFO("Norepinephrine system reset");
}

} // namespace nlm