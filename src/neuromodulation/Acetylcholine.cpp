// Acetylcholine: Attention and memory consolidation neuromodulator
// Biological role: Enhances attention, facilitates working memory, promotes memory consolidation

#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionBoost;
    float memoryConsolidation;
    float decayRate;
    float peak;
    
    // Attention state
    float currentAttention;
    float attentionTarget;
    
    // Memory state
    float consolidationIndex;
    float recentActivity;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , attentionBoost(0.8f)
        , memoryConsolidation(0.6f)
        , decayRate(0.1f)
        , peak(1.0f)
        , currentAttention(0.5f)
        , attentionTarget(0.5f)
        , consolidationIndex(0.0f)
        , recentActivity(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

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
    // ACh enhances memory-related plasticity
    return 0.5f + 0.5f * pImpl->level * pImpl->memoryConsolidation;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Update attention
    pImpl->attentionTarget = 0.5f + 0.5f * pImpl->level;  // Higher ACh = more attention
    pImpl->currentAttention += (pImpl->attentionTarget - pImpl->currentAttention) * 0.1f;
    
    // Update memory consolidation
    pImpl->consolidationIndex = std::min(1.0f, pImpl->consolidationIndex + pImpl->level * 0.05f);
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float targetRegionActivation) {
    // Boost attention to regions with high activation
    if (pImpl->brain) {
        float attentionBoost = targetRegionActivation * pImpl->attentionBoost;
        pImpl->currentAttention += attentionBoost * 0.1f;
        pImpl->currentAttention = std::min(1.0f, pImpl->currentAttention);
        
        // Store in brain for integration
        if (auto* config = pImpl->brain->getConfig()) {
            config->set("attention_boost", pImpl->currentAttention);
        }
    }
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->currentAttention;
}

void Acetylcholine::markForConsolidation(NeuronId neuronId) {
    // Mark neuron for memory consolidation
    if (pImpl->brain) {
        pImpl->consolidationIndex = std::min(1.0f, pImpl->consolidationIndex + 0.01f);
        
        // In a real implementation, this would schedule synaptic strengthening
        // For now, just store in config
        if (auto* config = pImpl->brain->getConfig()) {
            std::string key = "consolidation_neuron_" + std::to_string(neuronId.value);
            config->set(key, pImpl->consolidationIndex);
        }
    }
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
    pImpl->currentAttention = 0.5f;
    pImpl->attentionTarget = 0.5f;
    pImpl->consolidationIndex = 0.0f;
    pImpl->recentActivity = 0.0f;
}

} // namespace nlm