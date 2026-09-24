#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionWeight;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), attentionWeight(1.0f) {}
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
    // ACh modulates attention and working memory consolidation
    // Higher ACh increases attention and memory strength
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: Decay towards baseline, peaks on attention-relevant stimuli
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // ACh peaks in response to attention-grabbing stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate);
}

void Acetylcholine::enhanceWorkingMemory(float memoryStrength) {
    // ACh enhances working memory storage and retrieval
    // Effect is proportional to current ACh level
    float enhancement = pImpl->level * memoryStrength * pImpl->attentionWeight;
    // In a full implementation, this would be passed to working memory systems
    // For now, we'll just log the enhancement
    if (enhancement > 0.01f) {
        // TODO: Pass to working memory system
        NLM_LOG_DEBUG("Acetylcholine enhanced working memory by " + std::to_string(enhancement));
    }
}

} // namespace nlm