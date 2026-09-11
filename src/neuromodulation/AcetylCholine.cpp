// AcetylCholine.cpp - Memory and attention modulation implementation

#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct AcetylCholine::Impl {
    class Brain* brain;
    float level;
    float memoryEnhancementFactor;
    float attentionFocusFactor;
    float baseline;
    float peak;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , memoryEnhancementFactor(0.5f)
        , attentionFocusFactor(0.8f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f) {}
};

AcetylCholine::AcetylCholine() : pImpl(new Impl) {}

AcetylCholine::~AcetylCholine() = default;

const char* AcetylCholine::getName() const {
    return "ACh";
}

float AcetylCholine::getLevel() const {
    return pImpl->level;
}

void AcetylCholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float AcetylCholine::getPlasticityFactor() const {
    // ACh enhances memory consolidation and attention
    return 0.5f + pImpl->memoryEnhancementFactor * pImpl->level;
}

void AcetylCholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

// Enhance memory consolidation
void AcetylCholine::enhanceMemoryConsolidation(Brain* brain, float consolidationStrength) {
    if (!brain || pImpl->level <= 0.0f) return;
    
    // Enhance episodic memory consolidation
    if (auto* episodicMem = brain->getEpisodicMemory()) {
        // Stronger consolidation with higher ACh levels
        float effectiveStrength = consolidationStrength * pImpl->attentionFocusFactor * pImpl->level;
        episodicMem->consolidate(std::min(1.0f, effectiveStrength));
    }
    
    // Enhance associative memory stabilization
    if (auto* assocMem = brain->getAssociativeMemory()) {
        // ACh stabilizes new associations
        for (size_t i = 0; i < assocMem->getAssociationCount(); ++i) {
            // Could add incremental strengthening here
        }
    }
}

// Enhance attention
void AcetylCholine::enhanceAttention(Brain* brain, float focusStrength) {
    if (!brain || pImpl->level <= 0.0f) return;
    
    // Enhance attentional selection system
    if (auto* attention = brain->getAttention()) {
        // Increase excitation strength
        float currentStrength = attention->getExcitationStrength();
        attention->setExcitationStrength(currentStrength + focusStrength * pImpl->attentionFocusFactor * pImpl->level);
    }
}

// Signal cholinergic event (e.g., novel stimulus, learning opportunity)
void AcetylCholine::signalNovelEvent(float novelty) {
    // Sudden increase in ACh level on novel events
    pImpl->level = std::min(pImpl->peak, pImpl->level + novelty * 0.5f);
}

} // namespace nlm
