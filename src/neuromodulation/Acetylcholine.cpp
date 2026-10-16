#include "Acetylcholine.hpp"
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
    float attentionGain;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), attentionGain(1.5f) {}
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
    // ACh modulates attention and memory encoding
    // Higher ACh increases attention gain and working memory encoding
    float attentionFactor = 1.0f + pImpl->level * pImpl->attentionGain;
    return std::min(2.0f, attentionFactor);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays towards baseline, increases on novel stimuli
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalNovelty(float novelty) {
    // ACh increases on novel stimuli
    if (novelty > 0.5f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level + (novelty - 0.5f) * pImpl->releaseRate * 2.0f);
    }
}

void Acetylcholine::enhanceWorkingMemory(const std::vector<float>& pattern) {
    // ACh enhances working memory encoding
    if (!pattern.empty()) {
        float totalActivation = 0.0f;
        for (float val : pattern) {
            totalActivation += std::abs(val);
        }
        
        float memoryBoost = totalActivation * 0.2f * pImpl->level;
        // This would be applied to working memory system
        // In real implementation, this would modify memory trace strength
    }
}

void Acetylcholine::modulateAttention(float globalInhibition) {
    // ACh modulates global inhibition level for attention
    // Lower inhibition = more focused attention
    float attentionLevel = globalInhibition * (2.0f - pImpl->level);
    // This would be applied to attentional selection system
}

} // namespace nlm