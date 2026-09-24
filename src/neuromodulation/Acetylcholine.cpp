#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionLevel;
    float baseline;
    float decayRate;
    float releaseRate;
    float workingMemoryLoad;
    
    Impl() : level(0.0f), attentionLevel(0.0f), baseline(0.05f), decayRate(0.5f), releaseRate(2.0f), workingMemoryLoad(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity based on working memory load
    // Higher load increases plasticity to support new learning
    float loadEffect = pImpl->workingMemoryLoad * 0.8f;  // Max 80% enhancement
    float satEffect = loadEffect / (1.0f + loadEffect);  // Saturation
    
    float basePlasticity = 0.3f;
    float plasticityFactor = basePlasticity * (1.0f + satEffect);
    
    return std::clamp(plasticityFactor, 0.3f, 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics based on Sarter & Bruno (2001) model
    float deltaT = static_cast<float>(dt) * 0.001f;
    
    // Decay towards baseline
    float decayFactor = std::exp(-pImpl->decayRate * deltaT);
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * decayFactor;
    
    // Maintain attention level influence
    pImpl->attentionLevel = std::max(0.0f, pImpl->attentionLevel - 0.1f * deltaT);
    
    // Ensure physiological bounds
    pImpl->level = std::max(0.0f, std::min(pImpl->level, 1.0f));
    pImpl->attentionLevel = std::max(0.0f, std::min(pImpl->attentionLevel, 1.0f));
}

void Acetylcholine::signalAttention(float attentionLevel) {
    // Phasic ACh release signals attention to task-relevant stimuli
    // Increases cholinergic tone to enhance cortical processing
    
    float clampedAttention = std::clamp(attentionLevel, 0.0f, 1.0f);
    pImpl->attentionLevel = clampedAttention;
    
    // Transient increase in ACh level
    pImpl->level = std::min(1.0f, pImpl->level + clampedAttention * pImpl->releaseRate * 5.0f);
}

void Acetylcholine::signalWorkingMemoryLoad(float load) {
    // ACh maintains persistent representation of working memory load
    pImpl->workingMemoryLoad = std::clamp(load, 0.0f, 1.0f);
    
    // Adjust baseline based on load for sustained attention
    pImpl->baseline = 0.05f + pImpl->workingMemoryLoad * 0.15f;
}

void Acetylcholine::initialize(class Brain* brain) {
    // Initialize acetylcholine system with brain reference
    if (brain) {
        // TODO: Store brain reference if needed for future extensions
        // ACh operates as an attention modulator that can be integrated
        // with brain systems through the Neuromodulation interface
    }
}