#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float adaptationRate;
    float attentionModulation;
    
    Impl() : level(0.0f), baseline(0.0f), adaptationRate(0.1f), attentionModulation(0.5f) {}
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
    // TODO PHASE 2: Implement ACh-enhanced plasticity factor
    // PLACEHOLDER: ACh promotes plasticity and memory consolidation
    return 1.0f + pImpl->level * 0.3f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real acetylcholine dynamics
    // PLACEHOLDER: ACh modulates attention and memory consolidation
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->adaptationRate * static_cast<float>(dt));
}

void Acetylcholine::applyToMemory(float activation, float &storedActivation) {
    // TODO PHASE 2: Implement real memory effects
    // PLACEHOLDER: ACh enhances memory storage
    storedActivation = activation * (1.0f + pImpl->level * 0.5f);
}

void Acetylcholine::applyToAttention(float &salience, float &bias) {
    // TODO PHASE 2: Implement attention modulation
    // PLACEHOLDER: ACh increases salience and top-down focus
    salience += pImpl->level * 0.3f;
    bias += pImpl->level * 0.4f;
}

} // namespace nlm