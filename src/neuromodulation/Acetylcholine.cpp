#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionWeight;
    float memoryConsolidationWeight;
    float decayRate;
    float activationThreshold;
    float maxLevel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , attentionWeight(1.0f)
        , memoryConsolidationWeight(0.8f)
        , decayRate(0.05f)
        , activationThreshold(0.2f)
        , maxLevel(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, pImpl->maxLevel);
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine modulates attention and memory consolidation
    // Higher ACh increases selective attention and memory stabilization
    float factor = 1.0f;
    
    // Attention modulation: stronger attention when ACh is high
    if (pImpl->level > pImpl->activationThreshold) {
        factor += (pImpl->level - pImpl->activationThreshold) * pImpl->attentionWeight;
    }
    
    // Memory consolidation: ACh promotes consolidation of important memories
    if (pImpl->level > 0.5f) {
        factor += (pImpl->level - 0.5f) * pImpl->memoryConsolidationWeight;
    }
    
    return factor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline (slow recovery)
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float intensity) {
    // Respond to attention-grabbing stimuli
    setLevel(std::min(pImpl->level + intensity * 0.5f, pImpl->maxLevel));
}

void Acetylcholine::signalMemoryEncoding(const std::vector<NeuronId>& activeNeurons) {
    // During memory encoding, ACh modulates synaptic strength
    if (brain_ && !activeNeurons.empty()) {
        // In a real implementation, this would modulate synaptic plasticity
        // for the active neurons involved in memory encoding
        // For now, it's a placeholder that tracks the activity
    }
}

void Acetylcholine::updateMemoryConsolidation() {
    // Promote consolidation of recent experiences when level is high
    if (pImpl->level > 0.7f && pImpl->brain) {
        // In a full implementation, this would trigger replay of recent
        // experiences to strengthen important memories
    }
}

} // namespace nlm