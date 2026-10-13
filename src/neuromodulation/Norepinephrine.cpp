#include "Norepinephrine.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // NE effects on arousal and learning
    float arousalLevel;
    float learningRateBoost;
    float attentionBroadening;
    
    // Stress response
    float stressLevel;
    float recoveryRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f)
           , arousalLevel(0.5f), learningRateBoost(0.2f), attentionBroadening(0.1f)
           , stressLevel(0.0f), recoveryRate(0.05f) {}
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
    // NE modulates arousal, learning rate, and attention breadth
    // Higher NE increases arousal and learning rate
    return 1.0f + pImpl->level * pImpl->learningRateBoost;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Stress recovery
    pImpl->stressLevel = std::max(0.0f, pImpl->stressLevel - pImpl->recoveryRate * static_cast<float>(dt));
    
    // Modulate attention broadening based on NE level
    // Higher NE = broader attention (more distractible)
    pImpl->attentionBroadening = 0.1f + pImpl->level * 0.3f;
}

void Norepinephrine::signalArousal(float arousal) {
    // Boost arousal level
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->releaseRate);
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + arousal * 0.5f);
}

void Norepinephrine::signalStress(float stress) {
    // Increase stress level (affects learning and memory)
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + stress);
    // Stress can temporarily boost NE level
    pImpl->level = std::min(pImpl->peak, pImpl->level + stress * 0.5f);
}

void Norepinephrine::signalRecovery() {
    // Recovery from stress/arousal
    pImpl->stressLevel = 0.0f;
    pImpl->arousalLevel = 0.5f;
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalLevel;
}

float Norepinephrine::getLearningRateBoost() const {
    return pImpl->learningRateBoost;
}

float Norepinephrine::getAttentionBroadening() const {
    return pImpl->attentionBroadening;
}

float Norepinephrine::getStressLevel() const {
    return pImpl->stressLevel;
}

} // namespace nlm