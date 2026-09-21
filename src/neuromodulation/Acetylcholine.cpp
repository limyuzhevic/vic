#include "Acetylcholine.hpp"
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
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
    // Real acetylcholine-modulated plasticity factor implementation
    // ACh enhances attention and memory consolidation
    // Modulated by novelty and expectation signals
    
    if (pImpl->level < 0.05f) {
        return 1.0f;  // Baseline plasticity
    }
    
    // Higher ACh increases plasticity for novel stimuli
    float modulation = 1.0f + 1.5f * pImpl->level;  // Up to 2.5x increase
    
    // Enhanced attention focus
    float attentionMod = 1.0f + 0.5f * pImpl->level;  // Up to 1.5x
    
    return std::clamp(modulation * attentionMod, 1.0f, 4.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Real acetylcholine dynamics implementation
    // Based on cholinergic signaling in the brain
    // Links attention to cortical processing
    
    float timeDecay = pImpl->decayRate * static_cast<float>(dt);
    
    // Decay towards baseline exponentially
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-timeDecay);
    
    // Clamp to valid range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

void Acetylcholine::signalNovelty(float novelty) {
    // Signal novel stimulus detection
    // ACh peaks with unexpected or salient stimuli
    // Used for selective attention and learning
    
    if (novelty > 0.0f) {
        // Novelty-triggered burst proportional to surprise
        float burst = std::min(pImpl->peak, novelty * pImpl->releaseRate * 2.0f);
        
        // Increase level (not replace)
        pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
        
        // Maintain novelty sensitivity as baseline
        if (pImpl->baseline == 0.0f) {
            pImpl->baseline = pImpl->level * 0.05f;  // 5% of peak as baseline
        }
    }
}

void Acetylcholine::signalAttentionShift(float attention) {
    // Signal attentional focus to specific regions
    // ACh modulates cortical gain based on task demands
    
    if (attention > 0.0f) {
        // Attention enhancement
        float enhancement = std::min(0.5f, attention * pImpl->releaseRate * 0.5f);
        pImpl->level = std::min(pImpl->peak, pImpl->level + enhancement);
    }
}

} // namespace nlm