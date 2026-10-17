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
    float consolidationWeight;
    std::vector<float> memoryHistory;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.0f)
        , peak(1.0f)
        , decayRate(0.05f)
        , consolidationWeight(0.8f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine system initialized - attention and memory consolidation");
}

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
    // Acetylcholine modulates plasticity and memory consolidation
    // Higher levels enhance memory formation
    return 1.0f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply memory consolidation effects
    if (pImpl->brain && pImpl->level > 0.5f) {
        // Enhance working memory consolidation when acetylcholine is high
        auto* workingMem = pImpl->brain->getWorkingMemory();
        if (workingMem) {
            workingMem->strengthenMemory(pImpl->consolidationWeight);
        }
    }
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Top-down attention signal
    setLevel(std::min(pImpl->peak, pImpl->level + attentionStrength * 0.5f));
}

void Acetylcholine::enhanceMemory(const std::vector<float>& pattern, float strength) {
    // Store pattern in memory history for potential consolidation
    pImpl->memoryHistory.push_back(strength);
}

float Acetylcholine::getConsolidationEffect() const {
    return pImpl->consolidationWeight;
}

void Acetylcholine::setConsolidationWeight(float weight) {
    pImpl->consolidationWeight = std::clamp(weight, 0.0f, 1.0f);
}

const std::vector<float>& Acetylcholine::getMemoryHistory() const {
    return pImpl->memoryHistory;
}

} // namespace nlm
