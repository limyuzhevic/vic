#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;                    // Current ACh concentration (0-1)
    float baseline;                  // Baseline level (0.1)
    float peak;                      // Peak level (1.0)
    float decayRate;                 // Natural decay rate (0.05)
    float consolidationRate;         // Memory consolidation strength (0.3)
    float attentionGain;             // Attention modulation factor (2.0)
    float learningBoost;             // Plasticity enhancement (1.5)
    
    Impl() 
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , consolidationRate(0.3f)
        , attentionGain(2.0f)
        , learningBoost(1.5f) {}
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
    // ACh enhances learning: boosts LTP, suppresses LTD
    // Formula: base_factor * (1 + attention_gain * level)
    float baseFactor = 1.0f;  // Normal plasticity
    return baseFactor * (1.0f + pImpl->attentionGain * pImpl->level);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                          pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::consolidateMemory(const std::vector<float>& memoryTrace, float strength) {
    if (memoryTrace.empty() || strength <= 0.0f) return;
    
    // Enhanced memory consolidation with ACh
    float consolidation = strength * pImpl->consolidationRate * pImpl->level;
    
    // Store consolidated memory
    for (size_t i = 0; i < std::min(size_t(10), memoryTrace.size()); ++i) {
        float enhancedTrace = memoryTrace[i] * (1.0f + consolidation);
        // In real implementation, this would store to episodic memory
        // For now, we just log the consolidation
        if (consolidation > 0.5f) {
            NLM_LOG_INFO("ACh memory consolidation: trace[" + std::to_string(i) + "] enhanced by " + 
                        std::to_string(consolidation * 100) + "%");
        }
    }
}

float Acetylcholine::modulateAttention(float baselineAttention, float novelty) const {
    // ACh modulates attention: increases with novelty, enhances baseline
    float noveltyResponse = std::min(1.0f, novelty * 2.0f);  // Scale novelty
    return baselineAttention * (1.0f + pImpl->attentionGain * pImpl->level * noveltyResponse);
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
}

} // namespace nlm
