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
    float releaseRate;
    float attentionEffect;
    float memoryConsolidationEffect;
    float sleepEffect;
    float corticalActivationEffect;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.03f), releaseRate(1.0f), attentionEffect(0.85f),
             memoryConsolidationEffect(0.8f), sleepEffect(0.7f), corticalActivationEffect(0.9f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine (ACh) system initialized - attention, memory consolidation, sleep");
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply neuromodulatory effects
    applyEffects(static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // Attention boosts acetylcholine
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate * 0.9f);
}

void Acetylcholine::signalMemory(float memorySignal) {
    // Memory events boost acetylcholine for consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + memorySignal * pImpl->releaseRate * 0.8f);
}

void Acetylcholine::signalSleep(float sleepState) {
    // Sleep state affects acetylcholine levels
    // ACh is typically low during sleep
    pImpl->level = std::min(pImpl->peak, pImpl->level + sleepState * pImpl->releaseRate * 0.3f);
}

void Acetylcholine::applyEffects(float dt) {
    if (!pImpl->brain || pImpl->level <= 0.0f) return;
    
    float effect = pImpl->level * pImpl->attentionEffect;
    
    // Attention enhancement - boosts cortical activation and sensory processing
    if (effect > 0.0f) {
        // Enhance attention and cortical activation
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Acetylcholine enhances neural responsiveness and attention
                    float attentionEffect = effect * 0.5f;
                    neuron->injectCurrent(attentionEffect);
                }
            }
        }
    }
    
    // Memory consolidation enhancement
    if (pImpl->level > 0.4f) {
        effect = (pImpl->level - 0.4f) * 2.0f;
        // Enhance episodic memory consolidation
        if (pImpl->brain->getEpisodicMemory()) {
            // Let episodic memory system know about high ACh state for consolidation
            static_cast<void>(effect); // Placeholder for memory consolidation enhancement
        }
    }
    
    // Sleep modulation
    if (pImpl->level < 0.3f) {
        // Low ACh promotes sleep-like states
        static_cast<void>(dt); // Placeholder for sleep modulation
    }
}

float Acetylcholine::getPlasticityFactor() const {
    // Acetylcholine enhances attention and memory-related plasticity
    return 0.7f + 0.3f * pImpl->level;
}

float Acetylcholine::getAttentionEffect() const {
    return pImpl->level * pImpl->attentionEffect;
}

float Acetylcholine::getMemoryConsolidationEffect() const {
    return pImpl->level * pImpl->memoryConsolidationEffect;
}

} // namespace nlm