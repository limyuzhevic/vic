#include "../src/neuromodulation/Acetylcholine.hpp"
#include "../src/core/Logger/Logger.hpp"
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
    float attentionWeight;
    float memoryBoost;
    
    Impl() : 
        brain(nullptr),
        level(0.0f),
        baseline(0.0f),
        peak(1.0f),
        decayRate(0.1f),
        releaseRate(1.0f),
        attentionWeight(0.8f),
        memoryBoost(1.5f) {}
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
    // Acetylcholine enhances memory consolidation and attentional plasticity
    // High ACh = high attention, strong memory encoding
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays and modulates neural excitability
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float salience) {
    // ACh bursts with stimulus salience
    pImpl->level = std::min(pImpl->peak, pImpl->level + salience * pImpl->releaseRate);
}

void Acetylcholine::enhanceMemory(float duration) {
    // ACh boosts memory consolidation during encoding
    setLevel(std::min(1.0f, getLevel() + duration * 0.1f * pImpl->memoryBoost));
}

} // namespace nlm