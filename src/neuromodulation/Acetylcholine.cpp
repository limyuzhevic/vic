#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryBoost;
    
    // For memory consolidation tracking
    float memoryConsolidationWindow;
    float lastConsolidationStep;
    
    Impl() 
        : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.5f),
          attentionGain(1.0f), memoryBoost(1.0f), memoryConsolidationWindow(1000.0f), lastConsolidationStep(0.0f) {}
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
    // Acetylcholine enhances attention and memory consolidation
    // at moderate levels, can cause saturation at high levels
    return 1.0f + pImpl->attentionGain * pImpl->level - pImpl->memoryBoost * pImpl->level * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    float decayAmount = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decayAmount);
}

void Acetylcholine::signalNovelty() {
    // ACh increases with novel sensory input
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f * pImpl->releaseRate);
}

void Acetylcholine::signalAttentionShift(NeuronId neuronId) {
    // Focused attention on specific neural populations
    // This would be used by attentional selection systems
    // For now, just note that attention occurred
    (void)neuronId; // Avoid unused parameter
}

void Acetylcholine::consolidateMemory(const std::vector<float>& pattern, float strength) {
    // ACh enhances memory consolidation
    if (pImpl->level > 0.3f) {
        float boostFactor = 1.0f + pImpl->memoryBoost * pImpl->level;
        // In a real implementation, this would enhance synaptic strengthening
        // of memory-related pathways
        (void)pattern;
        (void)strength;
    }
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain * pImpl->level;
}

float Acetylcholine::getMemoryBoost() const {
    return pImpl->memoryBoost;
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::max(0.0f, gain);
}

void Acetylcholine::setMemoryBoost(float boost) {
    pImpl->memoryBoost = std::max(0.0f, boost);
}

} // namespace nlm
