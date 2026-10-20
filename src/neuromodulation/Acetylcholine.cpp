// Acetylcholine implementation
#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float plasticityFactor;
    float timeConstant;
    float attentionModulation;
    
    Impl() : level(0.0f), plasticityFactor(1.0f), timeConstant(1.0f), attentionModulation(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}
Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "Acetylcholine";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update attention modulation based on level
    pImpl->attentionModulation = pImpl->level * 2.0f;  // Range: 0.0 to 2.0
    pImpl->plasticityFactor = 1.0f + pImpl->level * 0.5f;  // Range: 1.0 to 1.5
}

float Acetylcholine::getPlasticityFactor() const {
    return pImpl->plasticityFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay ACh level over time
    pImpl->level *= std::exp(-dt / pImpl->timeConstant);
}

void Acetylcholine::applyToAttention(Brain* brain) {
    if (!brain || pImpl->level < 0.01f) return;
    
    float attentionBoost = pImpl->attentionModulation;
    
    // Apply to attentional selection system
    // Enhances signal-to-noise ratio in attended neural populations
    for (auto& region : brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                // Apply attention modulation
                neuron->injectCurrent(attentionBoost * 0.3f);
                
                // Enhance plasticity for attended neurons
                auto& flags = neuron->getPlasticityFlags();
                if (pImpl->level > 0.2f) {
                    flags.hebbian = true;
                }
            }
        }
    }
}

void Acetylcholine::enhanceMemoryConsolidation(Brain* brain) {
    if (!brain || pImpl->level < 0.01f) return;
    
    // Enhance memory consolidation processes
    float consolidationBoost = pImpl->level * 0.8f;
    
    // This would enhance synaptic strengthening during memory consolidation
    // For now, we just inject current to simulate consolidation effects
    for (auto& region : brain->getRegions()) {
        for (auto& population : region->getPopulations()) {
            for (auto* neuron : population->getNeurons()) {
                neuron->injectCurrent(consolidationBoost * 0.2f);
            }
        }
    }
}

} // namespace nlm