#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Serotonin::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float reuptakeRate;
    float synthesisRate;
    float mood;
    float impulsivity;
    float socialBehavior;
    SimulationStep lastSynthesized;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             reuptakeRate(0.5f), synthesisRate(0.05f), mood(0.5f), impulsivity(0.5f),
             socialBehavior(0.5f), lastSynthesized(0) {}
};

Serotonin::Serotonin()
    : pImpl(new Impl)
{
    NLM_LOG_INFO("Serotonin neuromodulator initialized");
}

Serotonin::~Serotonin() {
    NLM_LOG_INFO("Serotonin neuromodulator destroyed");
}

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
    // Enhanced serotonin: modulates plasticity based on mood and social behavior
    float basePlasticity = 0.5f;
    float moodFactor = 0.5f + 0.5f * pImpl->mood;
    float socialFactor = 0.8f + 0.4f * pImpl->socialBehavior;
    float levelFactor = 0.5f + 0.5f * pImpl->level;
    
    return basePlasticity * moodFactor * socialFactor * levelFactor;
}

void Serotonin::update(TimestepDuration dt) {
    // Dynamic serotonin dynamics with synthesis and reuptake
    float timeFactor = static_cast<float>(dt) / 100.0f;
    
    // Natural decay and reuptake
    pImpl->level -= pImpl->decayRate * timeFactor;
    pImpl->level -= pImpl->reuptakeRate * timeFactor * pImpl->level;
    
    // Spontaneous synthesis (constant baseline)
    pImpl->level += pImpl->synthesisRate * timeFactor;
    
    // Ensure level stays in valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
    
    // Update related state variables
    pImpl->mood = std::max(0.0f, pImpl->mood - 0.05f * timeFactor);
    pImpl->impulsivity = std::max(0.0f, pImpl->impulsivity - 0.03f * timeFactor);
    pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + 0.01f * timeFactor);
}

void Serotonin::signalMoodChange(float moodDelta) {
    pImpl->mood += moodDelta;
    pImpl->mood = std::clamp(pImpl->mood, 0.0f, 1.0f);
    
    // High serotonin enhances social behavior
    if (pImpl->mood > 0.7f) {
        pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + 0.1f);
    }
}

void Serotonin::signalImpulsivityChange(float impulsivityDelta) {
    pImpl->impulsivity += impulsivityDelta;
    pImpl->impulsivity = std::clamp(pImpl->impulsivity, 0.0f, 1.0f);
}

float Serotonin::getMood() const {
    return pImpl->mood;
}

float Serotonin::getImpulsivity() const {
    return pImpl->impulsivity;
}

float Serotonin::getSocialBehavior() const {
    return pImpl->socialBehavior;
}

void Serotonin::enhanceSocialLearning(float socialExposure) {
    // Serotonin specifically enhances social learning and pattern recognition
    pImpl->socialBehavior = std::min(1.0f, pImpl->socialBehavior + 0.2f * socialExposure);
    pImpl->mood = std::min(1.0f, pImpl->mood + 0.1f * socialExposure);
    
    // Increase plasticity for social learning
    float plasticityBoost = 0.3f * socialExposure;
    setLevel(getLevel() + plasticityBoost);
}

void Serotonin::enhancePatternRecognition(float novelty) {
    // Serotonin enhances recognition of novel patterns
    pImpl->level = std::min(1.0f, pImpl->level + 0.15f * novelty);
    pImpl->mood = std::min(1.0f, pImpl->mood + 0.05f * novelty);
    
    // Improved novelty detection
    signalMoodChange(0.05f * novelty);
}

} // namespace nlm