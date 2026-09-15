#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Memory consolidation parameters
    float consolidationStrength;
    float attentionBoost;
    
    // ACh dynamics
    float acetylcholineSynthesized;
    float acetylcholineReleased;
    float synthesisRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f)
        , consolidationStrength(0.8f), attentionBoost(1.2f)
        , acetylcholineSynthesized(0.0f), acetylcholineReleased(0.0f), synthesisRate(0.05f) {}
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
    // ACh enhances synaptic consolidation and stabilizes memory
    // Higher ACh increases memory retention and attention
    return 0.7f + 0.3f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real acetylcholine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Synthesize ACh
    pImpl->acetylcholineSynthesized = pImpl->synthesisRate;
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // TODO PHASE 2: Implement attention signaling
    // PLACEHOLDER: ACh responds to attention signals
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate);
}

void Acetylcholine::consolidateMemory(float memoryStrength) {
    // TODO PHASE 2: Implement memory consolidation
    // PLACEHOLDER: ACh enhances memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + memoryStrength * 0.1f);
}

float Acetylcholine::getAttentionBoost() const {
    return pImpl->attentionBoost;
}

float Acetylcholine::getConsolidationStrength() const {
    return pImpl->consolidationStrength;
}

void Acetylcholine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->acetylcholineSynthesized = 0.0f;
    pImpl->acetylcholineReleased = 0.0f;
}

} // namespace nlm