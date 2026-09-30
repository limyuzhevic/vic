#include "Acetylcholine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention
    float attentionLevel;
    float arousalLevel;
    
    // Memory consolidation
    bool consolidationActive;
    float consolidationFactor;
    float retrievalBoost;
    
    // Sensory enhancement
    float sensoryEnhancement;
    float corticalExcitability;
    
    Impl() 
        : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), releaseRate(1.5f)
        , attentionLevel(0.0f), arousalLevel(0.0f)
        , consolidationActive(false), consolidationFactor(1.0f), retrievalBoost(1.0f)
        , sensoryEnhancement(0.0f), corticalExcitability(1.0f) {}
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
    
    // Update attention and arousal
    pImpl->attentionLevel = 0.3f + 0.7f * pImpl->level;
    pImpl->arousalLevel = 0.2f + 0.6f * pImpl->level;
    
    // Enhance sensory processing
    pImpl->sensoryEnhancement = 0.5f + 0.5f * pImpl->level;
    pImpl->corticalExcitability = 1.0f + 0.3f * pImpl->level;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh increases learning rate and plasticity, especially for novel experiences
    return 1.0f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Gradual decay
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // If consolidation is active, maintain elevated levels for memory consolidation
    if (pImpl->consolidationActive) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.01f);
        if (pImpl->level > 0.8f) {
            pImpl->consolidationActive = false;
        }
    }
}

void Acetylcholine::signalAttention(float attentionSignal) {
    pImpl->attentionLevel = std::clamp(attentionSignal, 0.0f, 1.0f);
    // Boost ACh level for attentional focus
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate * 0.5f);
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attentionLevel;
}

void Acetylcholine::initiateMemoryConsolidation() {
    pImpl->consolidationActive = true;
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f);
}

float Acetylcholine::getMemoryConsolidationFactor() const {
    return pImpl->consolidationFactor * (1.0f + 0.3f * pImpl->level);
}

void Acetylcholine::enhanceSensoryProcessing(float enhancementFactor) {
    pImpl->sensoryEnhancement = std::clamp(enhancementFactor, 0.0f, 2.0f);
    // Boost ACh for enhanced processing
    pImpl->level = std::min(pImpl->peak, pImpl->level + enhancementFactor * 0.2f);
}

float Acetylcholine::getSensoryEnhancement() const {
    return pImpl->sensoryEnhancement;
}

} // namespace nlm
