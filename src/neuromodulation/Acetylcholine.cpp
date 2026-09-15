#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float memoryStrength;
    float consolidation;
    float attentionSignal;
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , memoryStrength(0.5f), consolidation(0.0f), attentionSignal(0.0f) {}
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
    // Acetylcholine enhances plasticity for attention-focused learning
    // Boosts memory encoding and consolidation
    float basePlasticity = 0.5f;
    float levelFactor = pImpl->level * 1.5f;
    float memoryFactor = pImpl->memoryStrength * 0.4f;
    float consolidationFactor = pImpl->consolidation * 0.3f;
    float attentionFactor = pImpl->attentionSignal * 0.5f;
    
    // ACh significantly increases plasticity for attended/stimulated events
    float result = basePlasticity + levelFactor + memoryFactor + consolidationFactor + attentionFactor;
    result = std::max(0.2f, std::min(2.0f, result));
    NLM_LOG_DEBUG("Acetylcholine plasticity factor: " + std::to_string(result));
    return result;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Memory consolidation and attention decay
    pImpl->consolidation = std::max(0.0f, pImpl->consolidation - 0.02f * static_cast<float>(dt));
    pImpl->attentionSignal = std::max(0.0f, pImpl->attentionSignal - 0.05f * static_cast<float>(dt));
    
    NLM_LOG_DEBUG("Acetylcholine update: level=" + std::to_string(pImpl->level) + ", memory_strength=" + std::to_string(pImpl->memoryStrength));
}

void Acetylcholine::signalMemoryEncoding(float strength) {
    // Memory encoding signal (attention to new information)
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->releaseRate * 0.8f);
    pImpl->memoryStrength = std::min(1.0f, pImpl->memoryStrength + strength * 0.15f);
    NLM_LOG_INFO("Acetylcholine memory encoding: " + std::to_string(strength));
}

void Acetylcholine::signalMemoryConsolidation(float consolidation) {
    // Memory consolidation signal (stabilizing memories)
    pImpl->level = std::min(pImpl->peak, pImpl->level + consolidation * pImpl->releaseRate * 0.6f);
    pImpl->consolidation = std::min(1.0f, pImpl->consolidation + consolidation * 0.1f);
    NLM_LOG_INFO("Acetylcholine consolidation: " + std::to_string(consolidation));
}

void Acetylcholine::signalAttention(float attention) {
    // Attention signaling (focusing on stimuli)
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate * 0.9f);
    pImpl->attentionSignal = std::min(1.0f, pImpl->attentionSignal + attention * 0.2f);
    NLM_LOG_INFO("Acetylcholine attention: " + std::to_string(attention));
}

} // namespace nlm