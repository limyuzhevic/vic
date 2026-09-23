#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionWeight;
    float memoryWeight;
    float decayRate;
    float consolidationThreshold;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , attentionWeight(1.0f)
        , memoryWeight(0.5f)
        , decayRate(0.1f)
        , consolidationThreshold(0.7f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Acetylcholine levels influence attention and memory consolidation
    // Higher ACh = better attention focus = enhanced memory encoding
    if (pImpl->brain && pImpl->level > pImpl->consolidationThreshold) {
        // Enhance memory consolidation
        if (pImpl->brain->getEpisodicMemory()) {
            // Boost recent memory consolidation
            pImpl->brain->getEpisodicMemory()->boostConsolidation(pImpl->level * 0.5f);
        }
        
        // Enhance attention system
        if (pImpl->brain->getAttention()) {
            // ACh increases attentional focus
            pImpl->brain->getAttention()->setFocusStrength(pImpl->level);
        }
        
        // Increase neural excitability
        for (const auto& region : pImpl->brain->getRegions()) {
            for (auto* neuron : region->getAllNeurons()) {
                // ACh increases neuronal excitability by reducing threshold
                float excitabilityBoost = pImpl->level * 2.0f;
                if (excitabilityBoost > 0.0f) {
                    neuron->injectCurrent(excitabilityBoost);
                }
            }
        }
    }
    
    // Natural decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity, especially for attention-related learning
    return 0.8f + 0.4f * pImpl->level;  // Range: [0.8, 1.2]
}

void Acetylcholine::signalAttention(float novelty, TimestepDuration dt) {
    // ACh responds to novel stimuli and increases alertness
    float attentionResponse = std::min(1.0f, novelty * 2.0f);
    pImpl->level = std::min(1.0f, pImpl->level + attentionResponse * 0.3f);
}

void Acetylcholine::enhanceMemoryConsolidation(float strength, TimestepDuration dt) {
    // Boost memory consolidation based on attention
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.2f);
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
}

} // namespace nlm