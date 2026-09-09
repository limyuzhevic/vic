#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;                    // Reference to brain system
    float level;                    // Current ACh level (0.0-2.0)
    float baseline;                  // Baseline level
    float attentionGain;             // Attention modulation factor
    float memoryConsolidationStrength; // Memory consolidation enhancement
    float backgroundSuppression;     // Background activity suppression
    float peakLevel;                 // Peak release level
    float decayRate;                 // Decay towards baseline
    float releaseRate;               // Release rate on signaling
    float attentionThreshold;        // Threshold for attention events
    size_t memoryTraceCapacity;      // Working memory capacity
    float targetAttentionLevel;      // Target attention level for updating
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.1f), attentionGain(1.0f), 
             memoryConsolidationStrength(1.0f), backgroundSuppression(0.0f),
             peakLevel(2.0f), decayRate(0.05f), releaseRate(1.5f),
             attentionThreshold(0.3f), memoryTraceCapacity(100),
             targetAttentionLevel(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances plasticity for attention-related synapses
    return 0.8f + 0.4f * pImpl->attentionGain;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention gain based on current level
    pImpl->attentionGain = 0.5f + 0.5f * pImpl->level;
    
    // Update background suppression
    pImpl->backgroundSuppression = std::min(0.8f, pImpl->level * 0.4f);
    
    // Smooth approach to target attention level if set
    if (pImpl->targetAttentionLevel > 0.0f) {
        float diff = pImpl->targetAttentionLevel - pImpl->level;
        if (std::abs(diff) > 0.01f) {
            pImpl->level += diff * 0.1f;
            pImpl->targetAttentionLevel = 0.0f;
        }
    }
}

float Acetylcholine::getAttentionModulator() const {
    return pImpl->attentionGain;
}

void Acetylcholine::signalAttention(bool focusEvent) {
    if (focusEvent) {
        // Strong burst for attention event
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + 1.0f);
        pImpl->memoryConsolidationStrength = 1.5f;
        pImpl->attentionGain = 1.5f;
        pImpl->backgroundSuppression = 0.6f;
        NLM_LOG_INFO("Acetylcholine: Attention burst triggered");
    } else {
        // Moderate release for general attentional processing
        pImpl->level = std::min(pImpl->peakLevel, pImpl->level + 0.3f);
        pImpl->memoryConsolidationStrength = 1.2f;
        pImpl->attentionGain = 1.2f;
    }
}

float Acetylcholine::getMemoryConsolidationFactor() const {
    return pImpl->memoryConsolidationStrength;
}

void Acetylcholine::promoteMemoryConsolidation(int neuralTraceId) {
    // Enhance consolidation of neural traces
    pImpl->memoryConsolidationStrength = 2.0f;
    NLM_LOG_INFO("Acetylcholine: Promoting memory consolidation for trace " + std::to_string(neuralTraceId));
    
    // This would integrate with the working memory system
    // For now, just log the event
}

void Acetylcholine::enhanceSensoryProcessing() const {
    // Increase gain of sensory processing pathways
    // This would affect sensory neuron excitability
    if (pImpl->brain) {
        NLM_LOG_INFO("Acetylcholine: Enhancing sensory processing");
        // Implementation would modulate sensory neuron excitability
    }
}

void Acetylcholine::suppressBackgroundActivity() const {
    // Reduce spontaneous firing in non-attended areas
    // Implementation would modulate baseline currents
    if (pImpl->brain) {
        NLM_LOG_INFO("Acetylcholine: Suppressing background activity");
        // Implementation would adjust global neuronal excitability
    }
}

} // namespace nlm