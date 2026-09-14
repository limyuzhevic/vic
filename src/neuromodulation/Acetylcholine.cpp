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
    float memoryGain;
    float attentionStrength;
    float learningModulator;
    float memoryConsolidationRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.01f)
        , peak(1.0f)
        , decayRate(0.08f)
        , memoryGain(1.0f)
        , attentionStrength(0.8f)
        , learningModulator(0.9f)
        , memoryConsolidationRate(0.7f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    updateEffects();
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances synaptic plasticity and learning
    return 1.0f + pImpl->level * pImpl->memoryGain;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    updateEffects();
}

float Acetylcholine::getMemoryConsolidationRate() const {
    return pImpl->memoryConsolidationRate;
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionStrength;
}

float Acetylcholine::getLearningRate() const {
    return pImpl->learningModulator;
}

void Acetylcholine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

void Acetylcholine::setPeak(float peak) {
    pImpl->peak = peak;
}

void Acetylcholine::setDecayRate(float rate) {
    pImpl->decayRate = rate;
}

void Acetylcholine::setMemoryGain(float gain) {
    pImpl->memoryGain = gain;
    updateEffects();
}

void Acetylcholine::setAttentionStrength(float strength) {
    pImpl->attentionStrength = strength;
}

void Acetylcholine::setLearningModulator(float modulator) {
    pImpl->learningModulator = modulator;
    updateEffects();
}

void Acetylcholine::updateEffects() {
    // Calculate neuromodulatory effects based on current acetylcholine level
    float normalizedLevel = (pImpl->level - pImpl->baseline) / (pImpl->peak - pImpl->baseline);
    normalizedLevel = std::clamp(normalizedLevel, 0.0f, 1.0f);
    
    // Memory consolidation: ACh promotes stabilization of new memories
    pImpl->memoryConsolidationRate = 0.5f + 0.5f * normalizedLevel * pImpl->memoryGain;
    
    // Attention: ACh enhances signal-to-noise ratio in cortical circuits
    pImpl->attentionStrength = 0.3f + 0.7f * normalizedLevel;
    
    // Learning: ACh enhances synaptic strengthening during novel experiences
    pImpl->learningModulator = 0.8f + 0.2f * normalizedLevel;
}

} // namespace nlm