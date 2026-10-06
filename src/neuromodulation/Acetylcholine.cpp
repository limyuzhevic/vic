#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    bool attentionBoosted;
    bool memoryEncodingEnhanced;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             attentionBoosted(false), memoryEncodingEnhanced(false) {}
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
    // Acetylcholine modulates attention and memory encoding
    // Moderate ACh levels optimize memory consolidation
    float plasticityMod = 1.0f + pImpl->level * 0.5f;
    return std::clamp(plasticityMod, 0.5f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention and memory effects
    if (pImpl->attentionBoosted) {
        // Maintain attention boost
    }
    if (pImpl->memoryEncodingEnhanced) {
        // Maintain memory encoding enhancement
    }
}

void Acetylcholine::boostAttention(float strength) {
    pImpl->attentionBoosted = true;
    setLevel(std::max(getLevel(), strength));
    NLM_LOG_INFO("Acetylcholine boosted attention: level=" + std::to_string(getLevel()) + ", strength=" + std::to_string(strength));
}

void Acetylcholine::enhanceMemoryEncoding(bool enable) {
    pImpl->memoryEncodingEnhanced = enable;
    if (enable) {
        setLevel(std::max(getLevel(), 0.5f));
        NLM_LOG_INFO("Acetylcholine enhanced memory encoding");
    } else {
        NLM_LOG_INFO("Acetylcholine memory encoding enhancement disabled");
    }
}

} // namespace nlm