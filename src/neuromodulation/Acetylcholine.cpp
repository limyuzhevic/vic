#include "Neuromodulator.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float decayRate;
    float attentionGain;
    class RandomGenerator* rng;
    
    Impl() 
        : level(0.1f)  // Baseline ACh level
        , decayRate(0.05f)
        , attentionGain(1.0f)
        , rng(nullptr) {}
};

Acetylcholine::Acetylcholine() : pImpl(new Impl()) {}
Acetylcholine::~Acetylcholine() = default;

const char* Acetylcholine::getName() const {
    return "ACh";
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh modulates plasticity: high ACh increases LTP probability
    return 0.5f + 0.5f * std::min(1.0f, pImpl->level / 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    // ACh decays toward baseline but can be activated by attention tasks
    float decayRate = pImpl->decayRate;
    pImpl->level = std::max(0.0f, pImpl->level - decayRate * static_cast<float>(dt));
    
    // Stochastic fluctuations for realism
    if (pImpl->rng) {
        float fluctuation = pImpl->rng->uniformReal(-0.001f, 0.001f) * static_cast<float>(dt);
        pImpl->level += fluctuation;
        pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
    }
}

void Acetylcholine::signalAttention(float attentionLevel) {
    pImpl->level += attentionLevel * pImpl->attentionGain;
    pImpl->level = std::clamp(pImpl->level, 0.0f, 2.0f);
}

} // namespace nlm