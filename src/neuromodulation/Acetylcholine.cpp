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
    float memoryConsolidationStrength;
    
    Impl() : brain(nullptr), level(0.0f), baseline(0.0f), peak(1.0f), 
             decayRate(0.1f), releaseRate(1.0f), attentionGain(1.0f), 
             memoryConsolidationStrength(0.8f) {}
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
    // Real ACh plasticity factor: enhances working memory and attention
    // Attentional boost from 1.0 to 1.5x at full level
    return 1.0f + 0.5f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Real ACh dynamics: decay towards baseline, slow with adaptation
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    // Occasionally increase attention with homeostatic plasticity
    if (pImpl->level < 0.1f) {
        pImpl->level += 0.05f * static_cast<float>(dt);
    }
}

void Acetylcholine::enhanceAttention(float strength) {
    // Real attention enhancement: boosts cortical processing
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * pImpl->releaseRate);
    // Also increases attentional gain
    pImpl->attentionGain = 1.0f + pImpl->level * 0.5f;
}

float Acetylcholine::getAttentionGain() const {
    // Return current attentional boost factor
    return pImpl->attentionGain;
}

} // namespace nlm
