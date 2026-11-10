#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionWeight;
    float noveltyWeight;
    
    Impl()
        : level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.2f)
        , releaseRate(1.5f)
        , attentionWeight(0.7f)
        , noveltyWeight(0.3f) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl) {}

Acetylcholine::~Acetylcholine() = default;

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = clampLevel(level);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity: optimal level (around 0.3-0.5) enhances learning,
    // very high levels can impair plasticity ("attention narrowing")
    if (pImpl->level < 0.3f) {
        return 0.8f + 0.4f * pImpl->level;  // Increasing from baseline
    } else if (pImpl->level <= 0.6f) {
        return 1.2f;  // Optimal learning zone
    } else {
        return 1.2f - 0.5f * (pImpl->level - 0.6f);  // Decreasing at high levels
    }
}

void Acetylcholine::update(TimestepDuration dt) {
    // Natural decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float intensity) {
    if (intensity > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + intensity * pImpl->attentionWeight);
        NLM_LOG_INFO("ACh attention signal: level = " + std::to_string(pImpl->level));
    }
}

void Acetylcholine::signalNovelty(float noveltyLevel) {
    if (noveltyLevel > 0.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + noveltyLevel * pImpl->noveltyWeight);
        NLM_LOG_INFO("ACh novelty signal: level = " + std::to_string(pImpl->level));
    }
}

} // namespace nlm