#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float arousalEffect;
    float attentionEffect;
    float workingMemoryEffect;
    float cognitiveControlEffect;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.08f), releaseRate(1.0f), arousalEffect(0.9f),
             attentionEffect(0.8f), workingMemoryEffect(0.7f), cognitiveControlEffect(0.85f) {}
};

Norepinephrine::Norepinephrine() : pImpl(new Impl) {}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Norepinephrine (NE) system initialized - arousal, attention, working memory");
}

const char* Norepinephrine::getName() const {
    return "NE";
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply neuromodulatory effects
    applyEffects(static_cast<float>(dt));
}

void Norepinephrine::signalArousal(float arousal) {
    // Arousal events boost norepinephrine
    pImpl->level = std::min(pImpl->peak, pImpl->level + arousal * pImpl->releaseRate * 0.6f);
}

void Norepinephrine::signalAttention(float attention) {
    // Attention boosts norepinephrine
    pImpl->level = std::min(pImpl->peak, pImpl->level + attention * pImpl->releaseRate * 0.8f);
}

void Norepinephrine::signalWorkingMemory(float memoryUpdate) {
    // Working memory updates boost norepinephrine
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryUpdate * pImpl->releaseRate * 0.5f);
}

void Norepinephrine::applyEffects(float dt) {
    if (!pImpl->brain || pImpl->level <= 0.0f) return;
    
    float effect = pImpl->level * pImpl->arousalEffect;
    
    // Arousal enhancement - increases overall neural excitability
    if (effect > 0.0f) {
        // Enhance neural firing and excitability
        for (auto& region : pImpl->brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    // Norepinephrine increases excitability by lowering threshold
                    float excitabilityEffect = effect * 0.4f;
                    // Modify neuron threshold (implementation-dependent)
                    // For now, inject excitatory current
                    neuron->injectCurrent(excitabilityEffect);
                }
            }
        }
    }
    
    // Working memory enhancement
    if (pImpl->level > 0.3f) {
        effect = (pImpl->level - 0.3f) * 2.0f;
        // Enhance working memory capacity and stability
        if (pImpl->brain->getWorkingMemory()) {
            // Increase working memory stability
            static_cast<void>(effect); // Placeholder for working memory enhancement
        }
    }
    
    // Cognitive control enhancement
    if (pImpl->level > 0.6f) {
        effect = (pImpl->level - 0.6f) * 2.0f;
        // Enhance cognitive control and goal-directed behavior
        if (pImpl->brain->getPlanner()) {
            // Let planner know about high norepinephrine state
            static_cast<void>(effect); // Placeholder for cognitive control enhancement
        }
    }
}

float Norepinephrine::getPlasticityFactor() const {
    // Norepinephrine modulates plasticity for attention and memory
    return 0.6f + 0.4f * pImpl->level;
}

float Norepinephrine::getAttentionEffect() const {
    return pImpl->level * pImpl->attentionEffect;
}

float Norepinephrine::getWorkingMemoryEffect() const {
    return pImpl->level * pImpl->workingMemoryEffect;
}

} // namespace nlm