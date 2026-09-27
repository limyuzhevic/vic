#include "Acetylcholine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;           // ACh concentration (0-1)
    float baseline;        // Baseline level
    float peak;            // Peak level
    float decayRate;       // How fast ACh decays
    float releaseRate;     // How fast ACh increases
    float attentionGain;   // Effect on attention amplification
    float memoryGain;      // Effect on memory consolidation
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.2f), releaseRate(1.0f),
             attentionGain(1.5f), memoryGain(1.3f) {}
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
    // ACh enhances learning when at moderate levels (e.g., during encoding)
    // Very high or very low ACh impairs plasticity
    if (pImpl->level < 0.3f || pImpl->level > 0.7f) {
        return 0.8f;  // Reduced plasticity at extremes
    }
    return 1.2f;  // Enhanced plasticity at optimal levels
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: baseline with phasic bursts
    float targetLevel = pImpl->baseline;
    
    // Decay towards baseline
    pImpl->level = std::max(targetLevel, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply attention modulation factor based on ACh level
    // Higher ACh increases attentional gain
    attentionFactor_ = 1.0f + (pImpl->level * pImpl->attentionGain * 0.5f);
    
    // Apply memory consolidation factor based on ACh level
    // ACh has inverted-U effect on memory consolidation
    float memoryModulation = 1.0f;
    if (pImpl->level > 0.1f && pImpl->level < 0.9f) {
        memoryModulation = 1.0f + pImpl->memoryGain * (1.0f - std::abs(0.5f - pImpl->level) * 2.0f);
    }
    memoryConsolidationFactor_ = memoryModulation;
}

void Acetylcholine::signalAttentionBoost(float boost) {
    // ACh burst in response to novel or important stimuli
    setLevel(std::min(pImpl->peak, getLevel() + boost));
}

void Acetylcholine::signalMemoryEncoding(float encodingStrength) {
    // ACh release during learning signals memory encoding
    // Higher encoding strength produces larger ACh burst
    float burst = encodingStrength * pImpl->releaseRate * 0.5f;
    setLevel(std::min(pImpl->peak, getLevel() + burst));
}

float Acetylcholine::getAttentionFactor() const {
    return attentionFactor_;
}

float Acetylcholine::getMemoryConsolidationFactor() const {
    return memoryConsolidationFactor_;
}

} // namespace nlm