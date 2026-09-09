#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float focusStrength;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), focusStrength(2.0f) {}
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
}

float Acetylcholine::getPlasticityFactor() const {
    // Ach enhances memory consolidation
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::applyAttentionEffect(Brain* brain) {
    if (!brain || pImpl->level < 0.1f) return;
    
    // Ach enhances top-down attention bias
    auto* attention = brain->getAttention();
    if (attention) {
        // Apply stronger top-down bias based on Ach level
        float biasStrength = pImpl->level * pImpl->focusStrength;
        attention->setExcitationStrength(1.5f + biasStrength);
        attention->setInhibitionStrength(0.3f - biasStrength * 0.1f);
    }
}

void Acetylcholine::applyMemoryEffect(Brain* brain) {
    if (!brain || pImpl->level < 0.1f) return;
    
    // Ach enhances memory encoding and consolidation
    // In a real implementation, this would enhance synaptic strengthening during memory formation
    if (brain->getEpisodicMemory()) {
        // Could trigger memory consolidation based on Ach level
        // Higher Ach = better consolidation during encoding
        float consolidationFactor = 0.5f + 0.5f * pImpl->level;
        NLM_LOG_DEBUG("Acetylcholine: Enhancing memory consolidation with factor " + 
                     std::to_string(consolidationFactor));
    }
}

void Acetylcholine::applyArousalEffect(Brain* brain) {
    if (!brain) return;
    
    // Ach modulates wakefulness and attention
    // In a real implementation, this would affect alertness
    if (brain->getDevelopmentSystem()) {
        // Ach can influence developmental attention windows
        NLM_LOG_DEBUG("Acetylcholine: Modulating arousal and attention focus");
    }
}

void Acetylcholine::focusAttention(RegionId region, float strength) {
    // Set attention to focus on a specific region
    setLevel(strength);
}

void Acetylcholine::enhanceEncoding(const std::vector<float>& pattern, float reward) {
    // Simulate enhanced encoding based on pattern and reward
    // Higher reward or novelty = higher encoding efficiency
    float encodingFactor = 0.5f + reward * pImpl->level;
    setLevel(std::min(1.0f, getLevel() + encodingFactor * 0.1f));
}

} // namespace nlm
