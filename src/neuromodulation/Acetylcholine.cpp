#include "Acetylcholine.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float modulationFactor;
    float attentionStrength;
    float memoryConsolidation;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f),
             modulationFactor(0.9f), attentionStrength(0.5f), memoryConsolidation(0.7f) {}
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
    // Acetylcholine enhances plasticity for memory consolidation
    // More ACh -> better memory encoding and attention
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
    
    // Add biological noise
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-0.1f, 0.1f);
    pImpl->level += dist(gen);
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Acetylcholine::signalAttention(float attentionStrength) {
    pImpl->attentionStrength = std::clamp(attentionStrength, 0.0f, 1.0f);
    
    // Boost ACh level for attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionStrength * 0.5f);
}

void Acetylcholine::signalMemory(float memoryStrength) {
    pImpl->memoryConsolidation = std::clamp(memoryStrength, 0.0f, 1.0f);
    
    // Enhance memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryStrength * 0.3f);
}

} // namespace nlm