#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float attentionModulation;
    float memoryConsolidationEffect;
    float baseline;
    float encodingBoost;
    
    Impl() : level(0.0f), attentionModulation(1.0f), memoryConsolidationEffect(1.0f),
             baseline(0.1f), encodingBoost(1.5f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {
    attentionModulation_ = 1.0f;
    memoryConsolidationEffect_ = 1.0f;
    baseline_ = 0.1f;
}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize() {
    NLM_LOG_INFO("Acetylcholine system initialized");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
    // Update derived effects
    attentionModulation_ = 1.0f + pImpl->level * 0.5f;
    memoryConsolidationEffect_ = 1.0f + pImpl->level * 0.3f;
}

void Acetylcholine::update(double dt) {
    // ACh dynamics: slower decay, builds up with attention
    pImpl->level = std::max(baseline_, pImpl->level - pImpl->level * 0.05f * static_cast<float>(dt));
}

float Acetylcholine::enhanceMemoryEncoding() const {
    // ACh enhances memory encoding and consolidation
    return 1.0f + pImpl->level * pImpl->encodingBoost;
}

float Acetylcholine::increaseAttentionFocus() const {
    // ACh increases attention focus
    return attentionModulation_;
}

void Acetylcholine::clear() {
    pImpl->level = 0.0f;
    attentionModulation_ = 1.0f;
    memoryConsolidationEffect_ = 1.0f;
}

} // namespace nlm
