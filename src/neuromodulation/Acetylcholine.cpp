#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float decayRate;
    float releaseRate;
    float memoryConsolidationRate;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , decayRate(0.05f)
        , releaseRate(1.0f)
        , memoryConsolidationRate(0.5f) {}
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
    // ACh enhances memory consolidation and attention
    // Higher ACh increases effective learning rate
    float attentionMod = 1.0f + pImpl->level * 0.5f;
    float memoryConsolidation = 1.0f + pImpl->level * pImpl->memoryConsolidationRate;
    return std::min(2.0f, attentionMod * memoryConsolidation);
}

void Acetylcholine::update(TimestepDuration dt) {
    if (pImpl->brain) {
        // ACh modulates neural excitability based on attention demands
        // Higher ACh increases firing thresholds for better signal-to-noise ratio
        float attentionLevel = pImpl->level;
        
        // Find all neurons and modulate their excitability
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (attentionLevel > 0.5f) {
                        // Increase excitability: reduce effective threshold
                        float oldThreshold = neuron->getState().threshold;
                        neuron->setThreshold(oldThreshold * (1.0f - attentionLevel * 0.2f));
                    }
                }
            }
        }
    }
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // Boost ACh level for attention modulation
    setLevel(std::min(1.0f, pImpl->level + attention * pImpl->releaseRate));
}

void Acetylcholine::signalMemoryConsolidation() {
    // Enhance memory consolidation effects
    setLevel(std::min(1.0f, pImpl->level * 1.2f));
}

} // namespace nlm
