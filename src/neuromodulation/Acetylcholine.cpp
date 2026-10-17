#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Memory consolidation parameters
    float memoryConsolidationRate;
    float workingMemoryBoost;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f),
             memoryConsolidationRate(0.5f), workingMemoryBoost(1.5f) {}
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
    // Acetylcholine modulates learning rates based on attention and memory needs
    float learningModulation = 1.0f;
    
    // High ACh increases neural excitability and learning
    learningModulation *= (1.0f + pImpl->level * 0.5f);
    
    return std::clamp(learningModulation, 0.5f, 3.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Implement cholinergic dynamics
    // ACh fluctuates with attention demands and memory processing
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Add attention-related fluctuations
    float attentionMod = std::sin(static_cast<float>(pImpl->rng.generate()) * 3.14f) * 0.05f;
    pImpl->level += attentionMod;
}

void Acetylcholine::signalAttention(float attentionLevel) {
    // Signal attention demands with cholinergic bursts
    float attentionSignal = attentionLevel * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal);
    
    // Apply to memory systems
    if (pImpl->brain) {
        auto* workingMemory = pImpl->brain->getWorkingMemory();
        if (workingMemory) {
            // ACh enhances working memory capacity
            float capacityBoost = 1.0f + pImpl->level * (pImpl->workingMemoryBoost - 1.0f);
            workingMemory->setCapacity(static_cast<size_t>(workingMemory->getCapacity() * capacityBoost));
        }
        
        auto* episodicMemory = pImpl->brain->getEpisodicMemory();
        if (episodicMemory) {
            // ACh promotes memory consolidation
            float consolidationFactor = 1.0f + pImpl->level * (pImpl->memoryConsolidationRate - 1.0f);
            episodicMemory->setConsolidationFactor(consolidationFactor);
        }
    }
}

void Acetylcholine::signalMemoryNeed(float memoryNeed) {
    // Signal memory processing needs with cholinergic responses
    float needSignal = memoryNeed * pImpl->releaseRate * 0.5f;
    pImpl->level = std::min(pImpl->peak, pImpl->level + needSignal);
    
    // Enhance attention to memory-related stimuli
    if (pImpl->brain) {
        auto* attention = pImpl->brain->getAttention();
        if (attention) {
            attention->setInhibitionStrength(0.5f * (1.0f - pImpl->level));
            attention->setExcitationStrength(1.5f * (1.0f + pImpl->level));
        }
    }
}

void Acetylcholine::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

float Acetylcholine::getMemoryConsolidationFactor() const {
    return 1.0f + pImpl->level * (pImpl->memoryConsolidationRate - 1.0f);
}

float Acetylcholine::getAttentionModulationFactor() const {
    return 1.0f + pImpl->level * 0.5f;
}

} // namespace nlm
