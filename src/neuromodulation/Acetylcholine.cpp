#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <unordered_map>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float baselineRelease;
    float decayRate;
    float releaseRate;
    float facilitationRate;
    
    // Memory consolidation influence
    float memoryConsolidationFactor;
    
    // Attention modulation
    float topDownBias;
    float saliencyThreshold;
    
    // Track attended neurons for memory consolidation
    std::unordered_map<uint64_t, float> attendedNeurons;
    std::unordered_map<uint64_t, float> memoryConsolidationWeights;
    
    Impl() : 
        level(0.0f), 
        baseline(0.0f), 
        baselineRelease(0.05f), 
        decayRate(0.05f), 
        releaseRate(2.0f), 
        facilitationRate(0.3f),
        memoryConsolidationFactor(0.0f),
        topDownBias(0.0f),
        saliencyThreshold(0.5f) {}
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
    pImpl->level = std::clamp(level, 0. temi, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity: low levels enable LTP, high levels suppress it
    // This allows for attention-gated learning
    if (pImpl->level < 0.3f) {
        return 1.0f + pImpl->level * 2.0f;  // Boost plasticity for learning
    } else {
        return std::max(0.2f, 1.2f - pImpl->level * 0.5f);  // Suppress plasticity for consolidation
    }
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: decays to baseline, increases with attention/saliency
    // Models cholinergic system dynamics
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Facilitation from attended neurons
    pImpl->level = std::min(1.0f, pImpl->level + pImpl->facilitationRate * pImpl->topDownBias);
}

void Acetylcholine::signalAttention(float attentionStrength, float saliency) {
    // Top-down attentional signal increases ACh level
    pImpl->level = std::min(1.0f, pImpl->level + attentionStrength * static_cast<float>(saliency) * pImpl->releaseRate);
    
    // Store attended neuron info for memory consolidation
    pImpl->topDownBias = attentionStrength;
    if (saliency > pImpl->saliencyThreshold) {
        pImpl->memoryConsolidationFactor = 1.0f;
    }
}

void Acetylcholine::consolidateMemory(Brain* brain, NeuronId neuronId, float activation) {
    // If neuron is attended (high ACh), enhance memory consolidation
    if (pImpl->level > 0.3f && activation > 0.1f) {
        // Mark neuron as attended for consolidation
        uint64_t id = neuronId.index();
        pImpl->attendedNeurons[id] = activation;
        
        // Apply consolidation boost to memory systems
        if (brain) {
            auto* workingMemory = brain->getWorkingMemory();
            if (workingMemory && workingMemory->contains(neuronId)) {
                // Enhance activation in working memory for attended neurons
                float currentAct = workingMemory->getNeuronActivation(neuronId);
                workingMemory->strengthenMemory((activation + currentAct) * 0.5f * pImpl->memoryConsolidationFactor);
            }
            
            auto* episodicMemory = brain->getEpisodicMemory();
            if (episodicMemory) {
                // Mark this episode as attention-highlighted
                // This would require episodic memory tracking of attention
            }
        }
    }
}

void Acetylcholine::applyAttentionModulation(Brain* brain) {
    if (!brain || pImpl->level < 0.1f) return;
    
    // Apply attention effects across the brain:
    // 1. Enhance neural excitability in attended regions
    // 2. Suppress background activity (inhibition)
    // 3. Boost sensory processing gain
    
    float attentionGain = pImpl->level * 2.0f;  // Attended regions get 2x gain
    float backgroundSuppression = 1.0f - pImpl->level * 0.5f;  // Background gets 50% suppression at high ACh
    
    // This would be implemented based on the brain's region structure
    // For now, we just set the modulation parameters
    pImpl->memoryConsolidationFactor = std::min(1.0f, pImpl->level);
}

} // namespace nlm