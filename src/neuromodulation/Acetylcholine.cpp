#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionGain;
    float memoryConsolidationRate;
    float attentionalFocusDuration;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.05f), attentionGain(0.5f), 
             memoryConsolidationRate(0.3f), attentionalFocusDuration(100.0f) {}
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
    // ACh increases plasticity for memory consolidation
    // Higher ACh = more plasticity for recent experiences
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh dynamics: decay towards baseline, burst on attention-grabbing stimuli
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float novelty, float saliency) {
    // ACh bursts when novel or salient stimuli appear
    float burst = std::clamp((novelty + saliency) * 2.0f, 0.0f, 1.0f);
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Acetylcholine::signalMemoryConsolidation() {
    // Signal that memory consolidation is happening
    // Increases plasticity for recent memories
    pImpl->level = std::min(pImpl->peak, pImpl->level * 1.2f);
}

} // namespace nlm