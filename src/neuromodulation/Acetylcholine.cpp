#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionWeight;
    float memoryConsolidationFactor;
    float arousalLevel;
    
    Impl() : level(0.0f), attentionWeight(0.0f), memoryConsolidationFactor(1.0f), arousalLevel(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}
Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine (ACh)";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
    
    // ACh affects attention and memory consolidation
    pImpl->attentionWeight = pImpl->level * 0.5f;
    pImpl->memoryConsolidationFactor = 1.0f + pImpl->level * 0.5f;
    pImpl->arousalLevel = pImpl->level * 0.3f;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh increases plasticity for memory consolidation
    return 1.0f + pImpl->level * 0.3f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Slow decay of ACh signal
    pImpl->level *= 0.95f;
    
    // Apply attention modulation effects
    if (pImpl->level > 0.1f) {
        // ACh enhances attention to relevant stimuli
        // This would modulate neural excitability in attention circuits
        NLM_LOG_INFO("Acetylcholine level: " + std::to_string(pImpl->level) + 
                    ", attention weight: " + std::to_string(pImpl->attentionWeight));
    }
}

void Acetylcholine::signalAttentionFocus(NeuronId neuron, float strength) {
    // ACh signals focus attention on specific neural populations
    setLevel(getLevel() + strength * 0.1f);
}

void Acetylcholine::signalMemoryConsolidation(const std::vector<NeuronId>& memoryNeurons, float strength) {
    // ACh triggers memory consolidation processes
    if (strength > 0.0f) {
        setLevel(getLevel() + strength * 0.05f);
        
        // Enhance synaptic strengthening for these neurons
        for (NeuronId neuron : memoryNeurons) {
            NLM_LOG_INFO("ACh promoting memory consolidation in neuron " + 
                        std::to_string(neuron.getId()) + " with strength " + std::to_string(strength));
        }
    }
}

void Acetylcholine::signalSleepRelatedConsolidation() {
    // ACh levels should decrease during sleep for memory consolidation
    // This is typically handled by sleep systems
    // For now, we just note this
    NLM_LOG_INFO("Acetylcholine signaling sleep-related memory consolidation");
}

} // namespace nlm