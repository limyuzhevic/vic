#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baselineLevel;
    float attentionModulator;
    float memoryConsolidationStrength;
    float arousalState;
    std::vector<float> attentionWeights;
};

Acetylcholine::Acetylcholine() : pImpl(new Impl()) {
    pImpl->level = 0.0f;
    pImpl->baselineLevel = 0.05f;
    pImpl->attentionModulator = 1.0f;
    pImpl->memoryConsolidationStrength = 1.0f;
    pImpl->arousalState = 0.5f;
}

Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine (ACh)";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    
    // ACh modulates attention - higher ACh = better focus
    pImpl->attentionModulator = 0.5f + pImpl->level * 2.0f;
    
    // ACh modulates memory consolidation - cholinergic signaling enhances consolidation
    pImpl->memoryConsolidationStrength = 0.5f + pImpl->level * 2.0f;
    
    // ACh affects arousal and vigilance
    pImpl->arousalState = pImpl->level;
    
    NLM_LOG_DEBUG("Acetylcholine level set to " + std::to_string(level) + 
                  ", attention: " + std::to_string(pImpl->attentionModulator) +
                  ", consolidation: " + std::to_string(pImpl->memoryConsolidationStrength));
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention-related learning
    return pImpl->attentionModulator * pImpl->memoryConsolidationStrength;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Simple decay towards baseline
    if (pImpl->level > pImpl->baselineLevel) {
        pImpl->level -= 0.1f * dt;
        if (pImpl->level < pImpl->baselineLevel) {
            pImpl->level = pImpl->baselineLevel;
        }
    }
}

void Acetylcholine::signalAttentionBoost() {
    setLevel(std::min(pImpl->level + 0.2f, 1.0f));
}

void Acetylcholine::signalNoveltyDetection(float novelty) {
    // Novelty boosts ACh release
    if (novelty > 0.3f) {
        setLevel(std::min(pImpl->level + novelty * 0.5f, 1.0f));
    }
}

void Acetylcholine::enhanceMemoryConsolidation(const std::vector<NeuronId>& activeNeurons, 
                                              float strength) {
    // ACh signals trigger memory consolidation
    float consolidationBoost = strength * pImpl->memoryConsolidationStrength * pImpl->attentionModulator;
    NLM_LOG_INFO("ACh-enhanced memory consolidation: " + std::to_string(consolidationBoost * 100) + "% strength");
}

} // namespace nlm