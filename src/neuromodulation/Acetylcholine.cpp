#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    class Brain* brain;
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    
    Impl()
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(2.0f)
        , attentionGain(0.5f) {}
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
    // Acetylcholine enhances working memory and attention
    // Higher ACh increases signal-to-noise ratio in attention systems
    return 0.3f + 0.7f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionBoost() {
    // ACh released when attention is needed or when novelty detected
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f * pImpl->releaseRate);
}

void Acetylcholine::signalMemoryConsolidation() {
    // ACh peak during memory consolidation (sleep/rest)
    pImpl->level = pImpl->peak;
}

} // namespace nlm