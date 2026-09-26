#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulator;
    float memoryConsolidationRate;
    
    Impl(std::shared_ptr<Config> config)
        : level(0.0f) {
        // Read baseline from config with fallback
        baseline = 0.0f;  // Default hardcoded value
        if (config) {
            // Try to get acetylcholine_baseline from config
            if (auto optBaseline = config->get<float>("acetylcholine_baseline")) {
                baseline = *optBaseline;
            }
        }
        peak = 1.0f;
        decayRate = 0.1f;
        releaseRate = 1.0f;
        attentionModulator = 1.0f;
        memoryConsolidationRate = 1.0f;
    }
};

Acetylcholine::Acetylcholine(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

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

float Acetylcholine::getBaseline() const {
    return pImpl->baseline;
}

void Acetylcholine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh enhances attention and learning: Higher levels increase plasticity for attended stimuli
    float attentionEffect = 0.5f + 0.5f * pImpl->attentionModulator;
    float memoryEffect = 0.5f + 0.5f * pImpl->memoryConsolidationRate;
    return std::min(2.0f, attentionEffect * memoryEffect);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update attention modulation based on current level
    pImpl->attentionModulator = 0.5f + 0.5f * pImpl->level;
    
    // Memory consolidation effect
    pImpl->memoryConsolidationRate = 0.3f + 0.7f * pImpl->level;
}

void Acetylcholine::signalAttentionBurst() {
    // Strong attention signal increases ACh level
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f * pImpl->releaseRate);
}

void Acetylcholine::enhanceMemoryConsolidation(float duration) {
    // Enhance memory consolidation for the given duration
    pImpl->memoryConsolidationRate = std::min(2.0f, pImpl->memoryConsolidationRate + duration * 0.1f);
}

} // namespace nlm
