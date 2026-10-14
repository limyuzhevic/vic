#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulation;
    float memoryEncodingBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             attentionModulation(0.0f), memoryEncodingBoost(0.0f) {}
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
    
    void Acetylcholine::setBaseline(float baseline) {
        pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
    }
    
    float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention and memory tasks
    return 0.5f + 0.5f * pImpl->level * 1.5f;  // Stronger modulation
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    // Decay modulation factors
    pImpl->attentionModulation *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
    pImpl->memoryEncodingBoost *= (1.0f - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float focusStrength) {
    pImpl->attentionModulation = std::min(1.0f, focusStrength * pImpl->level);
}

void Acetylcholine::enhanceMemoryEncoding(float encodingStrength) {
    pImpl->memoryEncodingBoost = std::min(1.0f, encodingStrength * pImpl->level);
}

void Acetylcholine::reduceInterference(float reductionFactor) {
    // ACh reduces interference between memory traces
    // This affects working memory capacity and stability
    if (pImpl->attentionModulation > 0.0f) {
        pImpl->attentionModulation *= (1.0f - reductionFactor);
    }
}

void Acetylcholine::promoteWorkingMemoryConsolidation(float factor) {
    // ACh promotes consolidation of working memory into episodic memory
    // Modulates synaptic strengthening processes
    if (factor > 0.0f) {
        // Enhance memory encoding through cholinergic mechanisms
    }
}

void Acetylcholine::promoteEpisodicMemoryConsolidation(float factor) {
    // ACh enhances episodic memory consolidation
    // Supports long-term potentiation processes
    if (factor > 0.0f) {
        // Promote hippocampal-cortical interactions
    }
}

} // namespace nlm