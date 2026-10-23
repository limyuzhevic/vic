#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float arousal;
    float vigilance;
    float stress;
    float decayRate;
    float learningRate;
    float baselineLevel;
    float activationThreshold;
    float stressThreshold;
    std::vector<float> arousalHistory;
    std::vector<float> vigilanceHistory;
    std::vector<float> stressHistory;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , arousal(0.5f)
        , vigilance(0.5f)
        , stress(0.1f)
        , decayRate(0.04f)
        , learningRate(0.08f)
        , baselineLevel(0.05f)
        , activationThreshold(0.3f)
        , stressThreshold(0.8f)
    {
        // Initialize with baseline level
        level = baselineLevel;
        arousal = 0.5f;
        vigilance = 0.5f;
        stress = 0.1f;
    }
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "Norepinephrine";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::max(0.0f, level);
    
    // Update derived arousal parameters based on norepinephrine level
    float normalizedLevel = pImpl->level / 3.0f;  // Assume max level around 3.0
    if (normalizedLevel > 0.6f) {
        // High NE = high arousal and vigilance
        pImpl->arousal = 0.8f + (normalizedLevel - 0.6f) * 2.0f;
        pImpl->vigilance = 0.7f + (normalizedLevel - 0.6f) * 1.5f;
        if (normalizedLevel > 0.8f) {
            pImpl->stress = std::min(1.0f, (normalizedLevel - 0.8f) * 3.0f);
        } else {
            pImpl->stress = 0.0f;
        }
    } else {
        // Low NE = lower arousal and vigilance
        pImpl->arousal = normalizedLevel * 1.5f;
        pImpl->vigilance = normalizedLevel * 1.2f;
        pImpl->stress = 0.0f;
    }
    
    NLM_LOG_INFO("Norepinephrine: Level set to " + std::to_string(pImpl->level) + 
                 ", arousal: " + std::to_string(pImpl->arousal) + 
                 ", vigilance: " + std::to_string(pImpl->vigilance) +
                 ", stress: " + std::to_string(pImpl->stress));
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates plasticity in complex ways
    float modulation = 1.0f;
    
    if (pImpl->level > pImpl->activationThreshold) {
        // Optimal arousal enhances learning ( Yerkes-Dodson law)
        float arousalLevel = (pImpl->level - pImpl->activationThreshold) / 
                             (1.0f - pImpl->activationThreshold);
        
        if (arousalLevel < 0.5f) {
            // Moderate arousal enhances plasticity
            modulation = 1.0f + arousalLevel * 1.0f;
        } else {
            // High arousal can impair plasticity
            modulation = 2.0f - arousalLevel * 0.5f;
        }
        
        // Stress effects
        if (pImpl->stress > pImpl->stressThreshold) {
            // High stress reduces plasticity
            modulation *= 0.7f;
        }
    }
    
    return modulation;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay toward baseline
    float decayFactor = pImpl->decayRate * static_cast<float>(dt);
    float target = pImpl->baselineLevel;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * decayFactor;
    
    // External stimuli can increase norepinephrine
    if (pImpl->vigilance > 0.8f && pImpl->arousal > 0.8f) {
        // High vigilance and arousal can trigger sympathetic responses
        float increase = 0.01f * static_cast<float>(dt);
        pImpl->level = std::min(pImpl->level + increase, 3.0f);
        
        // This increases stress
        pImpl->stress = std::min(1.0f, pImpl->stress + increase * 0.5f);
    }
    
    // Stress feedback loop
    if (pImpl->stress > 0.5f) {
        // Stress increases norepinephrine
        pImpl->level *= 1.02f;
        pImpl->stress *= 0.95f;  // Stress decays slightly
    }
    
    // Cap at reasonable levels
    pImpl->level = std::min(pImpl->level, 3.0f);
    pImpl->stress = std::min(pImpl->stress, 1.0f);
    
    pImpl->arousalHistory.push_back(pImpl->arousal);
    pImpl->vigilanceHistory.push_back(pImpl->vigilance);
    pImpl->stressHistory.push_back(pImpl->stress);
}

void Norepinephrine::setArousal(float arousal) {
    pImpl->arousal = std::max(0.0f, std::min(1.0f, arousal));
    
    // Arousal influences norepinephrine level
    pImpl->level += (arousal - 0.5f) * 0.3f;
    NLM_LOG_INFO("Norepinephrine: Arousal set to " + std::to_string(pImpl->arousal) + 
                 ", new level: " + std::to_string(pImpl->level));
}

void Norepinephrine::setVigilance(float vigilance) {
    pImpl->vigilance = std::max(0.0f, std::min(1.0f, vigilance));
    
    // Vigilance influences norepinephrine level
    pImpl->level += (vigilance - 0.5f) * 0.25f;
    NLM_LOG_INFO("Norepinephrine: Vigilance set to " + std::to_string(pImpl->vigilance) +
                 ", new level: " + std::to_string(pImpl->level));
}

void Norepinephrine::setStress(float stress) {
    pImpl->stress = std::max(0.0f, std::min(1.0f, stress));
    
    // Stress directly increases norepinephrine
    pImpl->level += stress * 0.5f;
    NLM_LOG_INFO("Norepinephrine: Stress set to " + std::to_string(pImpl->stress) +
                 ", new level: " + std::to_string(pImpl->level));
}

float Norepinephrine::getArousal() const {
    return pImpl->arousal;
}

float Norepinephrine::getVigilance() const {
    return pImpl->vigilance;
}

float Norepinephrine::getStress() const {
    return pImpl->stress;
}

const std::vector<float>& Norepinephrine::getArousalHistory() const {
    return pImpl->arousalHistory;
}

const std::vector<float>& Norepinephrine::getVigilanceHistory() const {
    return pImpl->vigilanceHistory;
}

const std::vector<float>& Norepinephrine::getStressHistory() const {
    return pImpl->stressHistory;
}

void Norepinephrine::clearHistory() {
    pImpl->arousalHistory.clear();
    pImpl->vigilanceHistory.clear();
    pImpl->stressHistory.clear();
}

} // namespace nlm