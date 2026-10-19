#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    // Core cholinergic components
    float level;                 // Current acetylcholine concentration
    float baseline;              // Baseline level
    float peak;                  // Maximum concentration (100 nM)
    
    // Biological parameters
    float minLevel;              // Minimum biological level (5 nM)
    float maxLevel;              // Maximum biological level (200 nM)
    float decayRate;             // Decay rate (0.8 /s)
    float releaseRate;           // Release rate (2.0 /s)
    
    // Attention and memory components
    float attentionStrength;     // Current attention modulation
    float memoryConsolidation;   // Memory enhancement level
    
    // Theta rhythm coupling
    float thetaFrequency;        // Theta oscillation frequency (4-8 Hz)
    float thetaPhase;             // Phase offset in radians
    float thetaCoupling;          // Strength of theta coupling
    
    // Learning rate modulation
    float learningRate;          // Current learning rate (0.01 - 0.5)
    float memoryTraceDecay;      // Memory trace decay rate
    
    // State tracking
    float peakLevel;             // Historical peak
    float timeAboveBaseline;     // Time above baseline
    
    // Sensory enhancement parameters
    float sensoryGain;           // Sensory input amplification factor
    std::vector<float> sensoryHistory; // Sensory history for adaptation
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    Impl() : 
        level(0.0f),
        baseline(10.0f),
        peak(100.0f),
        minLevel(5.0f),
        maxLevel(200.0f),
        decayRate(0.8f),
        releaseRate(2.0f),
        attentionStrength(0.0f),
        memoryConsolidation(0.0f),
        thetaFrequency(6.0f),
        thetaPhase(0.0f),
        thetaCoupling(0.5f),
        learningRate(0.01f),
        memoryTraceDecay(0.1f),
        peakLevel(0.0f),
        timeAboveBaseline(0.0f),
        sensoryGain(1.0f),
        uniformDist(0.0f, 1.0f),
        normalDist(0.0f, 0.1f) {
        
        sensoryHistory.reserve(1000);
    }
};

Acetylcholine::Acetylcholine() : pImpl(std::make_unique<Impl>()) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    // Clamp to biological range
    pImpl->level = std::clamp(level, pImpl->minLevel, pImpl->maxLevel);
    
    // Update dependent parameters
    pImpl->attentionStrength = pImpl->level / pImpl->maxLevel;
    pImpl->learningRate = 0.01f + 0.49f * (pImpl->level / pImpl->maxLevel);
    
    // Log for monitoring
    if (pImpl->level > pImpl->baseline * 1.5f) {
        NLM_LOG_INFO("Acetylcholine elevated: level=" + std::to_string(pImpl->level));
    }
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances learning and plasticity
    return 1.0f + pImpl->learningRate;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update theta phase
    pImpl->thetaPhase += pImpl->thetaFrequency * 2.0f * M_PI * static_cast<float>(dt);
    if (pImpl->thetaPhase > 2.0f * M_PI) {
        pImpl->thetaPhase -= 2.0f * M_PI;
    }
    
    // Update memory trace
    pImpl->memoryTraceDecay = std::clamp(pImpl->memoryTraceDecay + 0.001f * static_cast<float>(dt), 0.01f, 1.0f);
    
    // Store sensory history
    pImpl->sensoryHistory.push_back(pImpl->sensoryGain);
    if (pImpl->sensoryHistory.size() > 1000) {
        pImpl->sensoryHistory.erase(pImpl->sensoryHistory.begin());
    }
}

void Acetylcholine::modulateAttention(float attentionStrength) {
    // Modulate attention level
    pImpl->attentionStrength = std::clamp(attentionStrength, 0.0f, 1.0f);
    
    // Increase acetylcholine level proportionally
    pImpl->level = std::min(pImpl->maxLevel, pImpl->level + pImpl->attentionStrength * pImpl->releaseRate);
    
    NLM_LOG_INFO("Acetylcholine attention modulation: strength=" + std::to_string(pImpl->attentionStrength));
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionStrength;
}

void Acetylcholine::enhanceMemoryConsolidation(float memoryStrength) {
    // Enhance memory consolidation
    pImpl->memoryConsolidation = std::clamp(memoryStrength, 0.0f, 1.0f);
    
    // Increase acetylcholine level to boost consolidation
    pImpl->level = std::min(pImpl->maxLevel, pImpl->level + pImpl->memoryConsolidation * pImpl->releaseRate * 0.5f);
    
    // Decay memory consolidation over time
    pImpl->memoryTraceDecay = std::clamp(pImpl->memoryTraceDecay + 0.01f, 0.01f, 1.0f);
}

float Acetylcholine::getMemoryEnhancement() const {
    return pImpl->memoryConsolidation;
}

void Acetylcholine::setThetaFrequency(float frequency) {
    // Set theta rhythm frequency (4-8 Hz is biological range)
    pImpl->thetaFrequency = std::clamp(frequency, 4.0f, 8.0f);
}

float Acetylcholine::getThetaFrequency() const {
    return pImpl->thetaFrequency;
}

void Acetylcholine::phaseWithTheta(float phaseOffset) {
    // Set phase offset relative to theta rhythm
    pImpl->thetaPhase = std::fmod(phaseOffset, 2.0f * M_PI);
}

float Acetylcholine::getLearningRateModulation() const {
    return pImpl->learningRate;
}

float Acetylcholine::enhanceSensoryProcessing(float inputStrength) const {
    // Enhance sensory processing based on ACh level
    return inputStrength * (1.0f + pImpl->sensoryGain * pImpl->level / pImpl->maxLevel);
}

NeuromodulatorInteraction Acetylcholine::getInteractionWith(const Neuromodulator& other) const {
    // ACh typically synergizes with dopamine and norepinephrine
    const auto* otherACh = dynamic_cast<const Acetylcholine*>(&other);
    if (otherACh) {
        return NeuromodulatorInteraction::Synergistic;
    }
    
    const auto* da = dynamic_cast<const Dopamine*>(&other);
    if (da) {
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

float Acetylcholine::getSynergyMultiplier(const Neuromodulator& other) const {
    if (getInteractionWith(other) == NeuromodulatorInteraction::Synergistic) {
        return 1.3f; // 30% increase in effect
    }
    return 1.0f;
}

float Acetylcholine::getAntagonismMultiplier(const Neuromodulator& other) const {
    if (getInteractionWith(other) == NeuromodulatorInteraction::Antagonistic) {
        return 0.7f; // 30% reduction in effect
    }
    return 1.0f;
}

float Acetylcholine::getHistoryAverage(unsigned int window) const {
    if (pImpl->sensoryHistory.empty()) return getLevel();
    
    unsigned int startIdx = std::max(0, static_cast<int>(pImpl->sensoryHistory.size()) - static_cast<int>(window));
    float sum = 0.0f;
    unsigned int count = 0;
    
    for (unsigned int i = startIdx; i < pImpl->sensoryHistory.size(); ++i) {
        sum += pImpl->sensoryHistory[i];
        count++;
    }
    
    return count > 0 ? sum / count : getLevel();
}

float Acetylcholine::getPeakLevel() const {
    return pImpl->peakLevel;
}

float Acetylcholine::getTimeAboveThreshold(float threshold) const {
    return pImpl->timeAboveBaseline;
}

bool Acetylcholine::isWithinBiologicalRange(float level) const {
    return level >= pImpl->minLevel && level <= pImpl->maxLevel;
}

float Acetylcholine::getMinLevel() const {
    return pImpl->minLevel;
}

float Acetylcholine::getMaxLevel() const {
    return pImpl->maxLevel;
}

} // namespace nlm