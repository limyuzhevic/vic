#include "Neuromodulator.hpp"
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
    float attentionBoost;
    float memoryBoost;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f), 
             attentionBoost(2.0f), memoryBoost(1.5f) {}
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
    // ACh enhances learning-related plasticity
    // Modulates NMDA receptors, strengthens memory consolidation
    return 1.0f + pImpl->attentionBoost * pImpl->level + (pImpl->memoryBoost - 1.0f) * pImpl->level * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Acetylcholine::signalAttention(float attention) {
    // ACh signals when attention is focused
    // Increases level proportionally to attention
    setLevel(std::min(pImpl->level + attention * pImpl->releaseRate, pImpl->peak));
}

void Acetylcholine::signalMemoryLoad(float memoryLoad) {
    // ACh increases during memory encoding
    // Temporarily boosts level for memory consolidation
    if (memoryLoad > 0.0f) {
        setLevel(pImpl->level + memoryLoad * 0.3f);
    }
}

void Acetylcholine::signalThetaOscillation(float thetaPower) {
    // ACh modulated by theta rhythm (8-12 Hz)
    // Coordinates prefrontal-hippocampal interactions
    // Simple model: level modulated by theta power
    setLevel(std::min(pImpl->level + thetaPower * 0.1f, 1.0f));
}

} // namespace nlm
