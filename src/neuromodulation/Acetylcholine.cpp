#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Memory modulation state
    float attentionGain;
    float memoryConsolidationRate;
    float workingMemoryBoost;
    
    // Learning phase tracking
    float learningPhase;
    
    // Random generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> randDist;
    
    Impl() : level(0.0f), baseline(0.1f), peak(1.0f), decayRate(0.05f), releaseRate(1.0f)
        , attentionGain(1.0f), memoryConsolidationRate(0.0f), workingMemoryBoost(1.0f)
        , learningPhase(0.0f) {
        std::random_device rd;
        rng.seed(rd());
        randDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }
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
    
    // Update attention and memory parameters based on ACh level
    // Higher ACh increases attention and memory consolidation
    pImpl->attentionGain = 1.0f + 2.0f * pImpl->level;
    pImpl->memoryConsolidationRate = 0.01f * pImpl->level;
    pImpl->workingMemoryBoost = 1.0f + 0.5f * pImpl->level;
}

float Acetylcholine::getPlasticityFactor() const {
    // ACh has complex effects on plasticity:
    // - Low ACh promotes exploration and flexible learning
    // - High ACh enhances attention and memory consolidation
    return 0.5f + 0.3f * pImpl->level + 0.2f * (1.0f - pImpl->level);
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Update learning phase based on ACh level
    // ACh correlates with cholinergic activation during learning phases
    pImpl->learningPhase = std::clamp(pImpl->level, 0.0f, 1.0f);
}

} // namespace nlm