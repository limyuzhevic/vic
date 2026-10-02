#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float learningRate;
    float attentionModulation;
    float memoryConsolidationStrength;
    TimestepDuration lastUpdateTime;
    
    Impl() : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.1f), 
             releaseRate(1.5f), learningRate(0.05f), attentionModulation(0.0f),
             memoryConsolidationStrength(0.0f), lastUpdateTime(0) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    NLM_LOG_INFO("Acetylcholine system initialized");
}

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
    // ACh modulates plasticity based on attention
    // Higher ACh increases plasticity during attended tasks
    float attentionFactor = pImpl->attentionModulation;
    float baselinePlasticity = 0.5f;
    
    // Non-linear relationship: moderate ACh increases plasticity most
    return baselinePlasticity * (1.0f + attentionFactor * 1.5f);
}

void Acetylcholine::update(TimestepDuration dt) {
    if (dt <= 0) return;
    
    TimestepDuration timeSinceUpdate = dt - pImpl->lastUpdateTime;
    pImpl->lastUpdateTime = dt;
    
    // Decay towards baseline
    float decayFactor = 1.0f - pImpl->decayRate * static_cast<float>(timeSinceUpdate);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Adaptive learning
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Acetylcholine::signalAttention(float attentionStrength) {
    pImpl->attentionModulation = std::clamp(attentionStrength, 0.0f, 1.0f);
}

void Acetylcholine::signalMemoryConsolidation(float memoryStrength) {
    pImpl->memoryConsolidationStrength = std::clamp(memoryStrength, 0.0f, 1.0f);
    // ACh release facilitates memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryStrength * pImpl->releaseRate);
}

void Acetylcholine::updateAttention(float newAttention, TimestepDuration dt) {
    if (dt <= 0) return;
    
    // Smooth attention signal
    pImpl->attentionModulation += (newAttention - pImpl->attentionModulation) * 0.2f;
    pImpl->attentionModulation = std::clamp(pImpl->attentionModulation, 0.0f, 1.0f);
    
    // Update level based on attention
    float attentionEffect = pImpl->attentionModulation * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionEffect * pImpl->releaseRate);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionModulation = 0.0f;
    pImpl->memoryConsolidationStrength = 0.0f;
}

} // namespace nlm