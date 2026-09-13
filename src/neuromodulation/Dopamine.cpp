#include "Dopamine.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float plasticityFactor;
    float memoryConsolidationFactor;
    float attentionalModulation;
    float conceptModulation;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             plasticityFactor(1.0f), memoryConsolidationFactor(1.0f), attentionalModulation(1.0f), conceptModulation(1.0f) {}
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
    // Enhanced plasticity factor with dopamine's influence
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update derived factors based on current dopamine level
    pImpl->plasticityFactor = 1.0f + 0.8f * pImpl->level;
    pImpl->memoryConsolidationFactor = 1.0f + 0.6f * pImpl->level;
    pImpl->attentionalModulation = 1.0f + 0.5f * pImpl->level;
    pImpl->conceptModulation = 1.0f + 0.7f * pImpl->level;
}

void Dopamine::signalReward(float reward) {
    // Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::enhanceWorkingMemory(NeuralWorkingMemory* workingMemory, float factor) {
    if (workingMemory && pImpl->level > 0.5f) {
        // Enhance working memory trace strength based on dopamine level
        // Positive dopamine strengthens memory traces
        workingMemory->strengthenMemory(factor * pImpl->memoryConsolidationFactor);
    }
}

void Dopamine::enhanceEpisodicMemory(NeuralEpisodicMemory* episodicMemory, float consolidation) {
    if (episodicMemory && pImpl->level > 0.5f) {
        // Enhance memory storage and consolidation
        // Positive dopamine enhances memory consolidation and retrieval
        // We need to add a method to NeuralEpisodicMemory to set consolidation rate
        // For now, we'll use a placeholder implementation
        
        // Check if the method exists - if not, we might need to modify the interface
        // episodicMemory->setConsolidationRate(episodicMemory->getConsolidationRate() * 
        //                                     (1.0f + consolidation * pImpl->memoryConsolidationFactor));
        
        // For now, just log that we would enhance episodic memory
        // In a real implementation, we'd add methods to NeuralEpisodicMemory
        // to directly support dopamine-enhanced consolidation
    }
}

void Dopamine::modulateAttention(AttentionalSelection* attention, float bias) {
    if (attention && pImpl->level > 0.5f) {
        // Modulate attentional selection and bias
        // Positive dopamine biases attention toward salient features
        // Note: AttentionalSelection doesn't have direct bias setters
        // This function could add new methods to the interface or be a placeholder
        // For now, we'll add a placeholder for future implementation
        
        // Check if the method exists
        // float currentBias = attention->getAttentionBias();
        // attention->setAttentionBias(currentBias + bias * pImpl->attentionalModulation);
        
        // For now, we'll just enhance attentional selection by calling processCompetition
        // which is already available in the interface
        std::vector<NeuronId> competitors;
        if (attention->getBrain()) {
            auto* brain = attention->getBrain();
            if (brain && brain->getWorkingMemory()) {
                competitors = brain->getWorkingMemory()->getMemoryNeurons();
            }
        }
        if (!competitors.empty()) {
            attention->processCompetition(competitors);
        }
    }
}

void Dopamine::modulateConceptFormation(ConceptFormation* conceptFormation, float threshold) {
    if (conceptFormation && pImpl->level > 0.5f) {
        // Modulate concept discovery and abstraction
        // Positive dopamine accelerates concept discovery
        // Lower threshold means easier concept formation, which is what we want with positive dopamine
        float currentThreshold = conceptFormation->getFormationThreshold();
        float newThreshold = currentThreshold * (1.0f - threshold * pImpl->conceptModulation);
        // Clamp to valid range
        newThreshold = std::clamp(newThreshold, 0.0f, 1.0f);
        conceptFormation->setFormationThreshold(newThreshold);
    }
}

float Dopamine::getMemoryConsolidationFactor() const {
    return pImpl->memoryConsolidationFactor;
}

float Dopamine::getAttentionalModulation() const {
    return pImpl->attentionalModulation;
}

float Dopamine::getConceptModulation() const {
    return pImpl->conceptModulation;
}

} // namespace nlm