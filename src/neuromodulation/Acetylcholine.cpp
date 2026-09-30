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
    float attentionModulator;
    float memoryConsolidationRate;
    float tone;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), attentionModulator(1.0f), memoryConsolidationRate(0.5f), tone(0.5f) {}
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
    // ACh enhances synaptic plasticity, particularly for memory consolidation
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::modulateAttention(float attentionLevel) {
    // ACh modulates attention and cortical excitability
    pImpl->level = std::min(1.0f, pImpl->level + attentionLevel * 0.5f);
}

void Acetylcholine::triggerMemoryConsolidation() {
    // ACh triggers memory consolidation processes
    if (pImpl->brain) {
        // Enhance synaptic connections that are active
        pImpl->level = std::min(1.0f, pImpl->level + pImpl->memoryConsolidationRate);
        
        // Update working memory based on ACh level
        if (pImpl->brain->getWorkingMemory()) {
            float weight = pImpl->level * 0.5f + 0.5f;
            pImpl->brain->getWorkingMemory()->setConsolidationBias(weight);
        }
        
        // Enhance episodic memory encoding
        if (pImpl->brain->getEpisodicMemory()) {
            pImpl->brain->getEpisodicMemory()->setAChEnhancement(pImpl->level);
        }
    }
}

void Acetylcholine::setAChTone(float tone) {
    pImpl->tone = std::clamp(tone, 0.0f, 1.0f);
    // Adjust baseline based on tone
    pImpl->baseline = pImpl->tone * 0.2f;
}

float Acetylcholine::getAChTone() const {
    return pImpl->tone;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm