#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// NeuromodulatorRegistry implementation
Neuromodulator* NeuromodulatorRegistry::getNeuromodulator(const std::string& name) {
    auto it = modulators.find(name);
    if (it != modulators.end()) {
        // Return pointer to the unique_ptr's object
        // Note: This is potentially unsafe if the unique_ptr is destroyed
        // In production code, you would need a safer approach
        return it->second.get();
    }
    return nullptr;
}

// Acetylcholine implementation
struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float modulationFactor;
    float attentionStrength;
    float memoryConsolidation;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             modulationFactor(0.9f), attentionStrength(0.5f), memoryConsolidation(0.7f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances plasticity for memory consolidation
    // More ACh -> better memory encoding and attention
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add biological noise
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    pImpl->level += dist(gen);
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Acetylcholine::signalAttention(float attentionStrength) {
    pImpl->attentionStrength = std::clamp(attentionStrength, 0.0f, 1.0f);
    
    // Boost ACh level for attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionStrength * 0.5f);
}

void Acetylcholine::signalMemory(float memoryStrength) {
    pImpl->memoryConsolidation = std::clamp(memoryStrength, 0.0f, 1.0f);
    
    // Enhance memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryStrength * 0.3f);
}

// Norepinephrine implementation
struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float modulationFactor;
    float arousalLevel;
    float stressLevel;
    
    Impl() : level(0.1f), baseline(0.05f), peak(1.0f), decayRate(0.03f), releaseRate(1.2f),
             modulationFactor(0.8f), arousalLevel(0.5f), stressLevel(0.3f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates arousal and attention
    // Moderate NE improves focus, high NE can cause stress and impair learning
    float arousal = pImpl->level;
    float optimalArousal = 0.5f;
    
    // Inverted U-shaped relationship with plasticity
    float plasticity = std::exp(-std::pow((arousal - optimalArousal) / 0.3f, 2.0f));
    
    // Apply stress penalty if too high
    if (pImpl->stressLevel > 0.7f) {
        plasticity *= (1.0f - (pImpl->stressLevel - 0.7f) * 2.0f);
    }
    
    return plasticity * pImpl->modulationFactor;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add noise for environmental sensitivity
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.05f, 0.05f);
    pImpl->level += dist(gen);
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Norepinephrine::signalArousal(float arousalLevel) {
    pImpl->arousalLevel = std::clamp(arousalLevel, 0.0f, 1.0f);
    
    // Boost NE for arousal
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousalLevel * 0.4f);
}

void Norepinephrine::signalStress(float stressLevel) {
    pImpl->stressLevel = std::clamp(stressLevel, 0.0f, 1.0f);
    
    // Stress increases NE release
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressLevel * 0.3f);
}

// Serotonin implementation
struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float modulationFactor;
    float mood;
    float sociality;
    
    Impl() : level(0.2f), baseline(0.1f), peak(1.0f), decayRate(0.04f), releaseRate(0.8f),
             modulationFactor(0.7f), mood(0.5f), sociality(0.5f) {}
};

Serotonin::Serotonin() : pImpl(new Impl) {}

Serotonin::~Serotonin() = default;

const char* Serotonin::getName() const {
    return "5-HT";
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin regulates mood and social behavior
    // Affects risk-taking, impulsivity, and learning
    float mood = pImpl->level;
    
    // Good mood improves learning and reduces avoidance
    float plasticity = 0.5f + 0.5f * mood;
    
    // High sociality enhances plasticity for social learning
    plasticity *= (1.0f + pImpl->sociality * 0.3f);
    
    return plasticity * pImpl->modulationFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add noise for emotional variability
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.08f, 0.08f);
    pImpl->level += dist(gen);
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Serotonin::signalMood(float mood) {
    pImpl->mood = std::clamp(mood, 0.0f, 1.0f);
    
    // Boost serotonin for positive mood
    pImpl->level = std::min(pImpl->peak, pImpl->level + mood * 0.3f);
}

void Serotonin::signalSocialBehavior(float sociality) {
    pImpl->sociality = std::clamp(sociality, 0.0f, 1.0f);
    
    // Social interactions boost serotonin
    pImpl->level = std::min(pImpl->peak, pImpl->level + sociality * 0.2f);
}

} // namespace nlm
