#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention-related parameters
    float attentionGain;      // Multiplier for attention processes
    float memoryConsolidationRate;  // Rate at which memories are consolidated
    float encodingEfficiency; // Efficiency of new memory encoding
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             attentionGain(1.0f), memoryConsolidationRate(1.0f), encodingEfficiency(1.0f) {}
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
    
    // Update attention-related parameters based on level
    pImpl->attentionGain = 0.5f + pImpl->level * 2.0f;           // High ACh = high attention gain
    pImpl->memoryConsolidationRate = 0.5f + pImpl->level * 1.5f; // High ACh = faster consolidation
    pImpl->encodingEfficiency = 0.8f + pImpl->level * 0.4f;       // High ACh = better encoding
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine modulates plasticity for memory stability
    // High ACh promotes memory consolidation (reduced plasticity for new learning)
    return 1.0f - pImpl->level * 0.3f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // TODO: Implement real acetylcholine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionShift(bool focusedAttention) {
    // TODO: Implement attention shift signaling
    // PLACEHOLDER: Burst of acetylcholine on attention shifts
    if (focusedAttention) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f * pImpl->releaseRate);
    }
}

void Acetylcholine::signalMemoryEncoding() {
    // TODO: Implement memory encoding signaling
    // PLACEHOLDER: Brief increase in acetylcholine during encoding
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f);
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Acetylcholine::getMemoryConsolidationRate() const {
    return pImpl->memoryConsolidationRate;
}

float Acetylcholine::getEncodingEfficiency() const {
    return pImpl->encodingEfficiency;
}