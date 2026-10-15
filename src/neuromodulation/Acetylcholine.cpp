#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
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
    float attentionModulator;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , baseline(0.1f)
        , peak(1.0f)
        , decayRate(0.05f)
        , releaseRate(0.8f)
        , attentionModulator(0.0f) {}
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
    // ACh enhances attention and memory consolidation, increases plasticity
    // Higher ACh levels increase both LTP and LTD, making learning more flexible
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::enhanceAttention(float intensity) {
    // ACh modulates attention - higher levels increase signal-to-noise ratio
    pImpl->attentionModulator = std::clamp(intensity, 0.0f, 1.0f);
    
    // Also increase ACh level
    if (intensity > pImpl->level) {
        setLevel(intensity * 0.7f + pImpl->level * 0.3f);
    }
}

void Acetylcholine::signalLearning(float predictionError) {
    // ACh responds to prediction errors, particularly negative ones (surprise)
    // This helps focus attention on unexpected events
    if (predictionError > 0.0f) {
        // Positive prediction error (better than expected): increase ACh moderately
        setLevel(std::min(pImpl->peak, pImpl->level + predictionError * pImpl->releaseRate * 0.5f));
    } else {
        // Negative prediction error (worse than expected): increase ACh more strongly
        setLevel(std::min(pImpl->peak, pImpl->level + std::abs(predictionError) * pImpl->releaseRate));
    }
}

float Acetylcholine::getAttentionModulation() const {
    return pImpl->attentionModulator;
}

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Acetylcholine (attention) system initialized");
}

} // namespace nlm