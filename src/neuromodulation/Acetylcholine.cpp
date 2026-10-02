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
    float attentionWeight;
    float memoryConsolidationRate;
    
    Impl() 
        : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.15f), releaseRate(1.0f)
        , attentionWeight(0.8f), memoryConsolidationRate(0.5f) {}
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
    // Acetylcholine modulates plasticity for memory encoding
    // Higher ACh enhances LTP for attended stimuli
    float normalizedLevel = pImpl->level / pImpl->peak;
    
    // ACh increases plasticity for novel stimuli and attention
    float plasticityFactor = 1.0f + 0.5f * normalizedLevel * pImpl->attentionWeight;
    
    // Apply memory consolidation effect
    plasticityFactor *= (1.0f + pImpl->memoryConsolidationRate * 0.2f);
    
    return std::clamp(plasticityFactor, 0.5f, 2.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    float deltaTime = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, 
                           pImpl->level - pImpl->decayRate * deltaTime);
}

} // namespace nlm