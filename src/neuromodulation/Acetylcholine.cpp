#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float attentionGain;
    float memoryConsolidationRate;
    float cholinergicBurst;
    
    Impl() 
        : level(0.0f)
        , baseline(0.05f)
        , peak(1.0f)
        , decayRate(0.08f)
        , attentionGain(1.0f)
        , memoryConsolidationRate(0.5f)
        , cholinergicBurst(1.5f) {}
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
    // ACh enhances plasticity for attentionally relevant signals
    float attentionModulation = pImpl->level * pImpl->attentionGain;
    return 1.0f + attentionModulation * 0.5f;
}

void Acetylcholine::update(TimestepDuration dt) {
    float dtFloat = static_cast<float>(dt);
    
    // Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * dtFloat);
    
    // Spike-based signaling
    if (pImpl->cholinergicBurst > 1.0f) {
        pImpl->level = std::min(pImpl->peak, pImpl->level * pImpl->cholinergicBurst);
        pImpl->cholinergicBurst = 1.0f;
    }
}

void Acetylcholine::signalSpike() {
    pImpl->cholinergicBurst = 1.5f;
}

void Acetylcholine::reset() {
    pImpl->level = 0.0f;
    pImpl->cholinergicBurst = 1.0f;
}

void Acetylcholine::setAttentionGain(float gain) {
    pImpl->attentionGain = gain;
}

void Acetylcholine::setMemoryConsolidationRate(float rate) {
    pImpl->memoryConsolidationRate = rate;
}

} // namespace nlm
