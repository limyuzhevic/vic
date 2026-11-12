#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // ACh effects on attention and memory
    float attentionGain;     // Multiplier for attention signals
    float memoryConsolidation; // Factor for memory storage
    float arousal;          // General alertness level
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.05f), releaseRate(1.0f),
             attentionGain(1.0f), memoryConsolidation(1.0f), arousal(1.0f) {}
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
    
    // Update ACh effects based on level
    // Higher ACh increases attention and memory consolidation
    pImpl->attentionGain = 1.0f + pImpl->level * 2.0f;
    pImpl->memoryConsolidation = 1.0f + pImpl->level * 1.5f;
    pImpl->arousal = 1.0f + pImpl->level * 0.5f;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for memory-relevant synapses
    // Higher ACh increases the likelihood of LTP/LTD
    return 1.0f + pImpl->level * 0.8f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionEvent(float salience) {
    // ACh bursts in response to novel or salient stimuli
    // This is how ACh modulates attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + salience * pImpl->releaseRate * 2.0f);
}

void Acetylcholine::signalMemoryEncoding(const std::vector<float>& memoryStrengths) {
    // ACh promotes memory consolidation based on memory strength
    // Stronger memories trigger more ACh release
    float maxStrength = 0.0f;
    if (!memoryStrengths.empty()) {
        maxStrength = *std::max_element(memoryStrengths.begin(), memoryStrengths.end());
    }
    
    if (maxStrength > 0.1f) {
        // Enhance memory consolidation for strong memories
        pImpl->level = std::min(pImpl->peak, pImpl->level + maxStrength * 0.5f);
    }
}

} // namespace nlm