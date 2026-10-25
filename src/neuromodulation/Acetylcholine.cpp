#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float attentionLevel;
    float decayRate;
    float enhancementRate;
    float baselineAttention;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , attentionLevel(0.5f)
        , decayRate(0.05f)
        , enhancementRate(2.0f)
        , baselineAttention(0.5f) {}
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
    // Acetylcholine enhances memory consolidation and plasticity
    // Higher ACh increases synaptic strength for recent experiences
    return 0.8f + 0.4f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention level based on ACh concentration
    pImpl->attentionLevel = pImpl->baselineAttention + pImpl->level * 0.5f;
}

void Acetylcholine::enhanceAttention(float attentionBoost) {
    pImpl->attentionLevel += attentionBoost * pImpl->enhancementRate;
    pImpl->attentionLevel = std::min(1.0f, pImpl->attentionLevel);
    pImpl->level = std::min(2.0f, pImpl->level + attentionBoost);
}

float Acetylcholine::getAttentionLevel() const {
    return pImpl->attentionLevel;
}

} // namespace nlm
