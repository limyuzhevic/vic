// Enhanced Acetylcholine implementation
// Comprehensive acetylcholine neuromodulator for attention and memory

#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float cholinergicTone;
    float attentionGain;
    float memoryConsolidationRate;
    
    // Enhanced acetylcholine state tracking
    float corticalExcitability;
    float sensoryGating;
    float featureBinding;
    float workingMemoryMaintenance;
    float memoryEncoding;
    
    Impl() : 
        level(0.0f), baseline(0.05f), cholinergicTone(0.5f),
        attentionGain(1.5f), memoryConsolidationRate(1.2f),
        corticalExcitability(1.0f), sensoryGating(1.0f),
        featureBinding(1.0f), workingMemoryMaintenance(1.0f),
        memoryEncoding(1.0f) {}
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
    // Acetylcholine enhances plasticity for attention-related learning
    float baselinePlasticity = 0.4f;
    
    // Cortical excitability contributes to plasticity
    float corticalEffect = pImpl->corticalExcitability * 0.3f;
    
    // Memory encoding enhances synaptic strengthening
    float memoryEffect = pImpl->memoryEncoding * 0.4f;
    
    // Sensory gating filters irrelevant inputs
    float sensoryEffect = (1.0f - pImpl->sensoryGating) * 0.2f;
    
    // Feature binding enables coordinated activity
    float featureEffect = pImpl->featureBinding * 0.1f;
    
    return baselinePlasticity + corticalEffect + memoryEffect + sensoryEffect + featureEffect;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Enhanced acetylcholine dynamics with multiple components
    float dtFloat = static_cast<float>(dt);
    
    // Maintain cholinergic tone with slow decay
    pImpl->cholinergicTone = std::max(pImpl->baseline,
        pImpl->cholinergicTone - 0.02f * dtFloat);
    
    // Update attentional focus based on current level
    pImpl->attentionGain = 1.0f + pImpl->level * 1.5f;
    
    // Memory consolidation is enhanced by acetylcholine
    pImpl->memoryConsolidationRate = 1.0f + pImpl->level * 0.5f;
    
    // Overall level is weighted sum of cholinergic tone
    pImpl->level = pImpl->cholinergicTone;
}

// Enhanced acetylcholine methods implementation
float Acetylcholine::getAttentionModulation() const {
    // Acetylcholine modulates selective attention and filtering
    return pImpl->attentionGain * getLevel() * 1.5f;
}

float Acetylcholine::getMemoryEncodingFactor() const {
    // Acetylcholine enhances memory encoding through theta modulation
    return 1.0f + pImpl->memoryEncoding * 0.8f;
}

float Acetylcholine::getMemoryConsolidationFactor() const {
    // Acetylcholine promotes memory consolidation during sleep/relaxation
    return 1.0f + pImpl->memoryConsolidationRate * 0.6f;
}

float Acetylcholine::getCorticalExcitabilityFactor() const {
    // Acetylcholine increases cortical excitability and signal-to-noise
    return pImpl->corticalExcitability * 1.3f;
}

float Acetylcholine::getSensoryGatingFactor() const {
    // Acetylcholine gates sensory input to prioritize attended stimuli
    return pImpl->sensoryGating;
}

float Acetylcholine::getWorkingMemoryMaintenanceFactor() const {
    // Acetylcholine maintains working memory through persistent activity
    return 1.0f + pImpl->workingMemoryMaintenance * 0.7f;
}

float Acetylcholine::getFeatureBindingFactor() const {
    // Acetylcholine enables feature binding across neural populations
    return pImpl->featureBinding;
}

void Acetylcholine::setBaseline(float baseline) {
    pImpl->baseline = std::clamp(baseline, 0.0f, 1.0f);
    pImpl->cholinergicTone = pImpl->baseline;
}

void Acetylcholine::setCholinergicTone(float tone) {
    pImpl->cholinergicTone = std::clamp(tone, 0.0f, 1.0f);
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = std::clamp(gain, 0.5f, 3.0f);
}

float Acetylcholine::getBaseline() const {
    return pImpl->baseline;
}

float Acetylcholine::getCholinergicTone() const {
    return pImpl->cholinergicTone;
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

} // namespace nlm