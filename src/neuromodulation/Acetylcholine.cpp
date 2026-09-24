#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float memoryConsolidationStrength;
    float attentionBoost;
    std::vector<float> history;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , memoryConsolidationStrength(1.0f)
        , attentionBoost(0.0f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() {
    delete pImpl;
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
    // ACh enhances memory consolidation and reduces forgetting
    return 0.8f + 0.2f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - 0.1f * static_cast<float>(dt));
    
    // Apply attention boost if active
    if (pImpl->attentionBoost > 0.0f) {
        pImpl->level = std::min(1.0f, pImpl->level + pImpl->attentionBoost * static_cast<float>(dt));
        pImpl->attentionBoost = std::max(0.0f, pImpl->attentionBoost - 0.05f * static_cast<float>(dt));
    }
}

void Acetylcholine::signalAttentionBoost(float boost) {
    pImpl->attentionBoost = std::min(1.0f, boost);
    pImpl->level = std::min(1.0f, pImpl->level + boost);
    NLM_LOG_INFO(std::string("ACh attention boost: ") + std::to_string(boost));
}

void Acetylcholine::signalMemoryConsolidation(float strength) {
    // ACh triggers memory consolidation processes
    pImpl->memoryConsolidationStrength = strength;
    pImpl->level = std::min(1.0f, pImpl->level + strength * 0.3f);
    NLM_LOG_INFO(std::string("ACh memory consolidation: ") + std::to_string(strength));
}

} // namespace nlm
