#include "Neuromodulator.hpp"
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
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.05f), 
             releaseRate(1.0f), attentionGain(1.0f) {}
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
    // Phase 2: Real acetylcholine-modulated plasticity
    // ACh enhances working memory and attention to salient stimuli
    float modulation = 0.8f + 0.4f * pImpl->level;  // Range: 0.8 - 1.2
    // Clamp to reasonable bounds
    modulation = std::clamp(modulation, 0.5f, 2.0f);
    return modulation;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Real ACh dynamics: slower decay than dopamine
    float decay = pImpl->decayRate * static_cast<float>(dt);
    pImpl->level = std::max(pImpl->baseline, pImpl->level - decay);
}

void Acetylcholine::signalAttention(float salience, float novelty) {
    // Real attention signaling: proportional to stimulus salience
    float burst = std::min(1.0f, salience * pImpl->attentionGain);
    burst *= (1.0f + novelty * 0.5f);  // Novelty amplifies attention
    pImpl->level = std::min(pImpl->peak, pImpl->level + burst);
}

void Acetylcholine::enhanceMemory(const std::vector<float>& encoding) {
    // Enhance memory encoding based on current attention state
    if (pImpl->level > 0.5f) {
        // Amplify encoding strength
        for (float& value : encoding) {
            value *= (1.0f + pImpl->level);
        }
    }
}

} // namespace nlm
