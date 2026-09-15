#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Norepinephrine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalLevel;
    float stressLevel;
    float vigilance;
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , arousalLevel(0.5f), stressLevel(0.0f), vigilance(0.0f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {
    NLM_LOG_INFO("Norepinephrine system initialized");
}

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
    // Norepinephrine increases plasticity for salient events (learning focus)
    float basePlasticity = 0.5f;
    float levelFactor = pImpl->level * 0.8f;
    float arousalFactor = pImpl->arousalLevel * 0.3f;
    float stressFactor = pImpl->stressLevel * 0.1f;
    float vigilanceFactor = pImpl->vigilance * 0.2f;
    
    // High norepinephrine increases plasticity for attention-grabbing events
    float result = basePlasticity + levelFactor + arousalFactor - stressFactor + vigilanceFactor;
    result = std::max(0.2f, std::min(1.5f, result));
    NLM_LOG_DEBUG("Norepinephrine plasticity factor: " + std::to_string(result));
    return result;
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Arousal and vigilance decay
    pImpl->arousalLevel = std::max(0.0f, pImpl->arousalLevel - 0.05f * static_cast<float>(dt));
    pImpl->vigilance = std::max(0.0f, pImpl->vigilance - 0.03f * static_cast<float>(dt));
    
    NLM_LOG_DEBUG("Norepinephrine update: level=" + std::to_string(pImpl->level) + ", arousal=" + std::to_string(pImpl->arousalLevel));
}

void Norepinephrine::signalVigilance(float attention) {
    // Vigilance signaling (attention to environment)
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate * 0.7f);
    pImpl->vigilance = std::min(1.0f, pImpl->vigilance + attention * 0.2f);
    NLM_LOG_INFO("Norepinephrine vigilance: " + std::to_string(attention));
}

void Norepinephrine::signalStress(float stress) {
    // Stress signaling (threat response)
    pImpl->level = std::min(pImpl->peak, pImpl->level + stress * pImpl->releaseRate * 1.2f);
    pImpl->stressLevel = std::min(1.0f, pImpl->stressLevel + stress * 0.1f);
    NLM_LOG_INFO("Norepinephrine stress signal: " + std::to_string(stress));
}

void Norepinephrine::signalFocus(float focus) {
    // Focus signaling (directed attention)
    pImpl->level = std::min(pImpl->peak, pImpl->level + focus * pImpl->releaseRate * 0.9f);
    pImpl->arousalLevel = std::min(1.0f, pImpl->arousalLevel + focus * 0.15f);
    NLM_LOG_INFO("Norepinephrine focus: " + std::to_string(focus));
}

} // namespace nlm