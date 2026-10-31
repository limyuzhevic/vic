#include "Neuromodulator.hpp"
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;                    // Current ACh concentration
    float baselineLevel;             // Baseline ACh level
    float attentionLevel;            // Current attention level (0-1)
    float memoryPriority;            // Memory consolidation priority (0-1)
    float timeConstant;              // Time constant for dynamics (ms)
    
    // Attention modulation factors
    float focusWeight;               // Weight of focused attention
    float broadWeight;               // Weight of broad attention
    
    // Memory consolidation processes
    float memoryReconsolidation;     // Current memory reconsolidation rate
    float memoryStabilization;       // Current memory stabilization rate
    float consolidationEfficiency;   // Efficiency of memory consolidation
    
    // Learning related processes
    float workingMemoryCapacity;     // Working memory capacity modulation
    float encodingEfficiency;        // Memory encoding efficiency
    
    Impl() : level(0.0f)
        , baselineLevel(0.1f)
        , attentionLevel(0.5f)
        , memoryPriority(0.5f)
        , timeConstant(150.0f)
        , focusWeight(0.6f)
        , broadWeight(0.4f)
        , memoryReconsolidation(0.0f)
        , memoryStabilization(0.0f)
        , consolidationEfficiency(1.0f)
        , workingMemoryCapacity(1.0f)
        , encodingEfficiency(1.0f) {}
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
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates multiple aspects of learning and memory
    float plasticityFactor = 1.0f;
    
    // Attention modulation: higher ACh increases attention to novel/important stimuli
    plasticityFactor *= (1.0f + pImpl->attentionLevel * 0.8f);
    
    // Memory consolidation: ACh promotes memory consolidation and stabilization
    plasticityFactor *= (1.0f + pImpl->memoryPriority * 0.6f);
    
    // Working memory: ACh increases working memory capacity
    plasticityFactor *= pImpl->workingMemoryCapacity;
    
    // Memory encoding: ACh improves encoding efficiency
    plasticityFactor *= pImpl->encodingEfficiency;
    
    // Memory reconsolidation: ACh facilitates reconsolidation of memories
    plasticityFactor *= (1.0f + pImpl->memoryReconsolidation * 0.4f);
    
    // Apply neuromodulator-specific factors
    plasticityFactor *= (1.0f + pImpl->level * 0.7f);  // Higher ACh = more modulation
    plasticityFactor = std::clamp(plasticityFactor, 0.0f, 4.0f);
    
    return plasticityFactor;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Real ACh dynamics with multiple components
    
    // Convert dt to milliseconds
    float dtMs = static_cast<float>(dt) * 1000.0f;
    
    // Update attention level based on current ACh level
    pImpl->attentionLevel = 0.5f + pImpl->level * 0.3f;
    pImpl->attentionLevel = std::clamp(pImpl->attentionLevel, 0.1f, 1.0f);
    
    // Update memory priority based on attention
    pImpl->memoryPriority = pImpl->attentionLevel * 0.7f + (1.0f - pImpl->attentionLevel) * 0.3f;
    
    // Update working memory capacity based on ACh
    pImpl->workingMemoryCapacity = 0.5f + pImpl->level * 0.5f;
    pImpl->workingMemoryCapacity = std::clamp(pImpl->workingMemoryCapacity, 0.5f, 2.0f);
    
    // Update encoding efficiency based on ACh
    pImpl->encodingEfficiency = 0.7f + pImpl->level * 0.6f;
    pImpl->encodingEfficiency = std::clamp(pImpl->encodingEfficiency, 0.5f, 2.0f);
    
    // Memory consolidation processes
    if (pImpl->level > 0.2f) {
        // Promote memory consolidation and stabilization
        pImpl->memoryReconsolidation = std::min(pImpl->memoryReconsolidation + 0.05f, 1.0f);
        pImpl->memoryStabilization = std::min(pImpl->memoryStabilization + 0.03f, 1.0f);
    } else {
        // Decay consolidation processes
        pImpl->memoryReconsolidation *= std::exp(-dtMs / 300.0f);
        pImpl->memoryStabilization *= std::exp(-dtMs / 500.0f);
    }
    
    // Update consolidation efficiency based on attention and memory processes
    pImpl->consolidationEfficiency = 0.8f + pImpl->attentionLevel * 0.3f + 
                                     pImpl->memoryReconsolidation * 0.2f;
    pImpl->consolidationEfficiency = std::clamp(pImpl->consolidationEfficiency, 0.5f, 2.0f);
    
    // Smooth ACh level update with gradual changes
    float targetLevel = pImpl->baselineLevel + (pImpl->level * 0.5f);  // Bias toward baseline
    float alpha = 0.15f;  // Smoothing factor
    float smoothedLevel = pImpl->level * (1.0f - alpha) + targetLevel * alpha;
    
    // Apply small fluctuations based on attention level
    float attentionNoise = (pImpl->attentionLevel - 0.5f) * 0.1f;
    pImpl->level = smoothedLevel + attentionNoise;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
}

} // namespace nlm
