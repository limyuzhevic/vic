#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

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
    // ACh enhances memory consolidation and attention
    // Higher ACh improves synaptic stabilization
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // TODO: Implement real ACh dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attentionSignal) {
    // TODO: Implement attention signaling
    // PLACEHOLDER: Burst of ACh on attention demand
    pImpl->level = std::min(pImpl->peak, pImpl->level + attentionSignal * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryDemand(float memoryDemand) {
    // TODO: Implement memory demand signaling
    // PLACEHOLDER: ACh responds to memory consolidation needs
    pImpl->level = std::max(0.0f, pImpl->level + memoryDemand * pImpl->releaseRate * 0.5f);
}

} // namespace nlm