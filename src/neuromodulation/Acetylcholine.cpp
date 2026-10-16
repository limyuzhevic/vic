#include "Neuromodulator.hpp"
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
    
    // Learning state
    bool learningMode;
    
    // Enhancement factors
    float attentionalEnhancement;
    float memoryConsolidationFactor;
    float sensoryGain;
    
    // Timing for effects
    float learningPhaseStartTime;
    float learningPhaseDuration;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f)
        , learningMode(false), attentionalEnhancement(1.0f), memoryConsolidationFactor(1.0f), sensoryGain(1.0f)
        , learningPhaseStartTime(0.0f), learningPhaseDuration(100.0f) {} // Default 100 timestep learning phase
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
    // ACh specifically enhances memory-related plasticity
    // Promotes synaptic strengthening for memory encoding
    return 1.0f + pImpl->level * 0.8f;  // Up to 1.8x enhancement for memory
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay ACh levels over time
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Update enhancement factors based on current level
    pImpl->attentionalEnhancement = 1.0f + pImpl->level * 0.5f;  // Up to 1.5x attention boost
    pImpl->memoryConsolidationFactor = 1.0f + pImpl->level * 1.5f;  // Up to 2.5x memory boost
    pImpl->sensoryGain = 1.0f + pImpl->level * 0.7f;  // Up to 1.7x sensory boost
}

void Acetylcholine::enhanceAttention(float duration) {
    // Enter learning phase where attention is enhanced
    pImpl->learningMode = true;
    pImpl->learningPhaseStartTime = pImpl->level;  // Use current level as time reference
    pImpl->learningPhaseDuration = duration;
    
    // Boost attentional enhancement
    pImpl->attentionalEnhancement = 1.5f;
    NLM_LOG_INFO("ACh: Entering enhanced attention phase for " + std::to_string(duration) + " timesteps");
}

void Acetylcholine::strengthenMemoryTrace(const std::vector<NeuronId>& neuronIds, float strength) {
    // ACh specifically strengthens memory traces for given neurons
    if (neuronIds.empty()) return;
    
    // Calculate average strength based on current level
    float effectiveStrength = strength * (1.0f + pImpl->level * 0.8f);
    
    // In real implementation, this would:
    // 1. Strengthen recurrent connections for these neurons
    // 2. Enhance synaptic efficacy
    // 3. Increase persistent activity
    // 4. Reduce decay of memory traces
    
    NLM_LOG_INFO("ACh: Strengthening memory trace for " + std::to_string(neuronIds.size()) + 
                 " neurons with strength " + std::to_string(effectiveStrength));
}

void Acetylcholine::promoteMemoryConsolidation() {
    // ACh promotes memory consolidation by enhancing hippocampal-neocortical interactions
    pImpl->memoryConsolidationFactor = 2.5f * pImpl->level + 0.5f;
    
    // In real implementation, this would:
    // 1. Enhance slow-wave sleep activity
    // 2. Promote replay of episodic memories
    // 3. Strengthen long-term synaptic changes
    // 4. Reduce interference from new experiences
    
    NLM_LOG_INFO("ACh: Promoting memory consolidation (factor: " + std::to_string(pImpl->memoryConsolidationFactor) + ")");
}

void Acetylcholine::increaseSensoryGain(float enhancementFactor) {
    // ACh enhances sensory processing to prioritize novel or important stimuli
    pImpl->sensoryGain = 1.0f + enhancementFactor * (1.0f + pImpl->level * 0.5f);
    
    NLM_LOG_INFO("ACh: Increased sensory gain to " + std::to_string(pImpl->sensoryGain));
}

void Acetylcholine::resetForLearning() {
    // Reset ACh for new learning phase
    pImpl->learningMode = false;
    pImpl->learningPhaseStartTime = 0.0f;
    
    // Return to baseline enhancement factors
    pImpl->attentionalEnhancement = 1.0f + pImpl->level * 0.5f;
    pImpl->memoryConsolidationFactor = 1.0f + pImpl->level * 1.5f;
    pImpl->sensoryGain = 1.0f + pImpl->level * 0.7f;
    
    NLM_LOG_INFO("ACh: Reset for learning phase (baseline enhancement)");
}

} // namespace nlm
