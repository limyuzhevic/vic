#include "Acetylcholine.hpp"

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float decayRate;
    float consolidationWeight;
    
    Impl() : level(0.0f), decayRate(0.05f), consolidationWeight(0.8f) {}
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
    // ACh enhances plasticity, especially for memory-related processes
    return 1.0f + 2.0f * pImpl->level;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay level over time
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Acetylcholine::getConsolidationWeight() const {
    return pImpl->consolidationWeight;
}

void Acetylcholine::setConsolidationWeight(float weight) {
    pImpl->consolidationWeight = std::clamp(weight, 0.0f, 1.0f);
}

void Acetylcholine::enhanceMemoryEncoding(float novelty) {
    // Increase ACh level based on novelty for memory encoding
    setLevel(std::min(1.0f, getLevel() + 0.2f * novelty));
}

void Acetylcholine::promoteAttention(float focusLevel) {
    // Increase ACh level based on attention focus
    setLevel(std::min(1.0f, getLevel() + 0.1f * focusLevel));
}

} // namespace nlm