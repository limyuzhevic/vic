#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionDemandWeight;
    float encodingSensitivity;
    float retrievalBoost;
    
    // Memory state tracking
    float currentMemoryLoad;
    float attentionAllocation;
    float consolidationActive;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(0.8f)
        , decayRate(0.2f)
        , attentionDemandWeight(0.3f)
        , encodingSensitivity(0.4f)
        , retrievalBoost(1.2f)
        , currentMemoryLoad(0.0f)
        , attentionAllocation(0.5f)
        , consolidationActive(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Acetylcholine::getAttentionModulation() const {
    // Higher ACh improves attention focus
    return 1.0f + pImpl->level * 0.5f;
}

float Acetylcholine::getConsolidationStrength() const {
    // Higher ACh strengthens memory consolidation
    return 0.5f + pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt, float attentionDemand, float memoryLoad) {
    pImpl->currentMemoryLoad = memoryLoad;
    
    // Attention demand drives ACh release
    float demandDriven = pImpl->attentionDemandWeight * attentionDemand;
    
    // Memory operations influence ACh
    float memoryDriven = pImpl->encodingSensitivity * memoryLoad;
    
    // Combined input to ACh system
    float input = demandDriven + memoryDriven;
    
    // Smooth update
    pImpl->level += (input - pImpl->level) * 0.15f;
    
    // Decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Update attention allocation based on ACh
    pImpl->attentionAllocation = 0.3f + pImpl->level * 0.7f;
}

void Acetylcholine::signalEncoding(float strength) {
    // ACh release during encoding strengthens memory traces
    float boost = strength * pImpl->retrievalBoost;
    pImpl->level = std::min(pImpl->peak, pImpl->level + boost * 0.3f);
    
    // Promote consolidation
    pImpl->consolidationActive = std::max(pImpl->consolidationActive, strength);
}

void Acetylcholine::signalRetrieval(float strength) {
    // ACh enhances retrieval success
    if (strength > pImpl->attentionAllocation) {
        // If retrieval demand exceeds current attention, boost ACh
        pImpl->level = std::min(pImpl->peak, pImpl->level + strength * 0.2f);
    }
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->currentMemoryLoad = 0.0f;
    pImpl->attentionAllocation = 0.5f;
    pImpl->consolidationActive = 0.0f;
}

} // namespace nlm