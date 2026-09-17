#include "Acetylcholine.hpp"
#include "../../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    // ACh dynamics parameters
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Memory-related parameters
    float memoryConsolidationRate;
    float attentionBoost;
    float workingMemoryEnhancement;
    
    // State management
    float attentionScore;
    float memoryPriority;
    std::vector<float> memoryTrace;
    
    // Learning-related parameters
    float encodingRate;
    float consolidationRate;
    
    Impl() : level(0.0f), baseline(0.02f), peak(0.5f), decayRate(0.05f), 
             releaseRate(1.0f), memoryConsolidationRate(0.3f),
             attentionBoost(1.5f), workingMemoryEnhancement(1.2f),
             attentionScore(0.0f), memoryPriority(0.0f), encodingRate(0.5f),
             consolidationRate(0.2f) {}
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
    // ACh primarily affects encoding and consolidation, not raw plasticity
    // It enhances NMDA receptor function and promotes LTP at certain synapses
    float factor = 1.0f;
    
    // Low ACh: reduced attention, poor encoding
    if (pImpl->level < 0.2f) {
        factor = 0.5f + pImpl->level * 0.5f;
    } else if (pImpl->level < 0.6f) {
        // Moderate ACh: optimal attention and memory formation
        factor = 1.0f + pImpl->level * 0.4f;
    } else {
        // High ACh: may cause over-focusing on some stimuli
        factor = 1.3f + pImpl->level * 0.2f;
    }
    
    // Apply attention modulation
    factor *= (1.0f + pImpl->attentionScore * 0.3f);
    
    return factor;
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    float decay = pImpl->decayRate * dtFloat;
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update attention score based on level
    pImpl->attentionScore = pImpl->attentionScore * 0.9f + pImpl->level * 0.1f;
    
    // Update memory trace (slower decay than level)
    pImpl->memoryPriority = pImpl->memoryPriority * 0.95f;
    
    // Apply attention-based enhancement
    if (pImpl->level > 0.3f) {
        // Neural encoding enhancement
        float encodingBoost = pImpl->level * pImpl->encodingRate;
        
        // Memory consolidation
        float consolidationBoost = pImpl->level * pImpl->consolidationRate;
        
        // Update level with encoding/consolidation effects
        pImpl->level = std::min(pImpl->peak, pImpl->level + encodingBoost * 0.01f);
    }
}

void Acetylcholine::signalAttention(float attentionStrength, float memoryPriority) {
    // Enhance ACh release based on attentional demands
    if (attentionStrength > 0.0f) {
        // Phasic response to attentional focus
        float attentionResponse = attentionStrength * pImpl->attentionBoost;
        pImpl->level = std::max(pImpl->level, pImpl->baseline + attentionResponse);
        
        // Set memory priority
        pImpl->memoryPriority = std::max(pImpl->memoryPriority, memoryPriority);
        
        // Enhance working memory
        pImpl->workingMemoryEnhancement = 1.0f + attentionStrength * 0.5f;
    }
}

void Acetylcholine::consolidateMemory(const std::vector<float>& memoryPattern, float strength) {
    // Store memory pattern for consolidation
    if (memoryPattern.size() > 0) {
        // Average pattern strength
        float patternStrength = 0.0f;
        for (float val : memoryPattern) {
            patternStrength += val;
        }
        patternStrength /= static_cast<float>(memoryPattern.size());
        
        // Scale by consolidation rate
        float consolidation = patternStrength * strength * pImpl->memoryConsolidationRate;
        
        // Enhance memory trace
        pImpl->memoryTrace.push_back(consolidation);
        
        // If consolidation is strong, enhance ACh level for future consolidation
        if (consolidation > 0.5f) {
            pImpl->level = std::min(pImpl->peak, pImpl->level + consolidation * 0.1f);
        }
    }
}

float Acetylcholine::getAttentionScore() const {
    return pImpl->attentionScore;
}

float Acetylcholine::getMemoryPriority() const {
    return pImpl->memoryPriority;
}

const std::vector<float>& Acetylcholine::getMemoryTrace() const {
    return pImpl->memoryTrace;
}

} // namespace nlm