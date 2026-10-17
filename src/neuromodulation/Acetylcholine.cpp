#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float decayRate;
    
    // Attention components
    float attentionLevel;
    float noveltySignal;
    float focusDuration;
    float attentionThreshold;
    
    // Memory modulation
    float memoryConsolidationRate;
    float workingMemoryGain;
    float episodicMemoryBoost;
    
    // Learning modulation
    float attentionalLearningRate;
    float habituationRate;
    
    // Temporal dynamics
    float attentionalTime;
    float noveltyTime;
    
    Impl() : level(0.0f), baseline(0.0f), decayRate(0.1f),
             attentionLevel(0.0f), noveltySignal(0.0f), focusDuration(0.0f),
             attentionThreshold(0.5f), memoryConsolidationRate(1.0f),
             workingMemoryGain(1.0f), episodicMemoryBoost(0.0f),
             attentionalLearningRate(0.01f), habituationRate(0.95f),
             attentionalTime(0.0f), noveltyTime(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(std::make_unique<Impl>()) {}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attended stimuli and consolidates memory
    float attentionPlasticity = pImpl->attentionLevel * 1.5f;
    float noveltyPlasticity = pImpl->noveltySignal * 0.8f;
    float memoryConsolidationPlasticity = pImpl->memoryConsolidationRate;
    
    // Combine attentional and novelty components
    float totalPlasticity = attentionPlasticity + noveltyPlasticity + memoryConsolidationPlasticity;
    
    // Saturate at reasonable levels
    return std::min(totalPlasticity, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay attention and novelty signals
    pImpl->attentionLevel *= pImpl->habituationRate;
    pImpl->noveltySignal *= 0.9f;
    
    // Decay toward baseline
    float decay = 1.0f - pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decay;
}

void Acetylcholine::signalNovelty(float novelty) {
    // Novelty detection enhances ACh release
    pImpl->noveltySignal = std::min(novelty, 1.0f);
    pImpl->noveltyTime = 1.0f;  // Duration of novelty signal
    
    // Boost attention and memory consolidation
    pImpl->attentionLevel = std::min(pImpl->attentionLevel + novelty * 0.3f, 1.0f);
    pImpl->memoryConsolidationRate = std::min(pImpl->memoryConsolidationRate + novelty * 0.2f, 2.0f);
    
    // Update overall level
    pImpl->level += novelty * 0.5f;
}

void Acetylcholine::signalAttention(bool focus) {
    // Goal-directed attention
    if (focus) {
        pImpl->attentionLevel = std::min(pImpl->attentionLevel + 0.3f, 1.0f);
        pImpl->focusDuration = 2.0f;  // Duration of focused attention
    } else {
        pImpl->attentionLevel *= 0.5f;
    }
    
    // Update level based on attention
    pImpl->level += pImpl->attentionLevel * 0.3f;
}

void Acetylcholine::setAttentionThreshold(float threshold) {
    pImpl->attentionThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

Acetylcholine::~Acetylcholine() = default;

} // namespace nlm
