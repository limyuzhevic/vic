#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionStrength;      // 0.0f to 1.0f - attention modulation
    float memoryConsolidation;    // 0.0f to 1.0f - memory consolidation strength
    float learningRateMod;        // Learning rate modulation factor
    float arousalLevel;           // Overall arousal (0.0f to 1.0f)
    
    // Time constants
    static constexpr float ATTENTION_TIME_CONSTANT = 0.1f;    // seconds
    static constexpr float CONSOLIDATION_TIME_CONSTANT = 0.5f; // seconds
    static constexpr float DECAY_RATE = 0.05f;                 // per timestep
    static constexpr float MAX_ATTENTION = 0.95f;              // max attention to prevent overload
    
    // Integration weights
    static constexpr float ATTENTION_SENSORY_WEIGHT = 0.7f;
    static constexpr float ATTENTION_MEMORY_WEIGHT = 0.3f;
    static constexpr float CONSOLIDATION_WORKING_MEM_WEIGHT = 0.8f;
    static constexpr float CONSOLIDATION_EPISODIC_WEIGHT = 0.2f;
    
    Impl()
        : level(0.0f)
        , attentionStrength(0.0f)
        , memoryConsolidation(0.0f)
        , learningRateMod(1.0f)
        , arousalLevel(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
}

const char* Acetylcholine::getName() const {
    return "ACH";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity: enhances LTP, suppresses LTD
    return 1.0f + pImpl->attentionStrength * 0.5f - pImpl->memoryConsolidation * 0.2f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Smooth dynamics for attention and consolidation
    float targetAttention = pImpl->arousalLevel * ATTENTION_SENSORY_WEIGHT;
    
    // Integrate attention and consolidation
    pImpl->attentionStrength += (targetAttention - pImpl->attentionStrength) * 
                               ATTENTION_TIME_CONSTANT * static_cast<float>(dt);
    pImpl->memoryConsolidation += (pImpl->arousalLevel * CONSOLIDATION_WORKING_MEM_WEIGHT - 
                                   pImpl->memoryConsolidation) * 
                               CONSOLIDATION_TIME_CONSTANT * static_cast<float>(dt);
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->DECAY_RATE * static_cast<float>(dt));
    
    // Clamp values
    pImpl->attentionStrength = std::min(pImpl->MAX_ATTENTION, 
                                       pImpl->attentionStrength);
    pImpl->memoryConsolidation = std::min(1.0f, pImpl->memoryConsolidation);
    
    NLM_LOG_DEBUG(std::string("ACh level: ") + std::to_string(pImpl->level) +
                  ", attention: " + std::to_string(pImpl->attentionStrength) +
                  ", consolidation: " + std::to_string(pImpl->memoryConsolidation));
}

void Acetylcholine::modulateAttention(float attentionStrength) {
    pImpl->arousalLevel = std::clamp(attentionStrength, 0.0f, 1.0f);
}

void Acetylcholine::consolidateWorkingMemory(const Brain* brain) {
    if (!brain || pImpl->memoryConsolidation <= 0.01f) return;
    
    auto* wm = brain->getWorkingMemory();
    if (!wm) return;
    
    // Calculate consolidation based on ACh level
    size_t tracesToConsolidate = std::min(static_cast<size_t>(wm->getActiveTraces() * 
                                                           pImpl->memoryConsolidation),
                                         wm->getCapacity());
    
    if (tracesToConsolidate > 0) {
        NLM_LOG_INFO(std::string("ACh consolidating ") + std::to_string(tracesToConsolidate) +
                     " working memory traces (strength: " + std::to_string(pImpl->memoryConsolidation) + ")");
        
        // In a full implementation, we would move these traces to long-term storage
        // For now, we just log the action
    }
}

void Acetylcholine::enhanceSensoryMemoryEncoding(const Brain* brain) {
    if (!brain || pImpl->attentionStrength <= 0.01f) return;
    
    // Enhance sensory encoding when attention is high
    NLM_LOG_DEBUG(std::string("ACh enhancing sensory encoding with attention: ") + 
                  std::to_string(pImpl->attentionStrength));
}

void Acetylcholine::facilitateLongTermPotentiation(const Brain* brain) {
    if (!brain || pImpl->level <= 0.0f) return;
    
    // ACh facilitates LTP by increasing plasticity
    auto* stdp = brain->getSTDP();
    if (stdp) {
        float enhancedLTP = stdp->getLTPWeight() * (1.0f + pImpl->level * 0.3f);
        stdp->setLTPWeight(enhancedLTP);
        
        NLM_LOG_DEBUG(std::string("ACh enhanced LTP weight to: ") + std::to_string(enhancedLTP));
    }
}

void Acetylcholine::modulateLearningRate(float baseRate, float novelty, float reward) {
    // ACh modulates learning based on multiple factors
    pImpl->learningRateMod = baseRate * 
                           (1.0f + pImpl->attentionStrength * 0.2f) * 
                           (1.0f + novelty * 0.1f) * 
                           (1.0f - std::abs(reward) * 0.1f);
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
    pImpl->attentionStrength = 0.0f;
    pImpl->memoryConsolidation = 0.0f;
    pImpl->learningRateMod = 1.0f;
    pImpl->arousalLevel = 0.0f;
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionStrength;
}

float Acetylcholine::getMemoryConsolidationStrength() const {
    return pImpl->memoryConsolidation;
}

} // namespace nlm