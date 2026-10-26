#include "Acetylcholine.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Attention modulation state
    std::vector<float> attentionWeights;
    std::vector<float> memoryBoosts;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
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
    // ACh modulates plasticity - enhances LTP, reduces LTD
    // Higher ACh increases learning rate
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Apply attention modulation
    applyAttentionModulation();
}

void Acetylcholine::signalAttention(float attentionStrength) {
    // Burst of ACh on novel stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionStrength * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryBoost(float boostStrength) {
    // Enhance memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + boostStrength * pImpl->releaseRate * 0.5f);
    
    // Apply memory boost
    applyMemoryBoost(boostStrength);
}

void Acetylcholine::applyAttentionModulation() {
    // Modulate attention to sensory inputs
    // Higher ACh increases sensory processing priority
    if (pImpl->level > 0.5f) {
        // Strong attention - enhance relevant neural pathways
        for (auto& weight : pImpl->attentionWeights) {
            weight = std::min(1.0f, weight + 0.1f * pImpl->level);
        }
    }
}

void Acetylcholine::applyMemoryBoost(float boostStrength) {
    // Boost memory encoding and consolidation
    for (auto& boost : pImpl->memoryBoosts) {
        boost += boostStrength * 0.3f;
    }
}

float Acetylcholine::getAttentionModulation() const {
    // Compute average attention modulation
    if (pImpl->attentionWeights.empty()) return 0.0f;
    float sum = 0.0f;
    for (float weight : pImpl->attentionWeights) {
        sum += weight;
    }
    return sum / pImpl->attentionWeights.size();
}

float Acetylcholine::getMemoryBoost() const {
    // Compute average memory boost
    if (pImpl->memoryBoosts.empty()) return 0.0f;
    float sum = 0.0f;
    for (float boost : pImpl->memoryBoosts) {
        sum += boost;
    }
    return sum / pImpl->memoryBoosts.size();
}

} // namespace nlm