#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float decayRate;
    
    // Arousal components
    float arousalLevel;
    float stressLevel;
    float vigilance;
    float arousalThreshold;
    
    // Response characteristics
    float responseGain;
    float recoveryRate;
    float sensitization;
    
    // Temporal dynamics
    float arousalTime;
    float stressTime;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.1f),
             arousalLevel(0.0f), stressLevel(0.0f), vigilance(0.0f),
             arousalThreshold(0.5f), responseGain(1.0f), recoveryRate(0.9f),
             sensitization(0.0f), arousalTime(0.0f), stressTime(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(std::make_unique<Impl>()) {}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Norepinephrine::getPlasticityFactor() const {
    // NE enhances plasticity for arousing stimuli and facilitates learning under stress
    float arousalPlasticity = pImpl->arousalLevel * 1.2f;
    float stressPlasticity = pImpl->stressLevel * 0.8f;
    float vigilancePlasticity = pImpl->vigilance * 0.5f;
    float sensitizationPlasticity = pImpl->sensitization * 1.5f;
    
    // Combine arousal and stress components
    float totalPlasticity = arousalPlasticity + stressPlasticity + vigilancePlasticity + sensitizationPlasticity;
    
    // Saturate at reasonable levels
    return std::min(totalPlasticity, 3.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay arousal and stress signals
    pImpl->arousalLevel *= pImpl->recoveryRate;
    pImpl->stressLevel *= 0.8f;
    pImpl->vigilance *= 0.9f;
    pImpl->sensitization *= 0.95f;
    
    // Decay toward baseline
    float decay = 1.0f - pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decay;
}

void Norepinephrine::signalArousal(float arousal, float stress) {
    // Arousal and stress detection enhances NE release
    pImpl->arousalLevel = std::min(arousal, 1.0f);
    pImpl->stressLevel = std::min(stress, 1.0f);
    pImpl->arousalTime = 2.0f;  // Duration of arousal response
    
    // Update vigilance based on arousal
    pImpl->vigilance = std::min(pImpl->vigilance + arousal * 0.2f, 1.0f);
    
    // Sensitization for repeated stress
    if (pImpl->stressLevel > 0.5f) {
        pImpl->sensitization = std::min(pImpl->sensitization + 0.1f, 1.0f);
    }
    
    // Update response gain
    pImpl->responseGain = std::min(pImpl->responseGain + stress * 0.3f, 2.0f);
    
    // Update overall level
    pImpl->level += (arousal + stress * 0.5f) * pImpl->responseGain;
}

void Norepinephrine::setArousalThreshold(float threshold) {
    pImpl->arousalThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

Norepinephrine::~Norepinephrine() = default;

} // namespace nlm
