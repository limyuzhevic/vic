// Acetylcholine: Attention and memory consolidation
// Implements ACh signaling for attention allocation and memory consolidation

#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionStrength;
    float memoryConsolidation;
    float workingMemoryBoost;
    
    Impl() 
        : level(0.0f), baseline(0.01f), peak(1.0f), decayRate(0.03f), releaseRate(1.5f)
        , attentionStrength(0.0f), memoryConsolidation(0.0f), workingMemoryBoost(0.0f) {}
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
    // ACh modulates plasticity: enhances LTP, reduces LTD
    // High ACh promotes stable memory formation
    float plasticityFactor = 1.0f - 0.3f * pImpl->level;
    
    // Apply memory consolidation modulation
    plasticityFactor *= (1.0f - pImpl->memoryConsolidation * 0.5f);
    
    return std::clamp(plasticityFactor, 0.5f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Decay attention and memory consolidation effects
    pImpl->attentionStrength *= 0.95f;
    pImpl->memoryConsolidation *= 0.9f;
    pImpl->workingMemoryBoost *= 0.9f;
}

void Acetylcholine::signalAttention(float attention) {
    // Signal attention allocation
    float burst = attention * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
    
    // Create attention modulation effect
    pImpl->attentionStrength += attention * 0.5f;
    
    NLM_LOG_INFO("Acetylcholine signaling attention: " + std::to_string(attention) + 
                " (level: " + std::to_string(pImpl->level) + ")");
}

void Acetylcholine::signalNovelty(float novelty) {
    // Signal novelty detection (increases attention)
    float noveltySignal = novelty * 0.8f * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + noveltySignal);
    
    // Novelty increases attention strength
    pImpl->attentionStrength += novelty * 0.3f;
}

void Acetylcholine::signalSensoryInput(float inputStrength) {
    // Signal sensory processing (increases ACh release)
    float sensorySignal = inputStrength * pImpl->releaseRate * 0.3f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + sensorySignal);
    
    // Enhance working memory
    pImpl->workingMemoryBoost += inputStrength * 0.2f;
}

float Acetylcholine::getAttentionStrength() const {
    return pImpl->attentionStrength;
}

float Acetylcholine::getMemoryConsolidation() const {
    return pImpl->memoryConsolidation;
}

float Acetylcholine::getWorkingMemoryBoost() const {
    return pImpl->workingMemoryBoost;
}

} // namespace nlm
