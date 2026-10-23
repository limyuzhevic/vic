#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Serotonin::Impl {
    class Brain* brain;
    float level;
    float mood;
    float impulsivity;
    float socialBehavior;
    float decayRate;
    float learningRate;
    float baselineLevel;
    float serotoninThreshold;
    std::vector<float> moodHistory;
    std::vector<float> socialHistory;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , mood(0.5f)
        , impulsivity(0.5f)
        , socialBehavior(0.5f)
        , decayRate(0.03f)
        , learningRate(0.05f)
        , baselineLevel(0.1f)
        , serotoninThreshold(0.2f)
    {
        // Initialize with baseline level
        level = baselineLevel;
        mood = 0.5f;
        impulsivity = 0.5f;
        socialBehavior = 0.5f;
    }
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "Serotonin";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::max(0.0f, level);
    
    // Update derived mood parameters based on serotonin level
    float normalizedLevel = pImpl->level / 2.0f;  // Assume max level around 2.0
    if (normalizedLevel > 0.5f) {
        pImpl->mood = 0.5f + (normalizedLevel - 0.5f) * 2.0f;
        pImpl->socialBehavior = 0.5f + (normalizedLevel - 0.5f) * 1.5f;
        pImpl->impulsivity = std::max(0.1f, 0.5f - (normalizedLevel - 0.5f) * 1.0f);
    } else {
        pImpl->mood = normalizedLevel * 2.0f;
        pImpl->socialBehavior = normalizedLevel * 1.5f;
        pImpl->impulsivity = 0.5f + (0.5f - normalizedLevel) * 1.0f;
    }
    
    NLM_LOG_INFO("Serotonin: Level set to " + std::to_string(pImpl->level) + 
                 ", mood: " + std::to_string(pImpl->mood) + 
                 ", social behavior: " + std::to_string(pImpl->socialBehavior) +
                 ", impulsivity: " + std::to_string(pImpl->impulsivity));
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity and learning
    float modulation = 1.0f;
    
    if (pImpl->level > pImpl->serotoninThreshold) {
        // High serotonin enhances certain types of learning
        modulation = 1.0f + (pImpl->level - pImpl->serotoninThreshold) * 0.5f;
    } else if (pImpl->level < pImpl->serotoninThreshold * 0.5f) {
        // Low serotonin reduces plasticity and may increase error-based learning
        modulation = 0.8f + (pImpl->level / pImpl->serotoninThreshold) * 0.2f;
    }
    
    return modulation;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay toward baseline
    float decayFactor = pImpl->decayRate * static_cast<float>(dt);
    float target = pImpl->baselineLevel;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * decayFactor;
    
    // Apply social behavior modulation
    if (pImpl->socialBehavior > 0.7f) {
        // High serotonin enhances social learning
        pImpl->level *= 1.02f;
    } else if (pImpl->impulsivity > 0.7f) {
        // High impulsivity may cause serotonin fluctuations
        float fluctuation = (std::rand() % 100) / 100.0f * 0.1f - 0.05f;
        pImpl->level += fluctuation;
    }
    
    // Cap at reasonable levels
    pImpl->level = std::min(pImpl->level, 2.0f);
    
    pImpl->moodHistory.push_back(pImpl->mood);
    pImpl->socialHistory.push_back(pImpl->socialBehavior);
}

void Serotonin::setMood(float mood) {
    pImpl->mood = std::max(0.0f, std::min(1.0f, mood));
    
    // Mood influences serotonin level
    pImpl->level += (mood - 0.5f) * 0.2f;
    NLM_LOG_INFO("Serotonin: Mood set to " + std::to_string(pImpl->mood) + 
                 ", new level: " + std::to_string(pImpl->level));
}

void Serotonin::setSocialBehavior(float socialBehavior) {
    pImpl->socialBehavior = std::max(0.0f, std::min(1.0f, socialBehavior));
    
    // Social behavior influences serotonin level
    pImpl->level += (socialBehavior - 0.5f) * 0.15f;
    NLM_LOG_INFO("Serotonin: Social behavior set to " + std::to_string(pImpl->socialBehavior) +
                 ", new level: " + std::to_string(pImpl->level));
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

const std::vector<float>& Serotonin::getMoodHistory() const {
    return pImpl->moodHistory;
}

const std::vector<float>& Serotonin::getSocialHistory() const {
    return pImpl->socialHistory;
}

void Serotonin::clearHistory() {
    pImpl->moodHistory.clear();
    pImpl->socialHistory.clear();
}

} // namespace nlm