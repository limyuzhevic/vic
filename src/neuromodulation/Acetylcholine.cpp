#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             attentionGain(1.0f), memoryStrength(1.0f) {}
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
    // ACh modulates both attention and memory consolidation
    // Higher ACh increases plasticity for attended items
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Modulate attention and memory strength based on level
    // ACh has dual role: attention enhancement and memory strengthening
}

void Acetylcholine::enhanceAttention() {
    // Increase attention gain
    pImpl->attentionGain = 1.0f + pImpl->level * 2.0f;
    
    // This effect would be implemented in Brain integration
    // Enhanced attention to sensory inputs and working memory
}

void Acetylcholine::modulateWorkingMemory(float influence) {
    // Modulate working memory trace stability
    // Higher ACh makes working memory traces more stable
    // This is a parameter that would be used in Brain
}

void Acetylcholine::facilitateMemoryConsolidation() {
    // Promote long-term memory consolidation
    // ACh signals are important for memory consolidation during sleep/rest
    // This effect would be integrated in Brain memory systems
}

void Acetylcholine::enhanceMemoryEncoding(float strength) {
    // Enhance encoding of new memories into episodic memory
    // ACh facilitates new learning and memory formation
}

void Acetylcholine::improveMemoryRetrieval(float strength) {
    // Enhance retrieval of stored memories
    // ACh improves memory access and recall
}

void Acetylcholine::reduceMemoryInterference(float strength) {
    // Reduce interference between memory traces
    // ACh helps maintain distinct memory representations
}

} // namespace nlm