// Norepinephrine: Arousal and vigilance
// Implements NE signaling for arousal, stress response, and enhanced processing

#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float stressLevel;
    float processingSpeed;
    
    Impl() 
        : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.04f), releaseRate(1.8f)
        , arousalLevel(0.0f), stressLevel(0.0f), processingSpeed(1.0f) {}
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
    // NE modulates plasticity: enhances attention-based learning
    // High NE promotes rapid learning but also promotes forgetting
    float plasticityFactor = 1.0f + 0.5f * pImpl->level;
    
    // Apply stress modulation
    if (pImpl->stressLevel > 0.5f) {
        plasticityFactor *= 0.8f;  // Stress impairs learning
    }
    
    // Apply arousal modulation
    plasticityFactor *= (1.0f + pImpl->arousalLevel * 0.2f);
    
    return std::clamp(plasticityFactor, 0.5f, 2.5f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay arousal and stress effects
    pImpl->arousalLevel *= 0.9f;
    pImpl->stressLevel *= 0.95f;
    pImpl->processingSpeed *= 0.98f;
}

void Norepinephrine::signalArousal(float arousal) {
    // Signal arousal and alertness
    float burst = arousal * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Create arousal modulation effect
    pImpl->arousalLevel += arousal * 0.5f;
    
    NLM_LOG_INFO("Norepinephrine signaling arousal: " + std::to_string(arousal) + 
                " (level: " + std::to_string(pImpl->level) + ")");
}

void Norepinephrine::signalStress(float stress) {
    // Signal stress response
    float stressSignal = stress * pImpl->releaseRate * 0.7f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + stressSignal);
    
    // High stress increases stress level
    pImpl->stressLevel += stress * 0.3f;
    
    NLM_LOG_INFO("Norepinephrine signaling stress: " + std::to_string(stress) + 
                " (level: " + std::to_string(pImpl->level) + ")");
}

void Norepinephrine::signalWarning(float warning) {
    // Signal threat or warning
    float warningSignal = warning * pImpl->releaseRate * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + warningSignal);
    
    // Warning increases arousal and processing speed
    pImpl->arousalLevel += warning * 0.4f;
    pImpl->processingSpeed += warning * 0.2f;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

float Norepinephrine::getProcessingSpeed() const {
    return pImpl->processingSpeed;
}

} // namespace nlm
