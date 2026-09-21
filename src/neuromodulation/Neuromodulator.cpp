#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Memory consolidation parameters
    float consolidationThreshold;
    float memoryStrength;
    
    // Pointers to memory systems (set by brain)
    NeuralWorkingMemory* workingMemory;
    NeuralEpisodicMemory* episodicMemory;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
           , consolidationThreshold(0.5f), memoryStrength(1.0f)
           , workingMemory(nullptr), episodicMemory(nullptr) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Memory consolidation is now integrated with dopamine dynamics
    if (pImpl->workingMemory && pImpl->episodicMemory) {
        // Modulate working memory rehearsal based on dopamine level
        if (pImpl->level > 0.3f) {
            // Higher dopamine increases rehearsal strength
            float rehearsalFactor = pImpl->level;
            pImpl->workingMemory->strengthenMemory(rehearsalFactor);
        }
    }
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    
    // Trigger memory consolidation on reward
    if (pImpl->episodicMemory) {
        consolidateEpisodicMemory(pImpl->episodicMemory);
    }
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    
    // Prediction error drives learning and memory updating
    if (pImpl->episodicMemory) {
        consolidateEpisodicMemory(pImpl->episodicMemory);
    }
}

void Dopamine::consolidateEpisodicMemory(NeuralEpisodicMemory* episodicMemory) {
    if (!episodicMemory) return;
    
    // Store the memory system reference
    pImpl->episodicMemory = episodicMemory;
    
    // Higher dopamine = more important memories, higher consolidation priority
    float importanceFactor = std::min(1.0f, pImpl->level * 2.0f);
    
    // Trigger memory consolidation
    if (importanceFactor > 0.5f) {
        // Strong consolidation: reinforce important memories
        pImpl->episodicMemory->consolidate(0.3f * importanceFactor);
    } else {
        // Standard consolidation
        pImpl->episodicMemory->consolidate(0.3f);
    }
}

void Dopamine::modulateMemoryRehearse(NeuralWorkingMemory* workingMemory) {
    if (!workingMemory) return;
    
    // Store the memory system reference
    pImpl->workingMemory = workingMemory;
    
    // Dopamine modulates rehearsal in working memory
    if (pImpl->level > 0.2f) {
        // Dopamine promotes rehearsal of recent memories
        // Higher dopamine = more rehearsal, stronger memory trace
        float rehearsalStrength = pImpl->level * 0.5f;
        workingMemory->strengthenMemory(1.0f + rehearsalStrength);
    }
}

// Add method to set memory system references
void Dopamine::setMemorySystems(NeuralWorkingMemory* workingMem, NeuralEpisodicMemory* episodicMem) {
    pImpl->workingMemory = workingMem;
    pImpl->episodicMemory = episodicMem;
}

} // namespace nlm
