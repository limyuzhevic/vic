#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    float attentionGain;
    float memoryBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), attentionGain(0.5f), memoryBoost(0.5f) {}
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
    // Acetylcholine modulates plasticity by enhancing working memory encoding
    // Higher ACh increases plasticity for memory-related processes
    return 0.5f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttentionEvent() {
    // Burst of acetylcholine on attentional events
    pImpl->level = std::min(pImpl->peak, pImpl->level + pImpl->releaseRate);
}

void Acetylcholine::signalNoveltyEvent() {
    // Acetylcholine responds to novel stimuli
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.5f * pImpl->releaseRate);
}

void Acetylcholine::signalWorkingMemoryEvent() {
    // Acetylcholine enhances working memory consolidation
    pImpl->level = std::min(pImpl->peak, pImpl->level + 0.3f * pImpl->releaseRate);
}

} // namespace nlm