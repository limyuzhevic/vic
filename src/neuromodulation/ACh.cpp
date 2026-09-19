#include "ACh.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionSensitivity;
    float memoryModulationStrength;
    float attentionGain;
    float memoryConsolidationFactor;
    float plasticityEnhancementFactor;
    
    Impl()
        : level(0.0f), baseline(0.05f), peak(1.0f)
        , decayRate(0.1f), releaseRate(1.0f)
        , attentionSensitivity(1.0f), memoryModulationStrength(1.0f)
        , attentionGain(1.0f), memoryConsolidationFactor(1.0f), plasticityEnhancementFactor(1.0f) {}
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
    // ACh modulates plasticity based on attention and memory processing
    // Higher ACh levels during attentive states enhance synaptic plasticity
    float attentionBasedPlasticity = pImpl->level * pImpl->attentionSensitivity;
    return 0.3f + 0.7f * attentionBasedPlasticity;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline with time constant influenced by decayRate
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update attention gain and memory consolidation factors
    pImpl->attentionGain = 1.0f + 2.0f * pImpl->level;
    pImpl->memoryConsolidationFactor = 1.0f + pImpl->level * pImpl->memoryModulationStrength;
    pImpl->plasticityEnhancementFactor = 1.0f + 1.5f * pImpl->level;
}

void Acetylcholine::signalAttention(float salience, float novelty) {
    // Phasic ACh release proportional to stimulus salience
    // Enhanced by novelty for orienting responses
    float phasicRelease = salience * pImpl->releaseRate * (1.0f + novelty);
    pImpl->level = std::min(pImpl->peak, pImpl->level + phasicRelease);
}

void Acetylcholine::signalMemoryEncoding(float memoryStrength) {
    // ACh release during successful memory encoding
    // Strengthens memory traces through plasticity enhancement
    float encodingBoost = memoryStrength * 0.5f * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + encodingBoost);
}

void Acetylcholine::signalMemoryRetrieval(float memoryStrength) {
    // ACh release during memory retrieval supports working memory maintenance
    float retrievalBoost = memoryStrength * 0.3f * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + retrievalBoost);
}

void Acetylcholine::signalWorkingMemoryUpdate(float activeMemorySpan) {
    // ACh modulates working memory capacity and updating
    float wmUpdate = activeMemorySpan * 0.4f * pImpl->releaseRate;
    pImpl->level = std::min(pImpl->peak, pImpl->level + wmUpdate);
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Acetylcholine::getMemoryConsolidationFactor() const {
    return pImpl->memoryConsolidationFactor;
}

float Acetylcholine::getPlasticityEnhancementFactor() const {
    return pImpl->plasticityEnhancementFactor;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->attentionGain = 1.0f;
    pImpl->memoryConsolidationFactor = 1.0f;
    pImpl->plasticityEnhancementFactor = 1.0f;
}

void Acetylcholine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 0.5f);
}

void Acetylcholine::setDecayRate(float decayRate) {
    pImpl->decayRate = std::clamp(decayRate, 0.01f, 1.0f);
}

void Acetylcholine::setReleaseRate(float releaseRate) {
    pImpl->releaseRate = std::clamp(releaseRate, 0.5f, 5.0f);
}

void Acetylcholine::setAttentionSensitivity(float sensitivity) {
    pImpl->attentionSensitivity = std::clamp(sensitivity, 0.1f, 5.0f);
}

void Acetylcholine::setMemoryModulationStrength(float strength) {
    pImpl->memoryModulationStrength = std::clamp(strength, 0.1f, 5.0f);
}

} // namespace nlm