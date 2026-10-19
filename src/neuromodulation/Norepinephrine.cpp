#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct Norepinephrine::Impl {
    // Locus coeruleus firing dynamics
    float firingRate;           // LC firing rate (Hz)
    float baselineFiring;      // Baseline firing rate (2-4 Hz)
    float stressLevel;          // Current stress level
    float fatigueLevel;         // Fatigue level (0-1)
    
    // Biological parameters
    float minLevel;             // Minimum biological level (10 pM)
    float maxLevel;             // Maximum biological level (100 pM)
    float decayRate;            // Decay rate (0.3 /s)
    float releaseRate;          // Release rate (1.0 /s)
    
    // Arousal and vigilance components
    float arousalLevel;         // Arousal level (0-1)
    float vigilanceLevel;       // Vigilance level (0-1)
    float attentionShift;       // Attention shifting amount (-1 to 1)
    
    // Sensory gain control
    float sensoryGain;          // Sensory input amplification
    float sensoryAdaptation;    // Sensory adaptation factor
    
    // Stress response
    float stressResponse;       // Stress hormone response level
    float timeSinceLastStress;  // Time since last stress response
    
    // State tracking
    float peakLevel;            // Historical peak
    float timeAboveBaseline;    // Time above baseline threshold
    float timeAboveHigh;        // Time above high threshold
    
    // History for temporal dynamics
    std::vector<float> levelHistory;
    std::vector<float> arousalHistory;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    Impl() : 
        firingRate(2.5f),           // Normal LC firing rate
        baselineFiring(2.5f),
        stressLevel(0.0f),
        fatigueLevel(0.0f),
        minLevel(10.0f),
        maxLevel(100.0f),
        decayRate(0.3f),
        releaseRate(1.0f),
        arousalLevel(0.5f),
        vigilanceLevel(0.5f),
        attentionShift(0.0f),
        sensoryGain(1.0f),
        sensoryAdaptation(1.0f),
        stressResponse(0.0f),
        timeSinceLastStress(0.0f),
        peakLevel(0.0f),
        timeAboveBaseline(0.0f),
        timeAboveHigh(0.0f),
        uniformDist(0.0f, 1.0f),
        normalDist(0.0f, 0.1f) {
        
        levelHistory.reserve(1000);
        arousalHistory.reserve(1000);
    }
};

Norepinephrine::Norepinephrine() : pImpl(std::make_unique<Impl>()) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->firingRate * pImpl->releaseRate;
}

void Norepinephrine::setLevel(float level) {
    // Clamp to biological range
    pImpl->firingRate = std::clamp(level / pImpl->releaseRate, 0.1f, 20.0f);
    
    // Update dependent parameters
    pImpl->arousalLevel = std::clamp(pImpl->firingRate / 20.0f, 0.0f, 1.0f);
    pImpl->vigilanceLevel = std::clamp(1.0f - std::exp(-pImpl->firingRate * 0.1f), 0.0f, 1.0f);
    
    // Update history
    pImpl->levelHistory.push_back(getLevel());
    if (pImpl->levelHistory.size() > 1000) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
    }
    
    NLM_LOG_INFO("Norepinephrine level set: " + std::to_string(getLevel()) + 
                 ", arousal=" + std::to_string(pImpl->arousalLevel) +
                 ", vigilance=" + std::to_string(pImpl->vigilanceLevel));
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances signal detection and stabilizes memory
    float arousalFactor = pImpl->arousalLevel * 0.5f;
    float vigilanceFactor = pImpl->vigilanceLevel * 0.3f;
    
    return 1.0f + arousalFactor + vigilanceFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Update firing rate dynamics
    pImpl->firingRate = pImpl->baselineFiring;
    
    // Stress increases firing rate
    if (pImpl->stressLevel > 0.1f) {
        pImpl->firingRate += pImpl->stressLevel * 5.0f;
    }
    
    // Fatigue decreases firing rate
    if (pImpl->fatigueLevel > 0.0f) {
        pImpl->firingRate *= (1.0f - pImpl->fatigueLevel * 0.3f);
    }
    
    // Decay stress and fatigue
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - 0.1f * static_cast<float>(dt));
    pImpl->fatigueLevel = std::max(0.0f, pImpl->fatigueLevel - 0.05f * static_cast<float>(dt));
    
    // Update state tracking
    float currentLevel = getLevel();
    pImpl->peakLevel = std::max(pImpl->peakLevel, currentLevel);
    
    if (currentLevel > pImpl->baselineFiring * 0.5f) {
        pImpl->timeAboveBaseline += static_cast<float>(dt);
    }
    
    if (currentLevel > pImpl->baselineFiring * 1.5f) {
        pImpl->timeAboveHigh += static_cast<float>(dt);
    }
    
    // Update arousal and vigilance
    pImpl->arousalLevel = std::clamp(pImpl->firingRate / 20.0f, 0.0f, 1.0f);
    pImpl->vigilanceLevel = std::clamp(1.0f - std::exp(-pImpl->firingRate * 0.1f), 0.0f, 1.0f);
    
    // Update sensory gain with adaptation
    pImpl->sensoryGain = std::clamp(pImpl->sensoryGain + 
                                   (pImpl->arousalLevel - pImpl->sensoryGain) * 0.1f, 0.5f, 3.0f);
    
    // Update sensory adaptation
    pImpl->sensoryAdaptation = std::clamp(pImpl->sensoryAdaptation + 
                                         (1.0f - pImpl->sensoryAdaptation) * 0.01f, 0.1f, 1.0f);
    
    // Update arousal history
    pImpl->arousalHistory.push_back(pImpl->arousalLevel);
    if (pImpl->arousalHistory.size() > 1000) {
        pImpl->arousalHistory.erase(pImpl->arousalHistory.begin());
    }
    
    NLM_LOG_INFO("Norepinephrine updated: firingRate=" + std::to_string(pImpl->firingRate) +
                 ", arousal=" + std::to_string(pImpl->arousalLevel) +
                 ", vigilance=" + std::to_string(pImpl->vigilanceLevel));
}

