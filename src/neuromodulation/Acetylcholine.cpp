#include "Acetylcholine.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionModulator;  // How much ACh affects attention
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f), attentionModulator(1.0f) {}
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
    // ACh modulates attention and memory consolidation
    // Higher ACh increases attention modulation and memory consolidation
    // ACh affects both attention system and plasticity for newly attended items
    return 1.0f + pImpl->attentionModulator * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionModulation(float attentionStrength) {
    // ACh signals attentional focus
    // Modulates attention system to increase processing of attended stimuli
    pImpl->attentionModulator = std::min(2.0f, 1.0f + attentionStrength * 1.5f);
}

void Acetylcholine::signalMemoryConsolidation(float consolidationSignal) {
    // ACh signals memory consolidation
    // Enhances long-term potentiation for attended/stimulus
    if (consolidationSignal > 0.5f) {
        // Strong consolidation signal
        pImpl->level = std::min(pImpl->peak, pImpl->level + consolidationSignal * 0.5f);
    }
}

} // namespace nlm