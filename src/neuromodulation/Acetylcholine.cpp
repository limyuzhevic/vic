#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // ACh effects on neural systems
    float attentionGain;
    float memoryConsolidationRate;
    float corticalExcitability;
    
    // For attention and memory consolidation
    std::vector<float> attentionTrace;
    std::vector<float> memoryTrace;
    
    Impl() 
        : level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.1f)
        , releaseRate(1.0f)
        , attentionGain(0.0f)
        , memoryConsolidationRate(0.0f)
        , corticalExcitability(0.0f) {}
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
    
    // Update downstream effects
    pImpl->attentionGain = pImpl->level * 2.0f;  // ACh strongly modulates attention
    pImpl->memoryConsolidationRate = pImpl->level * 0.5f;  // ACh promotes memory consolidation
    pImpl->corticalExcitability = pImpl->level * 0.3f;  // Moderate excitation
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances LTP and LTP-dependent memory formation
    // Promotes synaptic strengthening for salient experiences
    return 1.0f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay of ACh signals
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update downstream effects
    setLevel(pImpl->level);
}

void Acetylcholine::signalAttention(int inputSize, float salience) {
    // ACh signals go up for novel, salient, or behaviorally relevant inputs
    float burst = salience * std::min(1.0f, static_cast<float>(inputSize) / 10.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst * pImpl->releaseRate);
    
    // Store attention trace for memory consolidation
    pImpl->attentionTrace.push_back(pImpl->level);
    if (pImpl->attentionTrace.size() > 100) {
        pImpl->attentionTrace.erase(pImpl->attentionTrace.begin());
    }
}

void Acetylcholine::signalMemoryConsolidation(float memoryStrength) {
    // ACh promotes memory consolidation when attention is high
    if (pImpl->level > 0.3f) {
        pImpl->memoryConsolidationRate = pImpl->level * memoryStrength * 0.5f;
        
        // Store memory trace
        pImpl->memoryTrace.push_back(pImpl->level);
        if (pImpl->memoryTrace.size() > 100) {
            pImpl->memoryTrace.erase(pImpl->memoryTrace.begin());
        }
        
        NLM_LOG_INFO("ACh promoting memory consolidation: level=" + std::to_string(pImpl->level) + 
                    " strength=" + std::to_string(memoryStrength));
    }
}

float Acetylcholine::getAttentionGain() const {
    return pImpl->attentionGain;
}

float Acetylcholine::getMemoryConsolidationRate() const {
    return pImpl->memoryConsolidationRate;
}

} // namespace nlm