void Norepinephrine::setLocusCoeruleusFiringRate(float rate) {
    // Set locus coeruleus firing rate (normal range: 2-4 Hz, stress can increase to 10+ Hz)
    pImpl->baselineFiring = std::clamp(rate, 0.1f, 15.0f);
}

float Norepinephrine::getLocusCoeruleusFiringRate() const {
    return pImpl->firingRate;
}

void Norepinephrine::updateFiringDynamics(TimestepDuration dt) {
    // Update firing dynamics with burst-pause patterns
    pImpl->timeSinceLastStress += static_cast<float>(dt);
    
    // Random phasic increases
    if (pImpl->uniformDist(pImpl->rng) < 0.05f * static_cast<float>(dt)) {
        pImpl->firingRate += pImpl->normalDist(pImpl->rng) * 2.0f;
        pImpl->firingRate = std::clamp(pImpl->firingRate, pImpl->baselineFiring, pImpl->baselineFiring * 3.0f);
    }
}

void Norepinephrine::applyStress(float stressLevel) {
    // Apply stress level (0-1)
    pImpl->stressLevel = std::clamp(stressLevel, 0.0f, 1.0f);
    pImpl->timeSinceLastStress = 0.0f;
    
    // Stress increases firing rate
    if (pImpl->stressLevel > 0.0f) {
        pImpl->firingRate += pImpl->stressLevel * 8.0f;
    }
    
    // Stress response buildup
    pImpl->stressResponse = std::clamp(pImpl->stressResponse + stressLevel * 0.1f, 0.0f, 1.0f);
    
    NLM_LOG_INFO("Norepinephrine stress response: level=" + std::to_string(pImpl->stressLevel) +
                 ", firingRate=" + std::to_string(pImpl->firingRate));
}

void Norepinephrine::applyFatigue(float fatigueLevel) {
    // Apply fatigue (0-1)
    pImpl->fatigueLevel = std::clamp(fatigueLevel, 0.0f, 1.0f);
    
    // Fatigue decreases firing rate
    pImpl->firingRate *= (1.0f - pImpl->fatigueLevel * 0.5f);
    
    NLM_LOG_INFO("Norepinephrine fatigue response: level=" + std::to_string(pImpl->fatigueLevel) +
                 ", firingRate=" + std::to_string(pImpl->firingRate));
}

float Norepinephrine::getStressModulation() const {
    return pImpl->stressLevel;
}

float Norepinephrine::getFatigueModulation() const {
    return pImpl->fatigueLevel;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getVigilanceLevel() const {
    return pImpl->vigilanceLevel;
}

void Norepinephrine::shiftAttention(float shiftAmount) {
    // Shift attention (positive for right, negative for left)
    pImpl->attentionShift = std::clamp(shiftAmount, -1.0f, 1.0f);
    
    // Shift affects firing rate
    pImpl->firingRate += pImpl->attentionShift * 1.5f;
    
    NLM_LOG_INFO("Norepinephrine attention shift: amount=" + std::to_string(shiftAmount) +
                 ", shift=" + std::to_string(pImpl->attentionShift));
}

float Norepinephrine::getAttentionShift() const {
    return pImpl->attentionShift;
}

NeuromodulatorInteraction Norepinephrine::getInteractionWith(const Neuromodulator& other) const {
    // NE typically synergizes with dopamine and acetylcholine
    const auto* otherNE = dynamic_cast<const Norepinephrine*>(&other);
    if (otherNE) {
        return NeuromodulatorInteraction::Additive;
    }
    
    const auto* da = dynamic_cast<const Dopamine*>(&other);
    if (da) {
        return NeuromodulatorInteraction::Synergistic;
    }
    
    const auto* ach = dynamic_cast<const Acetylcholine*>(&other);
    if (ach) {
        return NeuromodulatorInteraction::Synergistic;
    }
    
    const auto* 5ht = dynamic_cast<const Serotonin*>(&other);
    if (5ht) {
        return NeuromodulatorInteraction::Antagonistic;
    }
    
    return NeuromodulatorInteraction::Neutral;
}

float Norepinephrine::getSynergyMultiplier(const Neuromodulator& other) const {
    if (getInteractionWith(other) == NeuromodulatorInteraction::Synergistic) {
        return 1.4f; // 40% increase in effect
    }
    return 1.0f;
}

float Norepinephrine::getAntagonismMultiplier(const Neuromodulator& other) const {
    if (getInteractionWith(other) == NeuromodulatorInteraction::Antagonistic) {
        return 0.6f; // 40% reduction in effect
    }
    return 1.0f;
}

float Norepinephrine::getHistoryAverage(unsigned int window) const {
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

float Norepinephrine::getPeakLevel() const {
    return pImpl->peakLevel;
}

float Norepinephrine::getTimeAboveThreshold(float threshold) const {
    if (getLevel() > threshold) {
        return pImpl->timeAboveBaseline + pImpl->timeAboveHigh;
    }
    return 0.0f;
}

bool Norepinephrine::isWithinBiologicalRange(float level) const {
    return level >= pImpl->minLevel && level <= pImpl->maxLevel;
}

float Norepinephrine::getMinLevel() const {
    return pImpl->minLevel;
}

float Norepinephrine::getMaxLevel() const {
    return pImpl->maxLevel;
}

} // namespace nlm